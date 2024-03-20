#include <limits>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "touch.hpp"

TFT_eSPI tft = TFT_eSPI();

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

uint16_t countSimple = 0;
uint16_t countAdvanced = 0;

void loop()
{
	uint16_t x, y, z;

	char buffer[64];
	tft.setCursor(centerBoxX, centerBoxY);

	// Raw values
	touch::getRaw(x, y, z);
	sprintf(buffer, "rx=%5u ry=%5u rz=%5u     ", x, y, z);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.println(buffer);

	// Converted values
	if (touch::get(x, y)) {
		tft.fillCircle(x, y, 2, TFT_LIGHTGREY);
		countSimple++;
	}
	sprintf(buffer, " x=%5u  y=%5u     ", x, y);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.println(buffer);

	// Converted values (after average)
	// if (touch::getTouchAverage(x, y)) {
	if (touch::getFiltered(x, y)) {
		tft.fillCircle(x, y, 2, TFT_GREEN);
		countAdvanced++;
	}
	sprintf(buffer, "ax=%5u ay=%5u     ", x, y);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.println(buffer);

	// Counters
	sprintf(buffer, "cs=%5u ca=%5u     ", countSimple, countAdvanced);
	tft.setCursor(centerBoxX, tft.getCursorY());
	tft.println(buffer);

	// Clearing canvas by pressing up-left corner of the center box
	if (centerBoxX < x && x < centerBoxX + 40 && 
		centerBoxY < y && y < centerBoxY + 40) {
		tft.fillScreen(TFT_BLACK);
		countSimple = 0;
		countAdvanced = 0;
	}
}
