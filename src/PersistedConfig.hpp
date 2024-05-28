#pragma once
#include <CRC32.h>
#include "touch.hpp"

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
