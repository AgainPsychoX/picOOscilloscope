#pragma once
#include "common.hpp"

namespace sampling {

namespace voltage {

struct Range
{
	uint8_t id = 0;

	inline bool s0Value() const { return id & 0b01; }
	inline bool s1Value() const { return id & 0b10; }

	float minVoltage() const;
	float maxVoltage() const;

	Range() = default;
	Range(uint8_t id);

	Range(bool s1Value, bool s0Value)
		: id((s1Value << 1) | s0Value)
	{}

	inline bool operator==(const Range& other) const { return id == other.id; }
	Range operator+(int8_t direction) const;
};

struct Shifter
{
	const uint8_t s0Pin;
	const uint8_t s1Pin;

	Shifter(uint8_t s0Pin, uint8_t s1Pin) : s0Pin(s0Pin), s1Pin(s1Pin) {}

	void init();
	void set(Range range);
	Range get();
};

extern Shifter shifter[2]; // one per channel

void init();

}

enum class Resolution : uint8_t
{
	U8 = 8U,
	U12 = 12U,
};
extern Resolution resolution;

class ChannelSelection
{
public:
	enum Value : uint8_t
	{
		None            = 0b00000,
		OnlyFirst       = 0b00001,
		OnlySecond      = 0b00010,
		BothSeparate    = 0b00011,
		BothTogether    = 0b10011
	};

	constexpr bool first() const { return value & OnlyFirst; }
	constexpr bool second() const { return value & OnlySecond; }
	constexpr bool single() const { return first() ^ second(); }
	constexpr bool both() const { return value & BothSeparate; }
	constexpr bool together() const { return value & 0b10000; }

	ChannelSelection() = default;
	constexpr ChannelSelection(Value value) : value(value) {}
	constexpr operator Value() const { return value; }
	explicit operator bool() const = delete;
private:
	Value value;
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
void deinit();

void start();
void stop();

}
