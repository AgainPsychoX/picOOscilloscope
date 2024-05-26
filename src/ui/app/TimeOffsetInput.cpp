#include "ui/app/TimeOffsetInput.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/GraphDispatch.hpp"
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

////////////////////////////////////////////////////////////////////////////////
// Glue

extern TimeBaseInput timeBaseInput;
extern GraphDispatch graphDispatch;

uint16_t TimeOffsetInput::getOptimalStep() const
{
	// TODO: figure more exact solution
	//  this will miss sometimes: flooring because too little, ceiling too much
	uint32_t step = timeBaseInput.value / graphDispatch.getCellWidth();
	if (step < 1) 
		return 1;
	return step;
}

}
