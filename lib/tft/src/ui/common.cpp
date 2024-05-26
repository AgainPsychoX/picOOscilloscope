#include "ui/common.hpp"
#include <type_traits>
#include <Arduino.h>
#include <TFT_eSPI.h>

namespace ui {

uint32_t now;
uint32_t pressStartTime;
uint32_t pressLastTime = 0;
uint16_t pressX;
uint16_t pressY;

char sharedBuffer[32];

}

// Asserts to validate definition in `common.hpp`
static_assert(std::is_same_v<std::invoke_result_t<decltype(&TFT_eSPI::readPixel), TFT_eSPI*, int32_t, int32_t>, color_t>);
