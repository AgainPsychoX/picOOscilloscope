#include "ui/app/Graphs.hpp"
#include "ui/tft.hpp"
#include "sampling.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////

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

	// auto us_start = to_us_since_boot(get_absolute_time());

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

	// auto us_end = to_us_since_boot(get_absolute_time());
	// Serial.printf("Graph draw took %llu\n", us_end - us_start);
}

void Graph::partialDraw()
{
	if (fullyDirty)
		return draw();

	// auto us_start = to_us_since_boot(get_absolute_time());
	// unsigned int cellsCount = 0;

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

	// TODO: clear debug logging as trace log
	// auto us_end = to_us_since_boot(get_absolute_time());
	// Serial.printf("Graph partialDraw with %u cells took %llu\n", 
	// 	cellsCount, us_end - us_start);
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

void Graph::drawSeries()
{
	// TODO: ...
}

////////////////////////////////////////////////////////////////////////////////

const char* VoltageGraph::getYLabel(uint8_t cy) const
{
	auto foo = heightDivisions / 2 - cy;
	if (foo == 0) {
		return nullptr;
	}
	int16_t mV = yValueStep * foo;
	// TODO: consider avoiding printf here? https://godbolt.org/z/v6fM3Mxor 
	int w = sprintf(sharedBuffer, "%+.4d", mV);
	sharedBuffer[w - 1] = sharedBuffer[w - 2];
	sharedBuffer[w - 2] = sharedBuffer[w - 3];
	sharedBuffer[w - 3] = '.';
	sharedBuffer[5] = 0;
	// TODO: clear debug logging as trace log
	// Serial.printf("cy=%u mV=%d buf='%s'\n", cy, mV, sharedBuffer);
	return sharedBuffer;
}

////////////////////////////////////////////////////////////////////////////////
// Setup & glue

static const uint16_t yValueStepByRangeId[] = { 1000, 500, 250, 100 };
static const uint16_t heightDivisionsByRangeId[] = { 12, 10, 10, 12 };

bool GraphDispatch::isSingleGraphActive()
{
	using namespace sampling;
	return channelSelection.single() || channelSelection.together();
}

void GraphDispatch::setupSingleGraph()
{
	using namespace sampling;
	uint8_t widest = 3; // starts with 3 which is narrowest range
	if (channelSelection.first()) {
		widest = voltage::shifter[0].get().id;
	}
	if (channelSelection.second()) {
		widest = std::min(widest, voltage::shifter[1].get().id);
	}
	singleGraph.setHeightDivisions(heightDivisionsByRangeId[widest]);
	singleGraph.yValueStep = yValueStepByRangeId[widest];
}

void GraphDispatch::setupSplitGraphs()
{
	using namespace sampling;
	uint8_t id = voltage::shifter[0].get().id;
	firstSplitGraph.setHeightDivisions(heightDivisionsByRangeId[id] / 2);
	firstSplitGraph.yValueStep = yValueStepByRangeId[id] * 2;
	/*****/ id = voltage::shifter[1].get().id;
	secondSplitGraph.setHeightDivisions(heightDivisionsByRangeId[id] / 2);
	secondSplitGraph.yValueStep = yValueStepByRangeId[id] * 2;
}

void GraphDispatch::draw()
{
	using namespace sampling;
	if (isSingleGraphActive()) {
		setupSingleGraph();
		singleGraph.draw();
		lastWasSingle = true;
	}
	else /* both & separate */ {
		setupSplitGraphs();
		firstSplitGraph.draw();
		secondSplitGraph.draw();
		lastWasSingle = false;
	}
}

void GraphDispatch::partialDraw()
{
	using namespace sampling;
	if (isSingleGraphActive()) {
		if (lastWasSingle) [[likely]] {
			singleGraph.partialDraw();
		}
		else {
			setupSingleGraph();
			singleGraph.draw();
			lastWasSingle = true;
		}
	}
	else /* both channels selected separate view */ {
		if (!lastWasSingle) [[likely]] {
			firstSplitGraph.partialDraw();
			secondSplitGraph.partialDraw();
		}
		else {
			setupSplitGraphs();
			firstSplitGraph.draw();
			secondSplitGraph.draw();
			lastWasSingle = false;
		}
	}
}

GraphDispatch graphDispatch;

// TODO: better way to share it to main, maybe go fix TODO in Group constructor
Element& graph = graphDispatch;


}
