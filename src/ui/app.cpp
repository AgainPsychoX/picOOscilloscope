#include "ui/base.hpp"
#include <TFT_eSPI.h>
#include "sampling.hpp"
#include "colors.hpp"

using namespace colors;

extern TFT_eSPI tft; // from main

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Forward declarations to allow interleaving logic

struct ChannelButton;
struct VoltageShifterButton;

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

		updateVoltageShifterButton();
		// FIXME: ^ incomplete type... split into hpp/cpp's inside app folder? 
	}

	void updateVoltageShifterButton();

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
		if (!sampling::channelSelection.first())
			tft.setTextColor(TFT_DARKGREY, backgroundColor);
		tft.drawString("CH1", x + w / 4, y + h - 1);

		auto c = sampling::channelSelection.together() ? TFT_WHITE :  TFT_DARKGREY;
		tft.setTextColor(c, backgroundColor);
		tft.drawString("+", x + w / 2, y + h - 1);

		c = sampling::channelSelection.second() ? TFT_WHITE :  TFT_DARKGREY;
		tft.setTextColor(c, backgroundColor);
		tft.drawString("CH2", x + w - w / 4, y + h - 1);
	}
} channelButton;

////////////////////////////////////////////////////////////////////////////////

struct VoltageShifterButton : public RangeHorizontalButton
{
	VoltageShifterButton()
		: RangeHorizontalButton(
			/*position & size*/ 380, 40,  100, 40, 
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
				return;
			case ChannelSelection::OnlySecond:
				voltage::shifter[1].set(voltage::shifter[1].get() + direction);
				return;
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
				return;
		}
	}
} voltageShifterButton;

////////////////////////////////////////////////////////////////////////////////
// Extra glue

void ChannelButton::updateVoltageShifterButton()
{
	voltageShifterButton.render();
}

////////////////////////////////////////////////////////////////////////////////

// TODO: hit box in the middle

RootGroup root {
	&channelButton,
	&voltageShifterButton,
	new TwoLineDummyButton(380, 80,  100, 40, "Podstawa czasu", " -   2 us   + "),
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

struct Graph : public Element
{
	static constexpr uint16_t x = 0;
	static constexpr uint16_t y = 0;
	static constexpr uint16_t width = 380;
	static constexpr uint     widthDivisions = 10;
	static constexpr uint16_t widthPixelsPerDivision = width / widthDivisions;
	static constexpr uint16_t widthPixelWasted = width % widthDivisions;
	static constexpr uint16_t height = 320;
	static constexpr uint     heightDivisions = 10;
	static constexpr uint16_t heightPixelsPerDivision = height / heightDivisions;
	static constexpr uint16_t heightPixelWasted = height % heightDivisions;
	static constexpr color_t primaryGridColor = to565(RGB { 88, 88, 88 });
	static constexpr color_t secondaryGridColor = to565(RGB { 44, 44, 44 });

	virtual void render() override
	{
		tft.fillRect(x, y, width, height, TFT_BLACK);
		renderLines();
	}

	void renderLines()
	{
		uint16_t lineX = x;
		for (uint i = 0; i < widthDivisions; i++) {
			bool isPrimary = i == widthDivisions / 2;
			tft.drawFastVLine(lineX, y, height, 
				isPrimary ? primaryGridColor : secondaryGridColor);
			lineX += widthPixelsPerDivision;
		}
		uint16_t lineY = y;
		for (uint i = 0; i < heightDivisions; i++) {
			bool isPrimary = i == heightDivisions / 2;
			tft.drawFastHLine(x, lineY, width,
				isPrimary ? primaryGridColor : secondaryGridColor);
			lineY += heightPixelsPerDivision;
		}
	}
};

// TODO: better way to share it to main, maybe go fix TODO in Group constructor
Graph graph_;
Element& graph = graph_;

}

