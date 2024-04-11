#include "common.hpp"
#include <type_traits>
#include <Arduino.h>
#include <TFT_eSPI.h>

// Asserts to validate definition in `common.hpp`
static_assert(std::is_same_v<decltype(millis()), millis_t>);
static_assert(std::is_same_v<std::invoke_result_t<decltype(&TFT_eSPI::readPixel), TFT_eSPI*, int32_t, int32_t>, color_t>);
