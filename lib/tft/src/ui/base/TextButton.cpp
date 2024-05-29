#include "ui/base/TextButton.hpp"
#include "tft.hpp"

namespace ui {

void TextButton::draw()
{
	draw(defaultBackgroundColor);
}

void TextButton::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused
	draw(defaultPressedBackgroundColor);
}

void TextButton::draw(uint16_t backgroundColor)
{
	tft.drawRect(x, y, w, h, defaultBorderColor);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(defaultTextColor, backgroundColor);
	tft.drawString(text, x + w / 2, y + h / 2);
}

}
