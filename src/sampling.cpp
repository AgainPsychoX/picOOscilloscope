#include "sampling.hpp"
#include <algorithm>
#include <hardware/adc.h>
#include <hardware/dma.h>
#include <hardware/clocks.h>

namespace sampling {

namespace voltage
{

// Assuming range ID 0, with S1/S0 control inputs of 74HC4052 both being 0 (LOW), 
// is default and safe (widest range). 
static const float _minVoltages[] = { -5.872f, -2.152f, -1.120f, -0.404f };
static const float _maxVoltages[] = {  5.917f,  2.497f,  0.949f,  0.585f };
// TODO: couldn't those be int16_t, in mV ?
constexpr uint8_t _optionsCount = sizeof(_maxVoltages) / sizeof(*_maxVoltages);
static_assert(sizeof(_minVoltages) == sizeof(_maxVoltages));

float Range::minVoltage() const
{
	return _minVoltages[id];
}
float Range::maxVoltage() const
{
	return _maxVoltages[id];
}

Range::Range(uint8_t id)
	: id(id < _optionsCount ? id : 0)
{}

Range Range::operator+(int8_t direction) const
{
	return Range(std::clamp(id + direction, 0, _optionsCount - 1));
}

void Shifter::init()
{ 
	uint32_t mask = (1 << s0Pin) | (1 << s1Pin);
	gpio_init_mask(mask);
	gpio_set_dir_out_masked(mask);
	gpio_put_masked(mask, 0);
}

void Shifter::set(Range range)
{
	uint32_t value = (range.s1Value() << s1Pin) | (range.s0Value() << s0Pin);
	uint32_t mask = (1 << s1Pin) | (1 << s0Pin);
	gpio_put_masked(mask, value);
}

Range Shifter::get()
{
	return Range((!!gpio_get(s0Pin)) | ((!!gpio_get(s1Pin)) << 1));
}

Shifter shifter[2] {
	{ 18, 19 }, // First channel uses GPIO 18 & 19
	{ 16, 17 }, // Second channel uses GPIO 16 & 17
};

void init()
{
	for (auto& s : shifter) 
		s.init();
}

}

constexpr uint adc1Channel = 0;
constexpr uint adc2Channel = 1;
constexpr uint adc1GPIOPin = 26;
constexpr uint adc2GPIOPin = 27;

/// DMA channel used to move ADC collected samples from FIFO to samples buffer.
int sample_dma_ch = -1;

/// DMA channel used to reset address of "sample" DMA channel to the beginning.
int control_dma_ch = -1;

Resolution resolution;

uint16_t samplesBuffer[40000];

void* samplesBufferBeginAddress = &samplesBuffer[0]; // for "control" DMA channel

size_t numberOfSamples()
{
	switch (resolution) {
		default:
		case Resolution::U8:    return sizeof(samplesBuffer);
		case Resolution::U12:   return sizeof(samplesBuffer) / sizeof(*samplesBuffer);
	}
}

static inline dma_channel_transfer_size sampleDMATransferDataSize()
{
	switch (resolution) {
		default:
		case Resolution::U8:    return DMA_SIZE_8;
		case Resolution::U12:   return DMA_SIZE_16;
	}
}

ChannelSelection channelSelection = ChannelSelection::OnlyFirst;

uint32_t clockBase()
{
	return clock_get_hz(clk_adc);
}

uint16_t clockDivisor;

static inline void adc_set_clkdiv_integer(uint16_t div_int) {
	adc_hw->div = static_cast<uint32_t>(div_int) << 8;
}

uint32_t totalSampleRate()
{
	if (clockDivisor < 96)
		return clockBase() / 96;
	else
		return clockBase() / (clockDivisor + 1);
}

uint32_t sampleRate()
{
	switch (channelSelection) {
		default:
		case ChannelSelection::None:
			return 0; // invalid value
		case ChannelSelection::OnlyFirst:
		case ChannelSelection::OnlySecond: 
			return totalSampleRate();
		case ChannelSelection::BothSeparate:
		case ChannelSelection::BothTogether:
			return totalSampleRate() / 2;
	}
}

bool isInitialized()
{
	return sample_dma_ch != -1;
}

void init()
{
	bool isU8 = resolution == Resolution::U8;

	// Setup ADC
	{
		adc_gpio_init(adc1GPIOPin);
		adc_gpio_init(adc2GPIOPin);
		adc_init();
		switch (channelSelection) {
			default:
			case ChannelSelection::None:
				return; // invalid value
			case ChannelSelection::OnlyFirst: {
				adc_select_input(adc1Channel);
				adc_set_round_robin(0);
				break;
			}
			case ChannelSelection::OnlySecond: {
				adc_select_input(adc2Channel);
				adc_set_round_robin(0);
				break;
			}
			case ChannelSelection::BothSeparate:
			case ChannelSelection::BothTogether: {
				adc_select_input(adc1Channel);
				adc_set_round_robin((1 << adc1Channel) | (1 << adc2Channel));
				break;
			}
		}
		adc_fifo_setup(
			true,   // enable the ADC FIFO
			true,   // enable DMA requests
			1,      // DMA request threshold in number of samples
			false,  // if true, sample entry will contain error flag at bit 15
			isU8    // shift to be single byte, only for 8 bit resolution
		);
		adc_set_clkdiv_integer(clockDivisor);
	}

	// Claim the DMA channels
	control_dma_ch = dma_claim_unused_channel(true);
	sample_dma_ch = dma_claim_unused_channel(true);

	// Config sample DMA channel
	{
		dma_channel_config c = dma_channel_get_default_config(sample_dma_ch);
		channel_config_set_read_increment(&c, false);
		channel_config_set_write_increment(&c, true);
		channel_config_set_dreq(&c, DREQ_ADC); // pace based on ADC samples
		channel_config_set_chain_to(&c, control_dma_ch);
		channel_config_set_transfer_data_size(&c, sampleDMATransferDataSize());
		dma_channel_configure(
			sample_dma_ch,      // DMA channel handle
			&c,                 // config structure address
			samplesBuffer,      // source address
			&adc_hw->fifo,      // destination address
			numberOfSamples(),  // number of transfers
			false               // enable
		);
	}

	// Config control channel
	{
		dma_channel_config c = dma_channel_get_default_config(control_dma_ch);
		channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
		channel_config_set_read_increment(&c, false);
		channel_config_set_write_increment(&c, false);
		channel_config_set_chain_to(&c, sample_dma_ch);
		dma_channel_configure(
			control_dma_ch,
			&c,
			&dma_hw->ch[sample_dma_ch].write_addr,
			&samplesBufferBeginAddress,
			1,
			false
		);
	}

	dma_channel_start(sample_dma_ch);
	dma_channel_start(control_dma_ch);
	adc_run(true);
}

void stop()
{
	adc_run(false);
	dma_channel_abort(sample_dma_ch);
}

void deinit()
{
	stop();
	dma_channel_unclaim(sample_dma_ch);
	dma_channel_unclaim(control_dma_ch);
	sample_dma_ch = -1;
	control_dma_ch = -1;
}

void reinit()
{
	// TODO: ...u
}

}
