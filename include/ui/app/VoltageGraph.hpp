#pragma once
#include "ui/base/CelledGraph.hpp"
#include "sampling/voltage.hpp"

namespace ui {

struct VoltageGraph : public Graph
{
	VoltageGraph(
		uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		uint8_t widthDivisions, uint8_t heightDivisions
	) 
		: Graph(x, y, width, height, widthDivisions, heightDivisions)
	{}

	uint16_t yValueStep; // in mV

	virtual const char* getYLabel(uint8_t cy) const;

	virtual void drawSeries() override;

	void updateValueGraphingCache(sampling::voltage::Range range);

protected:
	/// Multiplier to be applied to raw value.
	uint32_t valueGraphingMultiplier;
	/// Shift (before dividing, added to the multiplied raw value) to cover for 
	/// the difference between voltage at the bottom Y and `range.minVoltage()`.
	uint32_t valueGraphingShift;
	/// Divider to bring the intermediate value to number of pixels, 
	/// to be subtracted from bottom Y.
	uint32_t valueGraphingDividerForHeight;
	// Most bottom of the graph, screen Y, which height will be subtracted from
	// to get final screen Y.
	uint16_t valueGraphingBottomScreenY;
};

}
