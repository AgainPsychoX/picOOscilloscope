#include "ui/base.hpp"
#include <bitset>
#include <TFT_eSPI.h>
#include "sampling.hpp"
#include "colors.hpp"

using namespace colors;

extern TFT_eSPI tft; // from main

namespace ui {

constexpr color_t channel1Color = to565(RGB { 255, 255, 0 } /* yellow */);
constexpr color_t channel2Color = to565(RGB { 180, 255, 0 } /* green-yellow */);
constexpr color_t centerLineColor = to565(RGB { 127, 127, 127 });
constexpr color_t helperLineColor = to565(RGB { 63, 63, 63 });
constexpr color_t helperLineVoltageLabelColor = to565(RGB { 95, 95, 95 });

#define GRAPHS_VOLTAGE_LABELS_EVEN_WITHOUT_V 1

////////////////////////////////////////////////////////////////////////////////
// Forward declarations to allow interleaving logic

struct ChannelButton;
struct VoltageShifterInput;

void updateGraphsByVoltageShiftersInput();

////////////////////////////////////////////////////////////////////////////////

struct DummyButton : public TextButton 
{
	virtual void action() override {}

	DummyButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* text)
		: TextButton(x, y, w, h, text)
	{}
};

struct TwoLineDummyButton : public Button
{
	const char* firstLine;
	const char* secondLine;

	TwoLineDummyButton(
		uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		const char* firstLine, const char* secondLine)
		: Button(x, y, w, h), firstLine(firstLine), secondLine(secondLine)
	{}

	virtual void action() override {}

	virtual void onPressDown(uint16_t hx, uint16_t hy) override
	{
		(void) hx; (void) hy; // unused
		render(TFT_DARKGREY);
	}

	virtual void render() override
	{
		render(TFT_BLACK);
	}

private:
	void render(uint16_t backgroundColor)
	{
		tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
		tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

		tft.setTextColor(TFT_WHITE, backgroundColor);
		auto fh = tft.fontHeight();
		auto xc = x + w / 2;
		auto yc = y + h / 2;
		if (secondLine) {
			tft.setTextDatum(TC_DATUM);
			tft.drawString(firstLine, xc, yc - fh);
			tft.drawString(secondLine, xc, yc);
		}
		else {
			tft.setTextDatum(CC_DATUM);
			tft.drawString(firstLine, xc, yc);
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

struct ChannelButton : public Button
{
	ChannelButton()
		: Button(380, 0, 100, 40)
	{}

	virtual void action() override
	{
		using namespace sampling;
		switch (channelSelection) {
			default:
			case ChannelSelection::None:
				channelSelection = ChannelSelection::OnlyFirst;
				break;
			case ChannelSelection::OnlyFirst:
				channelSelection = ChannelSelection::OnlySecond;
				break;
			case ChannelSelection::OnlySecond:
				channelSelection = ChannelSelection::BothSeparate;
				break;
			case ChannelSelection::BothSeparate:
				channelSelection = ChannelSelection::BothTogether;
				break;
			case ChannelSelection::BothTogether:
				channelSelection = ChannelSelection::OnlyFirst;
				break;
		}

		updateVoltageShifterInput();
	}

	void updateVoltageShifterInput();

	virtual void onPressDown(uint16_t hx, uint16_t hy) override
	{
		(void) hx; (void) hy; // unused
		render(TFT_DARKGREY);
	}
	virtual void render() override
	{
		render(TFT_BLACK);
	}

private:
	void render(uint16_t backgroundColor)
	{
		tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
		tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

		tft.setTextDatum(TC_DATUM);
		tft.setTextColor(TFT_WHITE, backgroundColor);
		tft.drawString("Wybor kanalu", x + w / 2, y + 2);
		// TODO: Polish ó and ł, maybe by overlapping: o' and -l, or custom font

		tft.setTextDatum(BC_DATUM);
		color_t c;

		c = sampling::channelSelection.first() ? channel1Color :  TFT_DARKGREY;
		tft.setTextColor(c, backgroundColor);
		tft.drawString("CH1", x + w / 4, y + h - 1);

		c = sampling::channelSelection.together() ? TFT_WHITE :  TFT_DARKGREY;
		tft.setTextColor(c, backgroundColor);
		tft.drawString("+", x + w / 2, y + h - 1);

		c = sampling::channelSelection.second() ? channel2Color :  TFT_DARKGREY;
		tft.setTextColor(c, backgroundColor);
		tft.drawString("CH2", x + w - w / 4, y + h - 1);
	}
} channelButton;

////////////////////////////////////////////////////////////////////////////////

struct VoltageShifterInput : public RangeHorizontalInput
{
	VoltageShifterInput()
		: RangeHorizontalInput(
			/*position & size*/ 380, 40, 100, 40, 
			/*labels*/ "Napiecie", "<", ">", // TODO: Polish ę
			/*sides labels font*/1, 
			/*padding*/ 4, /*align*/ true)
	{}

	char valueStringBuffer[16];

	virtual const char* valueString() override
	{
		using namespace sampling;

		voltage::Range range;
		switch (channelSelection) {
			default:
			case ChannelSelection::None:
				return "wybierz kanal"; // TODO: Polish ł
			case ChannelSelection::OnlyFirst:
				range = voltage::shifter[0].get();
				break;
			case ChannelSelection::OnlySecond:
				range = voltage::shifter[1].get();
				break;
			case ChannelSelection::BothSeparate:
			case ChannelSelection::BothTogether:
				range = voltage::shifter[0].get();
				auto otherRange = voltage::shifter[1].get();
				if (range == otherRange) {
					// Continue with displaying voltage
					break;
				}
				else {
					return "rozne"; // TODO: Polish ó
				}
		}

		snprintf(valueStringBuffer, sizeof(valueStringBuffer),
			"%.2f +%.2fV", range.minVoltage(), range.maxVoltage());

		return valueStringBuffer;
	}

	virtual void onLeftAction() override
	{
		move(-1);
	}

	virtual void onRightAction() override
	{
		move(1);
	}

protected:
	void move(int8_t direction)
	{
		using namespace sampling;
		switch (channelSelection) {
			default:
			case ChannelSelection::None:
				return; // do nothing
			case ChannelSelection::OnlyFirst:
				voltage::shifter[0].set(voltage::shifter[0].get() + direction);
				break;
			case ChannelSelection::OnlySecond:
				voltage::shifter[1].set(voltage::shifter[1].get() + direction);
				break;
			case ChannelSelection::BothSeparate:
			case ChannelSelection::BothTogether:
				auto a = voltage::shifter[0].get();
				auto b = voltage::shifter[1].get();
				if (a == b) {
					// Continue, changing both channels from current value
					auto previous = a + direction;
					voltage::shifter[0].set(previous);
					voltage::shifter[1].set(previous);
				}
				else {
					// Continue, changing both channels from safe/widest range
					auto safe = voltage::Range();
					voltage::shifter[0].set(safe);
					voltage::shifter[1].set(safe);
				}
				break;
		}
		updateGraphsByVoltageShiftersInput();
	}
} voltageShifterInput;

////////////////////////////////////////////////////////////////////////////////

uint16_t timeBaseIntegers[] {
	1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 50, 64, 80, 100, 128, 160, 200, 256, 320, 400, 500, 512, 800
	//	1	2	4		8		16		32			64			128			256				512
	//				5							50		 								500
	//						10		20		40			80			160			320
	//														100			200			400		800
};

struct TimeBaseInput : public RangeHorizontalInput
{
	TimeBaseInput()
		: RangeHorizontalInput(
			/*position & size*/ 380, 80, 100, 40, 
			/*labels*/ "Podstawa czasu", "-", "+", 
			/*sides labels font*/1, 
			/*padding*/ 4, /*align*/ true)
	{}

	char buffer[16];

	virtual const char* valueString() override
	{
		using namespace sampling;

		snprintf(buffer, sizeof(buffer), "%u %s", 
			2, 
			"us");

		return buffer;
	}

	virtual void onLeftAction() override
	{
		// TODO: ...
	}

	virtual void onRightAction() override
	{
		// TODO: ...
	}
} timeBaseInput;

////////////////////////////////////////////////////////////////////////////////
// Extra glue

void ChannelButton::updateVoltageShifterInput()
{
	voltageShifterInput.render();
}

////////////////////////////////////////////////////////////////////////////////

RootGroup root {
	&channelButton,
	&voltageShifterInput,
	&timeBaseInput,
	// &timeOffsetInput,
	// new TwoLineDummyButton(380, 80,  100, 40, "Podstawa czasu", " -   2 us   + "),
	new TwoLineDummyButton(380, 120, 100, 40,  "Przesuniecie" , " -   30 ms  + "),
	new TwoLineDummyButton(380, 160, 100, 40, "Tryb", " <  Ciagly  > "),
	new TwoLineDummyButton(380, 200, 100, 40, "Trigger type", "_/ \\_ _||_ |_|"),
	new TwoLineDummyButton(380, 240, 100, 40, "Trigger V", " -  2.00 V  + "),
	new TwoLineDummyButton(380, 280, 100, 40, "Trigger dT", " -   2 us   + "),
	/**
	 *  TODO: rethink it once more (or when implementing?)
	 * 
	 *  1) Wybór kanał(ów)
	 * 	2) Wybór napięcie
	 *  3) Podstawa czasu
	 *  4) Przesuniecie
	 *  5) Tryb wyzwalania
	 *  6) Napięcie wyzwalania
	 *  7) Szerokość wyzwalania
	 * 
	 * + dłuższe
	 * 
	 *  8) Inne (sub-menu)
	 *      1) Czas między próbkami (1ms .. 2us)
	 *      2) Częstotliwość próbkowania (732.4Hz, 1kHz .. 500kHz)
	 *      3) Baza zegara ADC (USB PLL 48MHz or SYS PLL 125MHz or even overclock with confirmation prompt?)
	 *      4) Podział zegara ADC (96 .. 65536)
	 *      5) Rozdzielczość ADC (U8/U12)
	 *      6) Częstotliwość testowa (x..y? GPIO 21)
	 *      7) 
	 *      8) < Wróć 
	*/
};

////////////////////////////////////////////////////////////////////////////////

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

	void setWidthDivisions(uint8_t number)
	{
		widthDivisions = number;
		cellWidth = width / widthDivisions;
		labelCenterX = x + cellWidth / 2;
		fullyDirty = true;
	}

	void setHeightDivisions(uint8_t number)
	{
		heightDivisions = number;
		cellHeight = height / heightDivisions;
		halfHeightWasted = height % heightDivisions / 2;
		fullyDirty = true;
	}

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

	virtual void render() override
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
		// Serial.printf("Graph render took %llu\n", us_end - us_start);
	}

	virtual void partialRender() override
	{
		if (fullyDirty)
			return render();

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
		// Serial.printf("Graph partialRender with %u cells took %llu\n", 
		// 	cellsCount, us_end - us_start);
	}

	inline void drawCell(uint8_t cx, uint8_t cy, uint16_t sx, uint16_t sy)
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

	virtual const char* getYLabel(uint8_t cy)
	{
		(void) cy; // unused in base
		return nullptr;
	}

	void drawGraph()
	{
		/*
			TODO:
			1. get sampling window start position (a bit before the trigger)
			2. calculate graphing window start position to match center to trigger point;
				factor in the offset (in time, set by user)
			3. graph next points, scaling from U8/U12 to height on graph

			size_t numberOfSamples = sampling::numberOfSamples();
			
			// Graphing window offset in samples
			auto graphingWindowOffsetInSamples = timeToSamplesNumber(widthDivisions / 2 * timeBase);

			// Need to limit graphing start position OR skip drawing for a bit
			#ifdef GRAPHING_LIMIT_TIME_OFFSET
			if (graphingWindowOffsetInSamples > sampling::numberOfPreTriggerSamples) {
				graphingWindowOffsetInSamples = sampling::numberOfPreTriggerSamples;
			}
			uint16_t x = 0;;
			#else
			uint16_t x = ?;
			// TODO: ... 
			#endif
			switch (sampling::resolution) {
				default:
				case Resolution::U8: {
					const uint8_t* p = sampling::windowStart;
					p -= graphingWindowOffsetInSamples;
					for (size_t i = 0; i < numberOfSamples; i++) {
						// TODO: break if end of graphing width

						// drawPoint()

						p++; // TODO: warp around
					}
					break;
				}
				case Resolution::U12: {
					// TODO: ...
					break;
				}
			}
		*/
	}
};

struct VoltageGraph : public Graph
{
	VoltageGraph(
		uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		uint8_t widthDivisions, uint8_t heightDivisions
	) 
		: Graph(x, y, width, height, widthDivisions, heightDivisions)
	{}

	uint16_t yValueStep; // in mV

	virtual const char* getYLabel(uint8_t cy)
	{
		auto foo = heightDivisions / 2 - cy;
		if (foo == 0) {
			return nullptr;
		}
		int16_t mV = yValueStep * foo;
		static char buffer[8];
		int w = sprintf(buffer, "%+.4d", mV);
		buffer[w - 1] = buffer[w - 2];
		buffer[w - 2] = buffer[w - 3];
		buffer[w - 3] = '.';
		buffer[5] = 0;
		// TODO: clear debug logging as trace log
		// Serial.printf("cy=%u mV=%d buffer='%s'\n", cy, mV, buffer);
		return buffer;
	}
};

VoltageGraph singleGraph { 0, 0, 380, 320, 10, 12 };
VoltageGraph firstSplitGraph { 0, 0, 380, 160, 10, 12 };
VoltageGraph secondSplitGraph { 0, 160, 380, 160, 10, 12 };

// TODO: union? single graph OR 2x split graphs

static const uint16_t yValueStepByRangeId[] = { 1000, 500, 250, 100 };
static const uint16_t heightDivisionsByRangeId[] = { 12, 10, 10, 12 };

inline bool isSingleGraphActive()
{
	using namespace sampling;
	return channelSelection.single() || channelSelection.together();
}

/// Setups single graph for properly displaying current voltage range etc.
void setupSingleGraph()
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

/// Setups split graphs for properly displaying current voltage range etc.
void setupSplitGraphs()
{
	using namespace sampling;
	uint8_t id = voltage::shifter[0].get().id;
	firstSplitGraph.setHeightDivisions(heightDivisionsByRangeId[id] / 2);
	firstSplitGraph.yValueStep = yValueStepByRangeId[id] * 2;
	/*****/ id = voltage::shifter[1].get().id;
	secondSplitGraph.setHeightDivisions(heightDivisionsByRangeId[id] / 2);
	secondSplitGraph.yValueStep = yValueStepByRangeId[id] * 2;
}

/// Glue function to update graphs after voltage shifter changes.
void updateGraphsByVoltageShiftersInput()
{
	if (isSingleGraphActive()) {
		setupSingleGraph();
	}
	else {
		setupSplitGraphs();
	}
}

////////////////////////////////////////////////////////////////////////////////

struct GraphDispatch : public Element
{
	bool lastWasSingle;

	virtual void render() override
	{
		using namespace sampling;
		if (isSingleGraphActive()) {
			setupSingleGraph();
			singleGraph.render();
			lastWasSingle = true;
		}
		else /* both & separate */ {
			setupSplitGraphs();
			firstSplitGraph.render();
			secondSplitGraph.render();
			lastWasSingle = false;
		}
	}

	virtual void partialRender() override
	{
		using namespace sampling;
		if (isSingleGraphActive()) {
			if (lastWasSingle) [[likely]] {
				singleGraph.partialRender();
			}
			else {
				setupSingleGraph();
				singleGraph.render();
				lastWasSingle = true;
			}
		}
		else /* both channels selected separate view */ {
			if (!lastWasSingle) [[likely]] {
				firstSplitGraph.partialRender();
				secondSplitGraph.partialRender();
			}
			else {
				setupSplitGraphs();
				firstSplitGraph.render();
				secondSplitGraph.render();
				lastWasSingle = false;
			}
		}
	}
} graphDispatch;

// TODO: better way to share it to main, maybe go fix TODO in Group constructor
Element& graph = graphDispatch;

}
