#include "ui/base/ColoredTextButton.hpp"
#include "tft.hpp"

namespace ui {

void ColoredTextButton::draw()
{
	draw(defaultBackgroundColor);
}

void ColoredTextButton::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused
	draw(pressedBackgroundColor);
}

void ColoredTextButton::draw(uint16_t backgroundColor)
{
	tft.drawRect(x, y, w, h, borderColor);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(textColor, backgroundColor);
	tft.drawString(text, x + w / 2, y + h / 2);
}

}
