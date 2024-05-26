#include "ui/app/DummyButton.hpp"
#include "tft.hpp"

namespace ui {

void TwoLineDummyButton::draw()
{
	draw(TFT_BLACK);
}

void TwoLineDummyButton::draw(color_t backgroundColor)
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextColor(TFT_WHITE, backgroundColor);
	auto fh = tft.fontHeight();
	auto xc = x + w / 2;
	auto yc = y + h / 2;
	if (secondLine) {
		tft.setTextDatum(TC_DATUM);
		tft.drawString(firstLine, xc, yc - fh);
		tft.drawString(secondLine, xc, yc);
	}
	else {
		tft.setTextDatum(CC_DATUM);
		tft.drawString(firstLine, xc, yc);
	}
}

void TwoLineDummyButton::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused
	draw(TFT_DARKGREY);
}

}
