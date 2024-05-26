#pragma once
#include <ui/base/TextButton.hpp>
#include <ui/common.hpp>

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

	virtual void draw() override;
	virtual void action() override {}
	virtual void onPressDown(uint16_t sx, uint16_t sy) override;

private:
	void draw(color_t backgroundColor);
};

}
