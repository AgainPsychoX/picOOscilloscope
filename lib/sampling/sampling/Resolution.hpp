#pragma once
#include <cstdint>

namespace sampling {

/// Enum representing possible sampling resolutions.
enum class Resolution : uint8_t
{
	U8 = 8U,
	U12 = 12U,
};

/// Resolution for the sampling.
/// Takes effect after (re)initialisation the sampling module.
extern Resolution resolution;

}
