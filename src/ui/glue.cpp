#include "ui/app/ChannelButton.hpp"
#include "ui/app/VoltageShifterInput.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/TimeOffsetInput.hpp"
#include "ui/app/GraphDispatch.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Extra glue code between active elements

extern VoltageShifterInput voltageShifterInput;
extern TimeBaseInput timeBaseInput;
extern TimeOffsetInput timeOffsetInput;
extern GraphDispatch graphDispatch;

void ChannelButton::updateVoltageShifterInput()
{
	voltageShifterInput.draw();
}

void VoltageShifterInput::updateGraphs()
{
	graphDispatch.setup();
}

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
