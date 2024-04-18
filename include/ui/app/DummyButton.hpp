#pragma once
#include "ui/base/TextButton.hpp"
#include "ui/common.hpp"

namespace ui {

struct DummyButton : public TextButton 
{
	DummyButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* text)
		: TextButton(x, y, w, h, text)
	{}

	virtual void action() override {}
};

struct TwoLineDummyButton : public Button
{
	const char* firstLine;
	const char* secondLine;

	TwoLineDummyButton(
		uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		const char* firstLine, const char* secondLine)
		: Button(x, y, w, h), firstLine(firstLine), secondLine(secondLine)
	{}

	virtual void render() override;
	virtual void action() override {}
	virtual void onPressDown(uint16_t hx, uint16_t hy) override;

private:
	void render(color_t backgroundColor);
};

}
