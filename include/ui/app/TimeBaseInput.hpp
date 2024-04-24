#pragma once
#include "ui/base/RangeHorizontalInput.hpp"

namespace ui {

struct TimeBaseInput : public RangeHorizontalInput
{
	TimeBaseInput(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: RangeHorizontalInput(
			/*position & size*/ x, y, w, h, 
			/*labels*/ "Podstawa czasu", "-", "+", 
			/*sides labels font*/1, 
			/*padding*/ 4, /*align*/ true)
	{}

	uint32_t value; // us

	virtual const char* valueString() override;
	virtual void onLeftAction() override;
	virtual void onRightAction() override;
};

}
