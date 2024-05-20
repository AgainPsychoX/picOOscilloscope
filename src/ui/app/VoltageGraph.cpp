#include "ui/app/VoltageGraph.hpp"
#include "ui/app/TimeBaseInput.hpp"
#include "ui/app/TimeOffsetInput.hpp"
#include "ui/tft.hpp"
#include "sampling.hpp"
#include "logging.hpp"

namespace ui {

const char* VoltageGraph::getYLabel(uint8_t cy) const
{
	auto foo = heightDivisions / 2 - cy;
	if (foo == 0) {
		return nullptr;
	}
	int16_t mV = yValueStep * foo;

	// Convert mV to string (with sign, up to 2 decimal places, max width 4)
	// TODO: consider avoiding printf here? https://godbolt.org/z/8o4Escvhb
	int w = sprintf(sharedBuffer, "%+.4d", mV);
	char* p = sharedBuffer + w - 5;
	p[4] = p[3];
	p[3] = p[2];
	p[2] = '.';
	sharedBuffer[5] = 0;

	// TODO: clear debug logging as trace log
	LOG_TRACE("Graphing", "cy=%u mV=%d buf='%s'", cy, mV, sharedBuffer);
	return sharedBuffer;
}

extern TimeBaseInput timeBaseInput;
extern TimeOffsetInput timeOffsetInput;

void VoltageGraph::drawSeries()
{
	using namespace sampling;

	// Calculate offset from graphing window start to trigger point
	const uint32_t offsetInTime = timeOffsetInput.value + (timeBaseInput.value * widthDivisions / 2);
	const size_t desiredOffsetInSamples = offsetInTime / getTimeBetweenSamples();
	const size_t maxOffsetInSamples = getNumberOfPreTriggerSamples();
	const bool isMissingSamples = desiredOffsetInSamples > maxOffsetInSamples;
	const size_t offsetInSamples = isMissingSamples ? maxOffsetInSamples : desiredOffsetInSamples;

	// Each pixel represents `timeBase / cellWidth` of time, 
	// so `timeBase / timeBetweenSamples / cellWidth` samples.
	// To avoid floating point division and multiplications and rounding etc, 
	// step by step method is implemented.

	const uint32_t samplesPerPixelNumerator = timeBaseInput.value;
	const uint32_t samplesPerPixelDivisor = getTimeBetweenSamples() * cellWidth;
	uint32_t a = samplesPerPixelNumerator; // accumulator

	uint_fast16_t xLocal = 0;

	if (isMissingSamples) {
		size_t samplesMissing = desiredOffsetInSamples - maxOffsetInSamples;
		do {
			if (a >= samplesPerPixelDivisor) {
				a -= samplesPerPixelDivisor;
			}
			else {
				a += samplesPerPixelNumerator;
				xLocal += 1;
			}
		}
		while (--samplesMissing);
	}

	uint_fast8_t xCellIndex = 0;
	uint_fast8_t xInsideCell;
	// TODO: benchmark and maybe replace with regular division (or some variation)
	for (auto foo = xLocal; ; foo -= cellWidth) {
		if (foo < cellWidth) {
			xInsideCell = foo;
			break;
		}
		xCellIndex += 1;
	}

	const auto numberOfSamples = getNumberOfSamples();
	size_t i = 0; // samples used so far; to avoid repeating

	switch (resolution) {
		default:
		case Resolution::U8: {
			auto p = getSamplesIterator<const uint8_t>();
			p -= offsetInSamples;
			do {
				if (a >= samplesPerPixelDivisor) {
					a -= samplesPerPixelDivisor;

					uint8_t value = p.get();
					
					uint_fast16_t h =
						(value * valueGraphingMultiplier + valueGraphingShift)
						/ valueGraphingDividerForHeight;

					tft.drawPixel(
						x + xLocal,
						valueGraphingBottomScreenY - h,
						channel1Color);

					// markCellDirty(xCellIndex, yCellIndex);
				}
				else {
					a += samplesPerPixelNumerator;
					if (++xInsideCell == cellWidth) {
						xInsideCell = 0;
						xCellIndex += 1;
					}
					if (++xLocal >= width) break;
				}

				if (++i >= numberOfSamples) break;
				++p;
			}
			while (true);
			break;
		}
		case Resolution::U12: {
			// TODO: ...
			break;
		}
	}
}

void VoltageGraph::updateValueGraphingCache(sampling::voltage::Range range)
{
	valueGraphingMultiplier = range.width() * cellHeight;
	uint ΔmV = (heightDivisionsBelowZero() * yValueStep - range.minVoltage());
	valueGraphingShift = ΔmV * cellHeight * 255u /*to make up for divider*/;
	valueGraphingDividerForHeight = 255u * yValueStep;
	valueGraphingBottomScreenY = y + height - halfHeightWasted;
}

}
