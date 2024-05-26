#pragma once
#include <cstdint>
#include <cstddef>
#include "sampling/Resolution.hpp"
#include "sampling/ChannelSelection.hpp"
#include "ArrayCircularIterator.hpp"

namespace sampling {

////////////////////////////////////////
// Samples storage

/// Buffer for the samples. 
/// If `Resolution::U8` is used then should be reinterpreted as `uint8_t` 
/// with double the length, as each `uint16_t` fits 2 samples; 
/// If `Resolution::U12` is used then it's original array of `uint16_t`.
extern uint16_t samplesBuffer[40000];

/// Pointer to (last) sampling window start (inside the samples buffer), 
/// where first sample is being stored. 
extern void* windowStart;

/// Returns current number of samples, for specific type (sample width).
template <typename T>
constexpr inline size_t getNumberOfSamples()
{
	// The -Wsizeof-array-div warning suppression using extra parenthesis 
	// (which is suggested by in the warning itself) doesn't work inside templated code 
	// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=114983
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-array-div"
	return sizeof(samplesBuffer) / (sizeof(T));
#pragma GCC diagnostic pop
}

/// Type for samples buffer circular iterator with specific type (sample width).
template <typename T>
using SamplesCircularIterator = ArrayCircularIterator<T, getNumberOfSamples<T>()>;

/// Prepares samples buffer circular iterator at window start position,
/// with specific type (sample width).
template <typename T>
inline auto getSamplesIterator()
{
	return SamplesCircularIterator<T> {
		reinterpret_cast<T*>(samplesBuffer),
		reinterpret_cast<T*>(windowStart),
	};
}

/// Returns current number of samples, depending on currently set resolution.
inline size_t getNumberOfSamples()
{
	switch (resolution) {
		default:
		case Resolution::U8:    return getNumberOfSamples<uint8_t>();
		case Resolution::U12:   return getNumberOfSamples<uint16_t>();
	}
}

/// Returns number of samples that should be captured and preserved for user
/// to view before the trigger point.
inline size_t getNumberOfPreTriggerSamples()
{
	return getNumberOfSamples() / 8; // TODO: make the fraction configurable
}

////////////////////////////////////////
// Sampling settings

/// Returns ADC currently selected clock base in Hz.
uint32_t getClockBase();

// TODO: getClockSource & setClockSource

/// Returns clock divisor for ADC.
uint16_t getClockDivisor();
/// Sets clock divisor for the ADC. Values below 96 means the same (fastest), 
/// because 96 clock cycles are required for the conversion to finish.
/// Takes effect after (re)initialisation the sampling module.
void setClockDivisor(uint16_t value);

/// Currently selected total (for both channels) sample rate (per second). 
/// To change, se `setClockDivisor()` and/or`setClockSource()`.
/// Takes effect after (re)initialisation the sampling module.
uint32_t getSampleRate();
/// Calculates sample rate based on given clock divisor.
uint32_t calculateSampleRate(uint16_t clockDivisor);

/// Returns time between samples in microseconds
uint32_t getTimeBetweenSamples();
/// Calculates sample rate based on given clock divisor.
uint32_t calculateTimeBetweenSamples(uint32_t sampleRate);

////////////////////////////////////////
// Control

bool isInitialized();

void init();
void deinit();

void start();
void stop();

////////////////////////////////////////////////////////////////////////////////

}
