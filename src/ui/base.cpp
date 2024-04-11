#include "ui/base.hpp"
#include <TFT_eSPI.h>
#include <array>

extern TFT_eSPI tft; // from main

namespace ui {

// TODO: few macros to configure base colors?

////////////////////////////////////////////////////////////////////////////////

void Button::render()
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
	render();
}

////////////////////////////////////////////////////////////////////////////////

void TextButton::render()
{
	render(TFT_BLACK);
}

void TextButton::onPressDown(uint16_t hx, uint16_t hy)
{
	(void) hx; (void) hy; // unused
	render(TFT_DARKGREY);
}

void TextButton::render(uint16_t backgroundColor)
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(TFT_WHITE, backgroundColor);
	tft.drawString(text, x + w / 2, y + h / 2);
}

////////////////////////////////////////////////////////////////////////////////

void RangeHorizontalButton::render()
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK);
	renderForeground();
}

void RangeHorizontalButton::onPressDown(uint16_t hx, uint16_t hy)
{
	(void) hy; // unused
	color_t leftColor = TFT_BLACK;
	color_t rightColor = TFT_DARKGREY;
	pressStartedOnLeft = isLeft(hx);
	if (pressStartedOnLeft) {
		std::swap(leftColor, rightColor);
	}
	tft.fillRectHGradient(x + 1, y + 1, w - 2, h - 2, leftColor, rightColor);
	renderForeground();
}

void RangeHorizontalButton::onPressUp(uint16_t hx, uint16_t hy)
{
	if (isHit(hx, hy)) {
		if (isLeft(hx) == pressStartedOnLeft) {
			if (pressStartedOnLeft) {
				onLeftAction();
			}
			else {
				onRightAction();
			}
		}
	}
	render();
}

void RangeHorizontalButton::renderForeground()
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

////////////////////////////////////////////////////////////////////////////////

void Text::render()
{
	tft.drawString(text, x, y);
}

////////////////////////////////////////////////////////////////////////////////

}
