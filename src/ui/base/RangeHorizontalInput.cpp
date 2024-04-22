#include "ui/base/RangeHorizontalInput.hpp"
#include "ui/tft.hpp"

namespace ui {

void RangeHorizontalInput::draw()
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK);
	drawForeground();
}

void RangeHorizontalInput::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sy; // unused
	color_t leftColor = TFT_BLACK;
	color_t rightColor = TFT_DARKGREY;
	pressStartedOnLeft = isLeft(sx);
	if (pressStartedOnLeft) {
		std::swap(leftColor, rightColor);
	}
	tft.fillRectHGradient(x + 1, y + 1, w - 2, h - 2, leftColor, rightColor);
	drawForeground();
}

void RangeHorizontalInput::onPressUp(uint16_t sx, uint16_t sy)
{
	if (isHit(sx, sy)) {
		if (isLeft(sx) == pressStartedOnLeft) {
			if (pressStartedOnLeft) {
				onLeftAction();
			}
			else {
				onRightAction();
			}
		}
	}
	draw();
}

void RangeHorizontalInput::onPressMove(uint16_t sx, uint16_t sy)
{
}

void RangeHorizontalInput::drawForeground()
{
	tft.setTextColor(TFT_WHITE);
	auto fh = tft.fontHeight();
	auto xc = x + w / 2;
	auto yc = y + h / 2;
	if (centerLabel) {
		tft.setTextDatum(TC_DATUM);
		tft.drawString(centerLabel, xc, yc - fh);
	}
	else {
		tft.setTextDatum(CC_DATUM);
	}
	tft.drawString(valueString(), xc, yc);

	if (sidesFontSize) {
		auto textsize = tft.textsize;
		tft.setTextSize(sidesFontSize);
		// if (leftLabel)
		// {
			tft.setTextDatum(sidesAlignToValue ? TL_DATUM : CL_DATUM);
			tft.drawString(leftLabel, x + sidesPadding, yc);
		// }
		// if (rightLabel) {
			tft.setTextDatum(sidesAlignToValue ? TR_DATUM : CR_DATUM);
			tft.drawString(rightLabel, x + w - sidesPadding, yc);
		// }
		tft.textsize = textsize; // restore
	}
}

}
