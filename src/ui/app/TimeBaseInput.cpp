#include "ui/app/TimeBaseInput.hpp"
#include "tft.hpp"

namespace ui {

uint16_t timeBaseIntegers[] {
	1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 50, 64, 80, 100, 128, 160, 200, 256, 320, 400, 500, 512, 800
	//	1	2	4		8		16		32			64			128			256				512
	//				5							50		 								500
	//						10		20		40			80			160			320
	//														100			200			400		800
};
constexpr auto timeBaseIntegersCount = sizeof(timeBaseIntegers) / sizeof(*timeBaseIntegers);

const char* timeBaseUnits = "us\0" "ms\0" "s\0" "\0";

const char* TimeBaseInput::valueString()
{
	assert(value < 1'000'000'000);
	const char* unit = timeBaseUnits;
	auto integer = value;
	while (integer >= 1000) {
		if (integer % 1000) {
			break;
		}
		else {
			integer /= 1000;
			unit += 3; // first from 'us' to 'ms' then to 's'
		}
	}
	snprintf(sharedBuffer, sizeof(sharedBuffer), "%lu %s", integer, unit);
	return sharedBuffer;
}

// TODO: allow precise manipulation of the value (per us)

void TimeBaseInput::onLeftAction()
{
	// Find current or next closest integer option
	auto closestInteger = value;
	uint8_t unitIdx = 0; // 0 = us, 1 = ms, 2 = s
	while (closestInteger >= 1000) {
		closestInteger /= 1000;
		unitIdx++;
	}
	uint8_t integerIdx = 0;
	while (timeBaseIntegers[integerIdx] < closestInteger) {
		integerIdx++;
	}

	// Select previous integer option
	if (integerIdx == 0) {
		if (unitIdx != 0) /* not microseconds */ {
			integerIdx = timeBaseIntegersCount - 1;
			unitIdx--;
		}
	}
	else {
		integerIdx--;
	}
	value = timeBaseIntegers[integerIdx];
	while (unitIdx--) {
		value *= 1000;
	}

	// TODO: prompt about going too close/below sampling rate
}

void TimeBaseInput::onRightAction()
{
	// Find current or previous closest integer option
	auto closestInteger = value;
	uint8_t unitIdx = 0; // 0 = us, 1 = ms, 2 = s
	while (closestInteger >= 1000) {
		closestInteger /= 1000;
		unitIdx++;
	}
	uint8_t integerIdx = timeBaseIntegersCount - 1;
	if (closestInteger < timeBaseIntegers[0]) {
		integerIdx = -1; // overflow few lanes below will make it to idx 0
	}
	else {
		while (closestInteger < timeBaseIntegers[integerIdx]) {
			integerIdx--;
		}
	}

	// Select previous integer option
	if (integerIdx == timeBaseIntegersCount - 1) {
		if (unitIdx != 2) /* not seconds */ {
			integerIdx = 0;
			unitIdx++;
		}
	}
	else {
		integerIdx++;
	}
	value = timeBaseIntegers[integerIdx];
	while (unitIdx--) {
		value *= 1000;
	}
}

}
