#include "ui/app/TimeOffsetInput.hpp"
#include "tft.hpp"
#include "utils.hpp"

namespace ui {

const char* TimeOffsetInput::valueString()
{
	assert(value < 1'000'000'000);
	snprintf(sharedBuffer, sizeof(sharedBuffer), "%ld us", value);
	return sharedBuffer;
}


void TimeOffsetInput::onLeftAction()
{
	auto step = getOptimalStep();
	if (value > 0 && value <= step) {
		value = 0;
	}
	else {
		value -= step;
	}
}

void TimeOffsetInput::onRightAction()
{
	auto step = getOptimalStep();
	if (value < 0 && value >= -step) {
		value = 0;
	}
	else {
		value += step;
	}
}

uint16_t TimeOffsetInput::longPressTickInterval() const
{
	return saturatedSubtract<uint32_t>(100, (now - pressStartTime) / 32);
}

}
