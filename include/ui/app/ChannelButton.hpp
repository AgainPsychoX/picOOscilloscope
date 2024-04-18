#pragma once
#include "ui/base/Button.hpp"
#include "common.hpp"

namespace ui {

struct ChannelButton : public Button
{
	ChannelButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: Button(x, y, w, h)
	{}

	virtual void render() override;

	virtual void action() override;
	virtual void onPressDown(uint16_t hx, uint16_t hy) override;

private:
	void render(color_t backgroundColor);

	void updateVoltageShifterInput(); // glue
};

}
