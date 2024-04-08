#pragma once
#include "common.hpp"

namespace sampling {

enum class Resolution : uint8_t
{
	U8 = 8U,
	U12 = 12U,
};
extern Resolution resolution;

enum class ChannelSelection : uint8_t
{
	None        = 0,
	OnlyFirst   = 1,
	OnlySecond  = 2,
	Both        = 4,
};
extern ChannelSelection channelSelection;

extern uint16_t samplesBuffer[40000];

size_t numberOfSamples();

/// Returns ADC clock base in Hz
uint32_t clockBase();

/// Clock divisor for ADC. Values below 96 means the same (fastest possible), 
/// because 96 clock cycles are required for the conversion to finish.
extern uint16_t clockDivisor;

/// Returns total (for both channels) sample rate (per second)
uint32_t totalSampleRate();

/// Returns sample rate for single channel
uint32_t sampleRate();

bool isInitialized();

void init();

void stop();

void deinit();

void reinit();

}
