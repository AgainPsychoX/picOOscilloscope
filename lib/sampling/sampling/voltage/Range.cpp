#include "sampling/voltage/Range.hpp"
#include <algorithm> // clamp

namespace sampling::voltage {

// Generally code assumption: range ID 0 is default and safe (widest range), 
// as it's state where  S1/S0 control inputs of 74HC4052 both being 0 (LOW). 

// TODO: consider moving this voltage stuff to the header, to allow inline

static const int16_t _minVoltages[] = { -5'872, -2'152, -1'120,   -404 };
static const int16_t _maxVoltages[] = {  5'917,  2'497,    949,    585 };
constexpr uint8_t _optionsCount = sizeof(_maxVoltages) / sizeof(*_maxVoltages);
static_assert(sizeof(_minVoltages) == sizeof(_maxVoltages));

int16_t Range::minVoltage() const
{
	return _minVoltages[id];
}
int16_t Range::maxVoltage() const
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

}
