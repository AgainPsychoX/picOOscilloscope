#include <limits>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include <CRC32.h>
#include "common.hpp"
#include "touch.hpp"
#include "sampling.hpp"
#include "ui/root.hpp"

TFT_eSPI tft = TFT_eSPI();

struct PersistedConfig
{
	////////////////////////////////////////
	// 0x000 - 0x020: Checksum

	uint8_t _emptyBeginPad[28];
	uint32_t checksum;

	uint32_t calculateChecksum() {
		constexpr uint16_t prefixLength = offsetof(PersistedConfig, checksum) + sizeof(checksum);
		return CRC32::calculate(reinterpret_cast<uint8_t*>(this) + prefixLength, sizeof(PersistedConfig) - prefixLength);
	}

	bool prepareForSave() {
		uint32_t calculatedChecksum = calculateChecksum();
		bool changed = checksum != calculatedChecksum;
		checksum = calculatedChecksum;
		return changed;
	}

	////////////////////////////////////////
	// 0x020 - ...: Touch calibration data

	touch::CalibrationData calibrationData;
};

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
			// TODO: LOG_DEBUG("Touch re-calibration requested");
			startTouchCalibration = true;
		}
	}
	else /* checksum miss-match */ {
		// TODO: LOG_ERROR("Persisted config checksum miss-match, reseting to default")
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
	ui::graph.draw();

	sampling::init();
	sampling::start();
}

void loop()
{
	using namespace ui;
	
	uint32_t now = to_ms_since_boot(get_absolute_time());

	bool detected = touch::getFiltered(pressX, pressY);
	bool wasReleased = now - pressLastTime > 100;
	if (wasReleased) {
		if (detected) {
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

	root.update();
	graph.update();
}
