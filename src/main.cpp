#include <limits>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include "logging.hpp"
#include "PersistedConfig.hpp"
#include "sampling.hpp" // init, start
#include "sampling/voltage.hpp" // init
#include "ui/root.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/GraphDispatch.hpp"

TFT_eSPI tft = TFT_eSPI();

namespace ui {
	extern TimeBaseInput timeBaseInput;
	extern GraphDispatch graphDispatch;
}

void processTouchEventsForUI(ui::RootGroup& root)
{
	using namespace ui;

	now = to_ms_since_boot(get_absolute_time());

	bool detected = touch::getFiltered(pressX, pressY);
	bool wasReleased = now - pressLastTime > 100;
	if (wasReleased) {
		if (detected) {
			pressStartTime = now;
			root.onPressDown(pressX, pressY);
		}
		else if (pressLastTime != notPressedTimeValue) {
			pressLastTime = notPressedTimeValue;
			root.onPressUp(pressX, pressY);
		}
	}
	if (detected) {
		pressLastTime = now;
		root.onPressMove(pressX, pressY);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Main application

#ifndef PIO_UNIT_TESTING

void setup()
{
	sampling::voltage::init();

	Serial.begin(921600);
	tft.init();
	tft.setRotation(1);
	tft.setCursor(20, 20);

	bool startTouchCalibration = false;

	EEPROM.begin(256);
	PersistedConfig& config = *reinterpret_cast<PersistedConfig*>(EEPROM.getDataPtr());
	touch::calibrationData = &config.calibrationData;
	if (config.calculateChecksum() == config.checksum) {
		// Allow for touch recalibration on start
		if (touch::anywhere()) {
			LOG_DEBUG("Touch", "Re-calibration requested");
			startTouchCalibration = true;
		}
	}
	else /* checksum miss-match */ {
		LOG_ERROR("EEPROM", "Persisted config checksum miss-match, reseting to default")
		startTouchCalibration = true;
	}

	if (startTouchCalibration) {
		tft.fillScreen(TFT_BLACK);
		tft.println("Touch points on the screen as indicated");
		if (touch::calibrate()) {
			config.prepareForSave();
			EEPROM.commit();
		}
	}

	tft.fillScreen(TFT_BLACK);

	tft.setTextFont(2);
	tft.setTextColor(TFT_WHITE); // also makes font background transparent, avoiding filling
	ui::root.draw();

	// TODO: include sampling config in PersistedConfig
	sampling::setClockDivisor(96);
	ui::timeBaseInput.value = ui::graphDispatch.getCellWidth() * 2;
	// TODO: maybe integrate root.cpp into main.cpp? 

	sampling::init();
	sampling::start();
}

void loop()
{
	processTouchEventsForUI(ui::root);
	ui::root.update();
}

#endif // ifndef PIO_UNIT_TESTING
