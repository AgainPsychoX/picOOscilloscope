#pragma once
#include "Element.hpp"
#include <cstdint>

namespace ui {

struct Rectangle : public Element
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;

	Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: x(x), y(y), w(w), h(h)
	{}

	/// @brief Check if screen coords hit the button.
	/// @param sx Hit ray horizontal position on the screen
	/// @param sy Hit ray vertical position on the screen
	/// @return True if the button is hit, false otherwise.
	bool isHit(uint16_t sx, uint16_t sy)
	{
		return x <= sx && sx < x + w && y <= sy && sy < y + h;
	}
};

}
