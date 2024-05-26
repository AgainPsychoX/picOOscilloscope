#pragma once
#include <ui/base/Button.hpp>
#include <ui/common.hpp>

namespace ui {

struct ChannelButton : public Button
{
	ChannelButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: Button(x, y, w, h)
	{}

	virtual void draw() override;

	virtual void action() override;
	virtual void onPressDown(uint16_t sx, uint16_t sy) override;

private:
	void draw(color_t backgroundColor);

	void updateVoltageShifterInput(); // glue
};

}
