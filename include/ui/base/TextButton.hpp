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

	virtual void render() override;
	virtual void onPressDown(uint16_t hx, uint16_t hy) override;

private:
	void render(uint16_t backgroundColor);
};

}
