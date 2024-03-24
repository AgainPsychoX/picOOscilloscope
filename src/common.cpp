#include "common.hpp"
#include <type_traits>
#include <Arduino.h>

// Assert to validate definition in `common.hpp`
static_assert(std::is_same_v<decltype(millis()), millis_t>);
