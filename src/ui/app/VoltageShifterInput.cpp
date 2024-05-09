#include "ui/app/VoltageShifterInput.hpp"
#include <cstdio> // snprintf
#include "sampling/ChannelSelection.hpp"
#include "sampling/voltage.hpp"

namespace ui {

const char* VoltageShifterInput::valueString()
{
	using namespace sampling;

	voltage::Range range;
	switch (channelSelection) {
		default:
		case ChannelSelection::None:
			return "wybierz kanal"; // TODO: Polish ł
		case ChannelSelection::OnlyFirst:
			range = voltage::shifter[0].get();
			break;
		case ChannelSelection::OnlySecond:
			range = voltage::shifter[1].get();
			break;
		case ChannelSelection::BothSeparate:
		case ChannelSelection::BothTogether:
			range = voltage::shifter[0].get();
			auto otherRange = voltage::shifter[1].get();
			if (range == otherRange) {
				// Continue with displaying voltage
				break;
			}
			else {
				return "rozne"; // TODO: Polish ó
			}
	}

	// Convert mV to string (with sign, up to 2 decimal places, max width 4)
	// TODO: consider avoiding printf here? https://godbolt.org/z/8o4Escvhb
	int w = sprintf(sharedBuffer, "%+.4d", range.minVoltage());
	char* p = sharedBuffer + w - 5;
	p[4] = p[3];
	p[3] = p[2];
	p[2] = '.';
	sharedBuffer[5] = ' ';
	w = sprintf(sharedBuffer + 6, "%+.4d", range.maxVoltage());
	p = sharedBuffer + 6 + w - 5;
	p[4] = p[3];
	p[3] = p[2];
	p[2] = '.';
	sharedBuffer[11] = 0;

	return sharedBuffer;
}

void VoltageShifterInput::onLeftAction()
{
	move(-1);
}

void VoltageShifterInput::onRightAction()
{
	move(1);
}

void VoltageShifterInput::move(int8_t direction)
{
	using namespace sampling;
	switch (channelSelection) {
		default:
		case ChannelSelection::None:
			return; // do nothing
		case ChannelSelection::OnlyFirst:
			voltage::shifter[0].set(voltage::shifter[0].get() + direction);
			break;
		case ChannelSelection::OnlySecond:
			voltage::shifter[1].set(voltage::shifter[1].get() + direction);
			break;
		case ChannelSelection::BothSeparate:
		case ChannelSelection::BothTogether:
			auto a = voltage::shifter[0].get();
			auto b = voltage::shifter[1].get();
			if (a == b) {
				// Continue, changing both channels from current value
				auto previous = a + direction;
				voltage::shifter[0].set(previous);
				voltage::shifter[1].set(previous);
			}
			else {
				// Continue, changing both channels from safe/widest range
				auto safe = voltage::Range();
				voltage::shifter[0].set(safe);
				voltage::shifter[1].set(safe);
			}
			break;
	}
	updateGraphs();
}

}
