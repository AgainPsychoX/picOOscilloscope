#include "ui/base/Button.hpp"
#include "ui/tft.hpp"

namespace ui {

void Button::draw()
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK);
}

void Button::onPressDown(uint16_t hx, uint16_t hy)
{
	(void) hx; (void) hy; // unused
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_DARKGREY);
}

void Button::onPressUp(uint16_t hx, uint16_t hy)
{
	if (isHit(hx, hy)) {
		action();
	}
	draw();
}

}
