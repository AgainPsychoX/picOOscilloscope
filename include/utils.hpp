#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>

/// Returns result of saturated subtraction. Example: `3 - 7 == 0`.
template<typename T>
constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
saturatedSubtract(T x, T y)
{
	T res = x - y;
	res &= -(res <= x);
	return res;
}
static_assert(saturatedSubtract<uint8_t>(3, 7) == 0);
