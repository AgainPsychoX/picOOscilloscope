#pragma once
#include "sampling/voltage/Range.hpp"
#include "sampling/voltage/Shifter.hpp"

namespace sampling::voltage {

/// Voltage shifter modules, one per channel.
extern Shifter shifter[2]; 

/// Initializes the voltage shifters to default state.
void init();

}
