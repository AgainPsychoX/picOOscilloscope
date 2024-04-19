#pragma once
#include "../common.hpp"
#include "colors.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Config

using namespace colors;

constexpr color_t channel1Color = to565(RGB { 255, 255, 0 } /* yellow */);
constexpr color_t channel2Color = to565(RGB { 180, 255, 0 } /* green-yellow */);
constexpr color_t centerLineColor = to565(RGB { 127, 127, 127 });
constexpr color_t helperLineColor = to565(RGB { 63, 63, 63 });
constexpr color_t helperLineVoltageLabelColor = to565(RGB { 95, 95, 95 });

////////////////////////////////////////////////////////////////////////////////
// Other stuff

// Shared buffer for various just-in-time UI related strings, 
// like for printf right before drawing contents to the display.
extern char sharedBuffer[32];

}
