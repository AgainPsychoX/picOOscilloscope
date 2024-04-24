#pragma once
#include "ui/base/RangeHorizontalInput.hpp"
#include "ui/common.hpp"

namespace ui {

struct VoltageShifterInput : public RangeHorizontalInput
{
	VoltageShifterInput(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: RangeHorizontalInput(
			/*position & size*/ x, y, w, h, 
			/*labels*/ "Napiecie", "<", ">", // TODO: Polish ę
			/*sides labels font*/1, 
			/*padding*/ 4, /*align*/ true)
	{
		enableLongPressSpeedUp = false;
	}

	virtual const char* valueString() override;
	virtual void onLeftAction() override;
	virtual void onRightAction() override;

protected:
	void move(int8_t direction);

private:
	void updateGraphs(); // glue
};

}
