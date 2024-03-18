#include <limits>
#include <SPI.h>
#include <TFT_eSPI.h>
#undef abs

TFT_eSPI tft = TFT_eSPI();

namespace touch {

// TODO: Adjustable Z threshold? (detect idle state in the calibration)
constexpr uint16_t zThreshold = 150;

struct CalibrationData {
	uint16_t screenWidth;
	uint16_t screenHeight;
	int16_t xScale;
	int16_t yScale;
	int16_t xOffset;
	int16_t yOffset;
	// uint16_t zThreshold;
	bool rotated; // true if screen X axis is touch Y (and vice-versa)

	void convertRaw(uint16_t& x, uint16_t& y)
	{
		if (rotated) {
			uint16_t t = y;
			y = (x - xOffset) * screenHeight / xScale;
			x = (t - yOffset) * screenWidth  / yScale;
		}
		else {
			x = (x - xOffset) * screenWidth  / xScale;
			y = (y - yOffset) * screenHeight / yScale;
		}
	}

	int snprintf(char* buffer, size_t maxLength) {
		return std::snprintf(
			buffer, maxLength, "w=%u h=%u xS=%d yS=%d xO=%d yO=%d",
			screenWidth, screenHeight, xScale, yScale, xOffset, yOffset
		);
	}
};
CalibrationData calibrationData;

bool getTouch(uint16_t& x, uint16_t& y)
{
	uint16_t z = tft.getTouchRawZ();
	if (z >= zThreshold) {
		tft.getTouchRaw(&x, &y);
		calibrationData.convertRaw(x, y);
		return true;
	}
	return false;
}

bool getTouchDebouncedZ(uint16_t& xOut, uint16_t& yOut)
{
	// Wait until pressure stops increasing to debounce pressure
	uint16_t z = tft.getTouchRawZ();
	uint16_t zPrevious;
	do {
		zPrevious = z;
		delay(1);
		z = tft.getTouchRawZ();
	}
	while (z > zPrevious);

	uint16_t x;
	uint16_t y;
	static uint16_t xPrevious;
	static uint16_t yPrevious;

	// Eliminate more than deadband error touch position sudden runaways
	constexpr auto deadbandErrorLimit = 20;
	tft.getTouchRaw(&x, &y);
	bool tooFar = std::abs(x - xPrevious) > deadbandErrorLimit 
	           || std::abs(y - yPrevious) > deadbandErrorLimit;
	xPrevious = x;
	yPrevious = y;
	if (tooFar) return false;

	if (z >= zThreshold) {
		calibrationData.convertRaw(x, y);
		xOut = x;
		yOut = y;
		return true;
	}
	return false;
}

bool getTouchAverage(uint16_t& x, uint16_t& y, uint8_t samplesTwoExponent = 4)
{
	static uint16_t ax, ay, az;
	static unsigned long lastTime;
	uint16_t rx, ry, rz;
	tft.getTouchRaw(&rx, &ry);
	rz = tft.getTouchRawZ();

	// TODO: make it distance based instead time 
	unsigned long now = micros();
	if (now - lastTime < 1'000) {
		rx = (ax + rx) / 2;
		ry = (ay + ry) / 2;
		rz = (az + rz) / 2;
	}
	ax = rx;
	ay = ry;
	az = rz;
	lastTime = now;

	if (rz >= zThreshold) {
		calibrationData.convertRaw(rx, ry);
		x = rx;
		y = ry;
		return true;
	}
	return false;
}

void calibrate()
{
	uint16_t w = tft.width();   // Width of the display
	uint16_t h = tft.height();  // Height of the display
	uint8_t as = 15;            // Arrow size (0 means 1px)
	uint16_t bgc = TFT_BLACK;   // Background color
	uint16_t ac = TFT_MAGENTA;  // Arrows color

	struct Corner {
		uint16_t minX = std::numeric_limits<uint16_t>::max();
		uint16_t minY = std::numeric_limits<uint16_t>::max();
		uint16_t maxX = 0;
		uint16_t maxY = 0;
	};
	Corner corners[4];

	// 5 steps (corners + clear)
	for (uint8_t i = 0; i < 5; i++) {
		// Clear all arrows
		tft.fillRect(0, 0, as+1, as+1, bgc);
		tft.fillRect(0, h-as-1, as+1, as+1, bgc);
		tft.fillRect(w-as-1, 0, as+1, as+1, bgc);
		tft.fillRect(w-as-1, h-as-1, as+1, as+1, bgc);

		if (i >= 4) break; // only clear

		// Draw arrows
		switch (i) {
			case 0: // up left
				tft.drawLine(0, 0, 0, as, ac);
				tft.drawLine(0, 0, as, 0, ac);
				tft.drawLine(0, 0, as, as, ac);
				break;
			case 1: // bottom left
				tft.drawLine(0, h-as-1, 0, h-1, ac);
				tft.drawLine(0, h-1, as, h-1, ac);
				tft.drawLine(as, h-as-1, 0, h-1 , ac);
				break;
			case 2: // up right
				tft.drawLine(w-as-1, 0, w-1, 0, ac);
				tft.drawLine(w-as-1, as, w-1, 0, ac);
				tft.drawLine(w-1, as, w-1, 0, ac);
				break;
			case 3: // bottom right
				tft.drawLine(w-as-1, h-as-1, w-1, h-1, ac);
				tft.drawLine(w-1, h-1-as, w-1, h-1, ac);
				tft.drawLine(w-1-as, h-1, w-1, h-1, ac);
				break;
		}

		if (i > 0) delay(1000); // allow user to release from previous arrow

		// Read the values until no more updates
		uint16_t timeout = 5000;
		bool anything = false;
		do {
			bool updates = false;

			uint16_t x, y, z;
			z = tft.getTouchRawZ();
			if (z >= zThreshold) {
				tft.getTouchRaw(&x, &y);

				if (corners[i].minX > x) {
					corners[i].minX = x;
					updates = true;
				}
				if (corners[i].minY > y) {
					corners[i].minY = y;
					updates = true;
				}
				if (corners[i].maxX < x) {
					corners[i].maxX = x;
					updates = true;
				}
				if (corners[i].maxY < y) {
					corners[i].maxY = y;
					updates = true;
				}
				
				if (updates) {
					anything = true;
					timeout = 2000;
				}
			}

			delay(1);
		}
		while (--timeout);

		if (!anything) {
			// Failed calibration, no touch detected after long time
			if (Serial) Serial.println("Failed to calibrate!");
			tft.fillScreen(bgc);
			tft.fillRect(0, 0, as+1, as+1, TFT_RED);
			delay(1000);
			return;
		}
	}

	// Use collected data to fill calibration data
	calibrationData.screenWidth = w;
	calibrationData.screenHeight = h;
	auto xVerticalDifference = std::abs(corners[0].maxX - corners[1].maxX);
	auto xHorizontalDifference = std::abs(corners[0].maxX - corners[2].maxX);
	calibrationData.rotated = xVerticalDifference > xHorizontalDifference;
	if (calibrationData.rotated) /* x is vertical, so y is horizontal */ {
		if (corners[0].maxX < corners[1].maxX) /* x grows towards bottom */ {
			calibrationData.xOffset = corners[0].minX;
			calibrationData.xScale = corners[0].minX - corners[1].maxX;
		}
		else /* x drops towards bottom */ {
			calibrationData.xOffset = corners[0].maxX;
			calibrationData.xScale = corners[1].minX - corners[0].maxX;
		}
		if (corners[0].maxY < corners[2].maxY) /* y grows towards right */ {
			calibrationData.yOffset = corners[0].minY;
			calibrationData.yScale = corners[0].minY - corners[2].maxY;
		}
		else /* y drops towards right */ {
			calibrationData.yOffset = corners[0].maxY;
			calibrationData.yScale = corners[2].minY - corners[0].maxY;
		}
	}
	else /* x is horizontal, so y is vertical */ {
		if (corners[0].maxX < corners[2].maxX) /* x grows towards right */ {
			calibrationData.xOffset = corners[0].minX;
			calibrationData.xScale = corners[0].minX - corners[2].maxX;
		}
		else /* x drops towards right */ {
			calibrationData.xOffset = corners[0].maxX;
			calibrationData.xScale = corners[2].minX - corners[0].maxX;
		}
		if (corners[0].maxY < corners[1].maxY) /* y grows towards bottom */ {
			calibrationData.yOffset = corners[0].minY;
			calibrationData.yScale = corners[0].minY - corners[1].maxY;
		}
	else /* y drops towards bottom */ {
			calibrationData.yOffset = corners[0].maxY;
			calibrationData.yScale = corners[1].minY - corners[0].maxY;
		}
	}
}

}

int16_t centerBoxX;
int16_t centerBoxY;

void setup()
{
	Serial.begin(115200);
	tft.init();
	tft.setRotation(1);

	tft.fillScreen(TFT_BLACK);
	// tft.setCursor(20, 20);
	// tft.setTextSize(1);
	// tft.setTextColor(TFT_WHITE, TFT_BLACK);
	// tft.println("Touch corners as indicated");
	touch::calibrate();

	char buffer[256];
	touch::calibrationData.snprintf(buffer, sizeof(buffer));
	Serial.println(buffer);

	tft.fillScreen(TFT_BLACK);

	centerBoxX = tft.width()/3;
	centerBoxY = tft.height()/3;
}

void loop()
{
	uint16_t x, y, z;

	char buffer[64];
	tft.setCursor(centerBoxX, centerBoxY);

	// Raw values
	tft.getTouchRaw(&x, &y);
	z = tft.getTouchRawZ();
	sprintf(buffer, "rx=%5u ry=%5u rz=%5u     \n", x, y, z);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.print(buffer);

	// Converted values
	if (touch::getTouch(x, y)) {
		tft.fillCircle(x, y, 2, TFT_DARKGREY);
	}
	sprintf(buffer, " x=%5u  y=%5u     \n", x, y);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.print(buffer);

	// Converted values (after average)
	// if (touch::getTouchAverage(x, y)) {
	if (touch::getTouchDebouncedZ(x, y)) {
		tft.fillCircle(x, y, 2, TFT_WHITE);
	}
	sprintf(buffer, "ax=%5u ay=%5u     \n", x, y);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.print(buffer);

	// Clearing canvas by pressing up-left corner of the center box
	if (centerBoxX < x && x < centerBoxX + 40 && 
		centerBoxY < y && y < centerBoxY + 40) {
		tft.fillScreen(TFT_BLACK);
	}
}
