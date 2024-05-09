#pragma once
#include "common.hpp"

namespace sampling::voltage {

/// Represents possible setting for given voltage shifter.
/// 
/// 4 possible states, representing control values for two `74HC4052`s chips,
/// that select resistors used to manipulate input voltages.
struct Range
{
	uint8_t id = 0;

	inline bool s0Value() const { return id & 0b01; }
	inline bool s1Value() const { return id & 0b10; }

	/// Returns minimal (low) voltage supported by the range in millivolts
	int16_t minVoltage() const;
	/// Returns maximal (high) voltage supported by the range in millivolts.
	int16_t maxVoltage() const;

	uint16_t width() const
	{
		return static_cast<uint16_t>(-minVoltage()) + maxVoltage();
	}

	Range() = default;
	Range(uint8_t id);

	Range(bool s1Value, bool s0Value)
		: id((s1Value << 1) | s0Value)
	{}

	inline bool operator==(const Range& other) const { return id == other.id; }
	Range operator+(int8_t direction) const;
};

}
