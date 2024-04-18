#include "ui/root.hpp"
#include "ui/app/ChannelButton.hpp"
#include "ui/app/VoltageShifterInput.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/DummyButton.hpp"
#include "ui/app/Graphs.hpp"

namespace ui {

VoltageGraph singleGraph { 0, 0, 380, 320, 10, 12 };
VoltageGraph firstSplitGraph { 0, 0, 380, 160, 10, 12 };
VoltageGraph secondSplitGraph { 0, 160, 380, 160, 10, 12 };
// TODO: union? single graph OR 2x split graphs

ChannelButton channelButton { 380, 0, 100, 40 };
VoltageShifterInput voltageShifterInput { 380, 40, 100, 40 }; 
TimeBaseInput timeBaseInput { 380, 80, 100, 40 };

RootGroup root {
	&channelButton,
	&voltageShifterInput,
	&timeBaseInput,
	// &timeOffsetInput,
	// new TwoLineDummyButton(380, 80,  100, 40, "Podstawa czasu", " -   2 us   + "),
	new TwoLineDummyButton(380, 120, 100, 40,  "Przesuniecie" , " -   30 ms  + "),
	new TwoLineDummyButton(380, 160, 100, 40, "Tryb", " <  Ciagly  > "),
	new TwoLineDummyButton(380, 200, 100, 40, "Trigger type", "_/ \\_ _||_ |_|"),
	new TwoLineDummyButton(380, 240, 100, 40, "Trigger V", " -  2.00 V  + "),
	new TwoLineDummyButton(380, 280, 100, 40, "Trigger dT", " -   2 us   + "),
	/**
	 *  TODO: rethink it once more (or when implementing?)
	 * 
	 *  1) Wybór kanał(ów)
	 * 	2) Wybór napięcie
	 *  3) Podstawa czasu
	 *  4) Przesuniecie
	 *  5) Tryb wyzwalania
	 *  6) Napięcie wyzwalania
	 *  7) Szerokość wyzwalania
	 * 
	 * + dłuższe
	 * 
	 *  8) Inne (sub-menu)
	 *      1) Czas między próbkami (1ms .. 2us)
	 *      2) Częstotliwość próbkowania (732.4Hz, 1kHz .. 500kHz)
	 *      3) Baza zegara ADC (USB PLL 48MHz or SYS PLL 125MHz or even overclock with confirmation prompt?)
	 *      4) Podział zegara ADC (96 .. 65536)
	 *      5) Rozdzielczość ADC (U8/U12)
	 *      6) Częstotliwość testowa (x..y? GPIO 21)
	 *      7) 
	 *      8) < Wróć 
	*/
};

////////////////////////////////////////////////////////////////////////////////
// Extra glue code between active elements

void ChannelButton::updateVoltageShifterInput()
{
	voltageShifterInput.draw();
}

void VoltageShifterInput::updateGraphs()
{
	if (graphDispatch.isSingleGraphActive()) {
		graphDispatch.setupSingleGraph();
	}
	else {
		graphDispatch.setupSplitGraphs();
	}
}

}
