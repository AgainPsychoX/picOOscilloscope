#pragma once
#include "sampling/voltage/Range.hpp"

namespace sampling::voltage {

/// Represents module that performs voltage shifting for single channel.
struct Shifter
{
	const uint8_t s0Pin; /// GPIO pin to 74HC4052 S0 input
	const uint8_t s1Pin; /// GPIO pin to 74HC4052 S1 input

	Shifter(uint8_t s0Pin, uint8_t s1Pin) : s0Pin(s0Pin), s1Pin(s1Pin) {}

	void init();
	void set(Range range);
	Range get();
};

}
