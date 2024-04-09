#include "ui/base.hpp"
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

}
