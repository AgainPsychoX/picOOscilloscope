#pragma once
#include <ui/base/Element.hpp>
#include <ui/common.hpp>

namespace ui {

struct GraphDispatch : public Element
{
	bool lastWasSingle;

	virtual void draw() override;
	virtual void update() override;

	/// Returns which graph should be currently active (by channel selection)
	bool isSingleGraphActive() const;

	/// Setups graph(s) for properly displaying current voltage range etc.
	void setup();
	void setupSingleGraph();
	void setupSplitGraphs();

	/// Returns width of cells that graph(s) are setup for.
	uint8_t getCellWidth() const;
};

}
