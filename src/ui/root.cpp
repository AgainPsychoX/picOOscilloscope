#include "ui/root.hpp"
#include "ui/app/ChannelButton.hpp"
#include "ui/app/VoltageShifterInput.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/TimeOffsetInput.hpp"
#include "ui/app/DummyButton.hpp"
#include "ui/app/VoltageGraph.hpp"
#include "ui/app/GraphDispatch.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Graphs

VoltageGraph singleGraph { 0, 0, 380, 320, 10, 12 };
VoltageGraph firstSplitGraph { 0, 0, 380, 160, 10, 12 };
VoltageGraph secondSplitGraph { 0, 160, 380, 160, 10, 12 };
// TODO: union? single graph OR 2x split graphs

GraphDispatch graphDispatch;

////////////////////////////////////////////////////////////////////////////////
// Top level menu

ChannelButton channelButton { 380, 0, 100, 40 };
VoltageShifterInput voltageShifterInput { 380, 40, 100, 40 }; 
TimeBaseInput timeBaseInput { 380, 80, 100, 40 };
TimeOffsetInput timeOffsetInput { 380, 120, 100, 40 };

struct RefreshButton : public DummyButton
{
	RefreshButton()
		: DummyButton(380, 280, 100, 40, "Refresh")
	{}

	virtual void action() override
	{
		graphDispatch.draw();
	}
};

#ifndef PIO_UNIT_TESTING

RootGroup root {
	&graphDispatch,
	&channelButton,
	&voltageShifterInput,
	&timeBaseInput,
	&timeOffsetInput,
	new TwoLineDummyButton(380, 160, 100, 40, "", ""),
	new TwoLineDummyButton(380, 200, 100, 40, "", ""),
	new TwoLineDummyButton(380, 240, 100, 40, "", ""),
	new RefreshButton(),
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
	 *      7) Podstawa czasu???
	 *      8) < Wróć 
	*/
};

#else // PIO_UNIT_TESTING

RootGroup root;

#endif // PIO_UNIT_TESTING

}
