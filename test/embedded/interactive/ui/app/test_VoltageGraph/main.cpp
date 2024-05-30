#include <unity.h> // testing framework
#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>
#include "PersistedConfig.hpp"
#include "sampling.hpp"
#include "ui/root.hpp"
#include "ui/app/GraphDispatch.hpp"
#include "ui/app/ChannelButton.hpp"
#include "ui/app/VoltageShifterInput.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/TimeOffsetInput.hpp"
#include "../../InteractiveTestingButtons.hpp"

extern TFT_eSPI tft; // from main application main

extern void processTouchEventsForUI(ui::RootGroup& root); // from main

namespace ui { // from root.cpp
	extern GraphDispatch graphDispatch;
	extern ChannelButton channelButton;
	extern VoltageShifterInput voltageShifterInput;
	extern TimeBaseInput timeBaseInput;
	extern TimeOffsetInput timeOffsetInput;
}

////////////////////////////////////////////////////////////////////////////////

void suiteSetUp()
{
	tft.init();
	tft.setRotation(1);
	tft.setTextFont(2);

	EEPROM.begin(256);
	PersistedConfig& config = *reinterpret_cast<PersistedConfig*>(EEPROM.getDataPtr());
	touch::calibrationData = &config.calibrationData;
}

int suiteTearDown(int num_failures)
{
	tft.fillScreen(TFT_BLACK);
	auto fh = tft.fontHeight();
	auto xc = tft.width() / 2;
	auto yc = tft.height() / 2;
	tft.setTextFont(2);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextDatum(TC_DATUM);
	tft.drawString("Testing finished", xc, yc - fh);
	char secondLine[32] = "All tests succeed";
	if (num_failures != 0) {
		sprintf(secondLine, "Failed %d tests", num_failures);
	}
	tft.drawString(secondLine, xc, yc);
	return num_failures;
}

namespace ui {

TestPassButton testPassButton;
TestFailButton testFailButton;

}

void setUp(void)
{
	tft.fillScreen(TFT_BLACK);
	tft.setTextFont(2);

	sampling::channelSelection = sampling::ChannelSelection::OnlyFirst;
	sampling::resolution = sampling::Resolution::U8;
	sampling::setClockDivisor(96); // will result in fastest sampling rate

	// One sample per pixel, assuming default fastest sampling rate
	ui::timeBaseInput.value = ui::graphDispatch.getCellWidth() * 2;

	ui::root = {
		&ui::graphDispatch,
		&ui::channelButton,
		&ui::voltageShifterInput,
		&ui::timeBaseInput,
		&ui::timeOffsetInput,
		&ui::testPassButton,
		&ui::testFailButton,
	};
}

void tearDown(void)
{
	tft.fillScreen(TFT_BLACK);

	// Wait for touch screen press release
	tft.setTextFont(2);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextDatum(CC_DATUM);
	tft.drawString("Remove press from touch screen before next test", 
		tft.width() / 2, tft.height() / 2);
	while (touch::anywhere()) delay(1);
}

////////////////////////////////////////////////////////////////////////////////

/// Fills the sampling buffer with sine wave.
/// T represents the resolution, and it must be uint8_t (U8) or uint16_t (U12).
template <typename T>
void fillSamplingBufferWithSine(size_t samplesPerCycle)
{
	using namespace sampling;

	auto p = getSamplesIterator<T>(); // with mutable access
	auto raw = p.pointer; // raw pointer to the beginning

	// Generate values up to first cycle
	const float step = 2 * M_PI / samplesPerCycle;
	for (size_t i = 0; i < samplesPerCycle && i < getNumberOfSamples<uint8_t>(); i++) {
		// Generate values from 0 to 256 (U8) or 0 to 4096 (U12).
		// The +1 is for more even distribution.
		*p++ = static_cast<T>((sinf(i * step) + 1) * (sizeof(T) == 1 ? 128 : 2048));
	}

	// Copy generated values for the rest of the buffer
	size_t j = 0;
	for (size_t i = samplesPerCycle; i < getNumberOfSamples<uint8_t>(); i++) {
		*p++ = raw[j++];
		if (j == samplesPerCycle) j = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

void test_sine()
{
	fillSamplingBufferWithSine<uint8_t>(1000);

	// Setup
	ui::root.draw();

	// Loop (breaks on TEST_FAIL or TEST_PASS using buttons)
	while (true) {
		processTouchEventsForUI(ui::root);
		ui::root.update();
		yield(); // not necessary, but just in case
	}
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	suiteSetUp();
	UNITY_BEGIN();
	RUN_TEST(test_sine);
	suiteTearDown(UNITY_END());
}

void loop() {}
