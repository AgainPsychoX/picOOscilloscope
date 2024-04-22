#pragma once
#include "Button.hpp"

namespace ui {

/// Default button with text inside
struct TextButton : public Button
{
	const char* text;

	TextButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* text)
		: Button(x, y, w, h), text(text)
	{}

	virtual void draw() override;
	virtual void onPressDown(uint16_t sx, uint16_t sy) override;

private:
	void draw(uint16_t backgroundColor);
};

}
