#include "ui/base/Button.hpp"
#include "tft.hpp"

namespace ui {

void Button::draw()
{
	tft.drawRect(x, y, w, h, defaultBorderColor);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, defaultBackgroundColor);
}

void Button::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused in base
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, defaultPressedBackgroundColor);
}

void Button::onPressUp(uint16_t sx, uint16_t sy)
{
	if (isHit(sx, sy)) {
		action();
	}
	draw();
}

void Button::onPressMove(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused in base
}

}
