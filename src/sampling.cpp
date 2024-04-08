#include "sampling.hpp"
#include <hardware/adc.h>
#include <hardware/dma.h>
#include <hardware/clocks.h>

namespace sampling {

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


ChannelSelection channelSelection;

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
		case ChannelSelection::None:        return 0; // invalid value
		case ChannelSelection::OnlyFirst:
		case ChannelSelection::OnlySecond:  return totalSampleRate();
		case ChannelSelection::Both:        return totalSampleRate() / 2;
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
			case ChannelSelection::Both: {
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
