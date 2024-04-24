#pragma once
#include "ui/base/RangeHorizontalInput.hpp"

namespace ui {

struct TimeOffsetInput : public RangeHorizontalInput
{
	TimeOffsetInput(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: RangeHorizontalInput(
			/*position & size*/ x, y, w, h, 
			/*labels*/ "Przesuniecie", "-", "+",  // TODO: Polish ę
			/*sides labels font*/1, 
			/*padding*/ 4, /*align*/ true)
	{}

	int32_t value; // us

	virtual const char* valueString() override;
	virtual void onLeftAction() override;
	virtual void onRightAction() override;

protected:
	virtual uint16_t longPressTickInterval() const override;

	/// Optimal minimal amount should make the graph(s) move by single pixel
	uint16_t getOptimalStep() const;
};

}
