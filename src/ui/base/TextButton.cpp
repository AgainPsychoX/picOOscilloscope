#include "ui/base/TextButton.hpp"
#include "ui/tft.hpp"

namespace ui {

void TextButton::draw()
{
	draw(TFT_BLACK);
}

void TextButton::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused
	draw(TFT_DARKGREY);
}

void TextButton::draw(uint16_t backgroundColor)
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(TFT_WHITE, backgroundColor);
	tft.drawString(text, x + w / 2, y + h / 2);
}

}
