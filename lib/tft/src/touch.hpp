#pragma once
#include <cstdint>
#include <cstddef>

namespace touch
{

// TODO: Adjustable Z threshold? (detect idle state in the calibration)
constexpr uint16_t zThreshold = 150;

struct CalibrationData {
	uint16_t screenWidth;
	uint16_t screenHeight;
	int16_t xScale;
	int16_t yScale;
	int16_t xOffset;
	int16_t yOffset;
	bool rotated; // true if screen X axis is touch Y (and vice-versa)

	/// Converts raw x & y according to this calibration data
	void convertRaw(uint16_t& x, uint16_t& y);

	/// Prints out calibration data into cstring human readable
	int snprintf(char* buffer, size_t maxLength);
};

/// Global pointer to calibration data, used for converting raw values
/// to screen positions. Pointer used to allow using EEPROM easily.
extern CalibrationData* calibrationData;

/// Returns raw X, Y, Z values from the touch controller.
void getRaw(uint16_t& x, uint16_t& y, uint16_t& z);

/// Returns true and updates X & Y variables as screen position for the touch.
/// Returns false if no touch is currently detected.
bool get(uint16_t& xOut, uint16_t& yOut);

/// Returns true if there is touch detected anywhere on the screen, false otherwise.
bool anywhere();

/// Returns true and updates X, Y & Z variables as raw values from the touch controller for the filtered, valid touch.
/// Returns false if no valid touch is currently detected.
bool getFilteredRaw(uint16_t& xOut, uint16_t& yOut, uint16_t& zOut);

/// Returns true and updates X, Y & Z variables as screen position for the filtered, valid touch.
/// Returns false if no touch is currently detected.
bool getFiltered(uint16_t& xOut, uint16_t& yOut);

/// Performs user-assisted calibration. Blocks for a while, drawing arrows on the display,
/// that user is supposed to touch. If successful returns true and updates the global calibration data static variable.
bool calibrate();

}
