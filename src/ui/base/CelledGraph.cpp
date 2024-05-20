#include "ui/base/CelledGraph.hpp"
#include "ui/tft.hpp"
#include "logging.hpp"

namespace ui {

void Graph::setWidthDivisions(uint8_t number)
{
	assert(number <= maxDivisionsPerDim);
	widthDivisions = number;
	cellWidth = width / widthDivisions;
	labelCenterX = x + cellWidth / 2;
	fullyDirty = true;
}

void Graph::setHeightDivisions(uint8_t number)
{
	assert(number <= maxDivisionsPerDim);
	heightDivisions = number;
	cellHeight = height / heightDivisions;
	halfHeightWasted = height % heightDivisions / 2;
	fullyDirty = true;
}

void Graph::draw()
{
	// Clear and draw extra line parts in wasted space/padding
	if (halfHeightWasted) {
		uint16_t yBottom = y + height - halfHeightWasted;
		tft.fillRect(x, y, width, halfHeightWasted, TFT_BLACK);
		tft.fillRect(x, yBottom, width, halfHeightWasted, TFT_BLACK);

		uint_fast16_t sx = x;
		for (uint_fast8_t cx = 0; cx < widthDivisions; cx++) {
			bool center = cx == widthDivisions / 2;
			tft.drawFastVLine(sx, y, halfHeightWasted,
				center ? centerLineColor : helperLineColor);
			tft.drawFastVLine(sx, yBottom, halfHeightWasted,
				center ? centerLineColor : helperLineColor);
			sx += cellWidth;
		}
	}

	auto us_start = to_us_since_boot(get_absolute_time());

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(helperLineVoltageLabelColor, TFT_BLACK);

	uint_fast16_t sy = y + halfHeightWasted;
	for (uint_fast8_t cy = 0; cy < heightDivisions; cy++) {
		uint_fast16_t sx = x;
		for (uint_fast8_t cx = 0; cx < widthDivisions; cx++) {
			drawCell(cx, cy, sx, sy);
			sx += cellWidth;
		}
		sy += cellHeight;
	}

	cellDirtyBitset.reset(/*all*/);
	fullyDirty = false;

	drawSeries();

	auto us_end = to_us_since_boot(get_absolute_time());
	LOG_TRACE("Graphing", "Draw took %lluus", us_end - us_start);
}

void Graph::update()
{
	if (fullyDirty)
		return draw();

	// TODO: make those logging variables present only in debug/trace
	auto us_start = to_us_since_boot(get_absolute_time());
	unsigned int cellsCount = 0;

	tft.setTextDatum(CC_DATUM);
	tft.setTextColor(helperLineVoltageLabelColor, TFT_BLACK);

	uint_fast16_t sy = y + halfHeightWasted;
	for (uint_fast8_t cy = 0; cy < heightDivisions; cy++) {
		uint_fast16_t sx = x;
		for (uint_fast8_t cx = 0; cx < widthDivisions; cx++) {
			if (isCellDirty(cx, cy)) {
				drawCell(cx, cy, sx, sy);
				markCellFresh(cx, cy);
				// cellsCount++;
			}
			sx += cellWidth;
		}
		sy += cellHeight;
	}

	drawSeries();

	auto us_end = to_us_since_boot(get_absolute_time());
	LOG_TRACE("Graphing", "Update with %u cells took %lluus", 
		cellsCount, us_end - us_start);
}

void Graph::drawCell(uint8_t cx, uint8_t cy, uint16_t sx, uint16_t sy)
{
	// TODO: consider __attribute__((always_inline)) 
	// TODO: use lower-level API: startWrite, pushBlock, endWrite
	tft.drawFastHLine(sx, sy, cellWidth, 
		cy == heightDivisions / 2 ? centerLineColor : helperLineColor);
	tft.drawFastVLine(sx, sy, cellHeight,
		cx == widthDivisions / 2 ? centerLineColor : helperLineColor);
	tft.fillRect(sx + 1, sy + 1, cellWidth - 1, cellHeight - 1, TFT_BLACK);
	if (cx == 0) [[unlikely]] {
		const char* str = getYLabel(cy);
		if (str) [[likely]] {
			tft.drawString(str, labelCenterX, sy, 1);
		}
	}
	if (cy == heightDivisions - 1) {
		uint16_t yLast = sy + cellHeight;
		if (!halfHeightWasted) yLast -= 1;
		tft.drawFastHLine(sx, yLast, cellWidth, helperLineColor);
		if (cx == 0) {
			const char* str = getYLabel(heightDivisions);
			if (str) [[likely]] {
				tft.drawString(str, labelCenterX, yLast, 1);
			}
		}
	}
}

const char* Graph::getYLabel(uint8_t cy) const
{
	(void) cy; // unused in base
	return nullptr;
}

}
