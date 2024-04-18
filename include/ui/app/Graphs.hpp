#pragma once
#include "ui/base/Element.hpp"
#include "ui/common.hpp"
#include <bitset>

namespace ui {

/*
	Problem: to draw new graph we need clear previous one.

	Solutions:
	1. full fill & render
		- will cause flickering to re-render all the lines
		- can make touch a bit less responsible
	2. remembering which pixels were changed
		- requires memory to keep each pixel position - we can assume one per 
			column per channel
		- requires extra logic to handle grid lines
		- requires extra logic to handle vertical lines 
			which happen if value changes fast (quite often); 
			also makes it harder to use anti-aliased lines
	3. splitting the area to cells and remembering which cells are dirty
		- cell consist of vertical and horizontal line, and the area they mark out
		- can make use of bitset to keep track cells dirty state
		- cells need to be divided gracefully

	I decided to go with using cells approach.
*/

////////////////////////////////////////////////////////////////////////////////

struct Graph : public Element
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

	static constexpr auto maxDivisionsPerDim = 12;
	uint8_t widthDivisions;
	uint8_t heightDivisions;
	uint8_t cellWidth;
	uint8_t cellHeight;
	uint8_t halfHeightWasted;
	uint16_t labelCenterX;

	// TODO: support asymmetric value ranges
	// TODO: support width waste 

	void setWidthDivisions(uint8_t number);
	void setHeightDivisions(uint8_t number);

	Graph(
		uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		uint8_t widthDivisions, uint8_t heightDivisions
	) :
		x(x), y(y), width(width), height(height),
		widthDivisions(widthDivisions), heightDivisions(heightDivisions), 
		cellWidth(width / widthDivisions), cellHeight(height / heightDivisions),
		halfHeightWasted(height % heightDivisions / 2),
		labelCenterX(x + cellWidth / 2)
	{
		assert(widthDivisions <= maxDivisionsPerDim);
		assert(heightDivisions <= maxDivisionsPerDim);
	}

	std::bitset<maxDivisionsPerDim * maxDivisionsPerDim> cellDirtyBitset;
	bool fullyDirty = true;

	inline bool isCellDirty(uint8_t cx, uint8_t cy)
	{
		return cellDirtyBitset[cx * maxDivisionsPerDim + cy];
	}

	inline void markCellDirty(uint8_t cx, uint8_t cy)
	{
		cellDirtyBitset[cx * maxDivisionsPerDim + cy] = true;
	}

	inline void markCellFresh(uint8_t cx, uint8_t cy)
	{
		cellDirtyBitset[cx * maxDivisionsPerDim + cy] = false;
	}

	virtual void render() override;
	virtual void partialRender() override;

	virtual void drawCell(uint8_t cx, uint8_t cy, uint16_t sx, uint16_t sy);

	virtual const char* getYLabel(uint8_t cy) const;

	virtual void drawSeries();
};

////////////////////////////////////////////////////////////////////////////////

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
};

////////////////////////////////////////////////////////////////////////////////
// Setup & glue

struct GraphDispatch : public Element
{
	bool lastWasSingle;

	virtual void render() override;
	virtual void partialRender() override;

	bool isSingleGraphActive();

	/// Setups single graph for properly displaying current voltage range etc.
	void setupSingleGraph();

	/// Setups split graphs for properly displaying current voltage range etc.
	void setupSplitGraphs();
};

extern VoltageGraph singleGraph;
extern VoltageGraph firstSplitGraph;
extern VoltageGraph secondSplitGraph;

extern GraphDispatch graphDispatch;

// TODO: better way to share it to main, maybe go fix TODO in Group constructor
extern Element& graph;

}
