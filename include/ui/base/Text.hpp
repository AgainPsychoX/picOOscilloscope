#pragma once
#include "Element.hpp"
#include <cstdint>

namespace ui {

struct Text : public Element
{
	uint16_t x;
	uint16_t y;
	const char* text;

	virtual void render() override;
};

}
