#pragma once
#include "ui/common.hpp"
#include "Button.hpp"

namespace ui {

/// Default button with text inside, with defined colors.
struct ColoredTextButton : public Button
{
	const char* text;
	color_t textColor = defaultTextColor;
	color_t borderColor = defaultBorderColor;
	color_t defaultBackgroundColor = defaultBackgroundColor;
	color_t pressedBackgroundColor = defaultPressedBackgroundColor;

	inline ColoredTextButton(
		uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		const char* text, color_t textColor = defaultTextColor, 
		color_t borderColor = defaultBorderColor
	) : 
		Button(x, y, w, h), 
		text(text), textColor(textColor), borderColor(borderColor)
	{}

	virtual void draw() override;
	virtual void onPressDown(uint16_t sx, uint16_t sy) override;

private:
	void draw(uint16_t backgroundColor);
};

}
