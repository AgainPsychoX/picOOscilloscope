#include "ui/base.hpp"
#include <TFT_eSPI.h>

extern TFT_eSPI tft; // from main

namespace ui {

struct DummyButton : public TextButton 
{
	virtual void action() override
	{
		Serial.print(text);
		Serial.println(" action!");
	}

	DummyButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* text)
		: TextButton(x, y, w, h, text)
	{}
};

// TODO: 2 text lines buttons
// TODO: hit box in the middle

RootGroup root {
	new DummyButton(380, 0,   100, 40, " - tDIV=2us + "),
	new DummyButton(380, 40,  100, 40, "Przycisk 2"),
	new DummyButton(380, 80,  100, 40, "Przycisk 3"),
	new DummyButton(380, 120, 100, 40, "Przycisk 4"),
	new DummyButton(380, 160, 100, 40, "Trigger: OFF  "), // off, 1 or repeat
	new DummyButton(380, 200, 100, 40, "_/ \\_ _||_ |_|"),
	new DummyButton(380, 240, 100, 40, " -  2.00 V  + "),
	new DummyButton(380, 280, 100, 40, " -   2 us   + "),
	/**
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

}
