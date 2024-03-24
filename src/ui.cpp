#include "ui.hpp"
#include <TFT_eSPI.h>
#include <array>

extern TFT_eSPI tft; // from main

namespace ui {

////////////////////////////////////////////////////////////////////////////////

void Button::render()
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK);
}

void Button::onPressDown(uint16_t x, uint16_t y)
{
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_DARKGREY);
}

void Button::onPressUp(uint16_t x, uint16_t y)
{
	if (isHit(x, y)) {
		action();
	}
	render();
}

////////////////////////////////////////////////////////////////////////////////

void TextButton::render()
{
	render(TFT_BLACK);
}

void TextButton::onPressDown(uint16_t x, uint16_t y)
{
	(void) x; (void) y; // unused
	render(TFT_DARKGREY);
}

void TextButton::render(uint16_t backgroundColor)
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	uint8_t tempdatum = tft.textdatum;
	tft.textdatum = CC_DATUM;
	tft.drawString(text, x + w / 2, y + h / 2);
	tft.textdatum = tempdatum;
}

////////////////////////////////////////////////////////////////////////////////

void Text::render()
{
	tft.drawString(text, x, y);
}

////////////////////////////////////////////////////////////////////////////////

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

RootGroup root {
	new DummyButton(400, 0, 80, 40, "Przycisk 1"),

	// DummyButton(400, 0,   80, 40, " - tDIV=2us + "),
	// DummyButton(400, 40,  80, 40, "Przycisk 2"),
	// DummyButton(400, 80,  80, 40, "Przycisk 3"),
	// DummyButton(400, 120, 80, 40, "Przycisk 4"),
	// DummyButton(400, 160, 80, 40, "Trigger: OFF  "), // off, 1 or repeat
	// DummyButton(400, 200, 80, 40, "_/ \\_ _||_ |_|"),
	// DummyButton(400, 240, 80, 40, " -  2.00 V  + "),
	// DummyButton(400, 280, 80, 40, " -   2 us   + "),
};

}
