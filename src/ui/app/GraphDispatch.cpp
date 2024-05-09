#include "ui/app/GraphDispatch.hpp"
#include "ui/app/VoltageGraph.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/TimeOffsetInput.hpp"
#include "sampling/ChannelSelection.hpp"

namespace ui {

extern VoltageGraph singleGraph;
extern VoltageGraph firstSplitGraph;
extern VoltageGraph secondSplitGraph;

extern GraphDispatch graphDispatch;

void GraphDispatch::draw()
{
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

void GraphDispatch::update()
{
	if (isSingleGraphActive()) {
		if (lastWasSingle) [[likely]] {
			singleGraph.update();
		}
		else {
			setupSingleGraph();
			singleGraph.draw();
			lastWasSingle = true;
		}
	}
	else /* both channels selected separate view */ {
		if (!lastWasSingle) [[likely]] {
			firstSplitGraph.update();
			secondSplitGraph.update();
		}
		else {
			setupSplitGraphs();
			firstSplitGraph.draw();
			secondSplitGraph.draw();
			lastWasSingle = false;
		}
	}
}

static const uint16_t yValueStepByRangeId[] = { 1000, 500, 250, 100 };
static const uint16_t heightDivisionsByRangeId[] = { 12, 10, 10, 12 };

bool GraphDispatch::isSingleGraphActive() const
{
	using namespace sampling;
	return channelSelection.single() || channelSelection.together();
}

void GraphDispatch::setup()
{
	if (isSingleGraphActive())
		setupSingleGraph();
	else
		setupSplitGraphs();
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
	singleGraph.updateValueGraphingCache(voltage::shifter[widest].get());
}

void GraphDispatch::setupSplitGraphs()
{
	using namespace sampling;
	auto r0 = voltage::shifter[0].get();
	firstSplitGraph.setHeightDivisions(heightDivisionsByRangeId[r0.id] / 2);
	firstSplitGraph.yValueStep = yValueStepByRangeId[r0.id] * 2;
	firstSplitGraph.updateValueGraphingCache(r0);
	auto r1 = voltage::shifter[1].get();
	secondSplitGraph.setHeightDivisions(heightDivisionsByRangeId[r1.id] / 2);
	secondSplitGraph.yValueStep = yValueStepByRangeId[r1.id] * 2;
	secondSplitGraph.updateValueGraphingCache(r1);
}

uint8_t GraphDispatch::getCellWidth() const
{
	if (isSingleGraphActive())
		return singleGraph.cellWidth;
	else
		return firstSplitGraph.cellWidth;
}

}
