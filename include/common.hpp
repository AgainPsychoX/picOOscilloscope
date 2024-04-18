#pragma once
#include <cstdint>
#include <cstddef>

/// Type for number of milliseconds across Arduino functions like `millis()` or `delay(ms)`
using millis_t = unsigned long; 

/// Type for color used by TFT_eSPI library (RGB565 format)
using color_t = uint16_t;
