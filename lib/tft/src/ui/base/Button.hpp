#pragma once
#include "Rectangle.hpp"

namespace ui {

/// Base button class
struct Button : public Rectangle
{
	Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: Rectangle(x, y, w, h)
	{}

	virtual void draw() override;

	/// @brief Action for the button to be executed on valid full press.
	virtual void action() = 0;

	/// @brief Called when the button is pressed down.
	/// @param sx Horizontal position on the screen (not relative to the button)
	/// @param sy Vertical position on the screen (not relative to the button)
	virtual void onPressDown(uint16_t sx, uint16_t sy);

	/// @brief Called when press is removed from the button.
	///	@note Use `isHit` to determine if last touch was still on the button.
	/// @param sx Horizontal position on the screen (not relative to the button)
	/// @param sy Vertical position on the screen (not relative to the button)
	virtual void onPressUp(uint16_t sx, uint16_t sy);

	/// @brief Called when press is moved while the button is being press.
	/// @param sx Horizontal position on the screen (not relative to the button)
	/// @param sy Vertical position on the screen (not relative to the button)
	virtual void onPressMove(uint16_t sx, uint16_t sy);

	// TODO: make button background change smoother (simple animation)
};

}
