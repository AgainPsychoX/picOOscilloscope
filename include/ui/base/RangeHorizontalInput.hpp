#pragma once
#include "Button.hpp"

namespace ui {

/// Default button allowing to pick value from range using sub-buttons on sides.
///
/// User might want to use '-', '<', 'a', etc., so strings can be provided
/// for left and right labels. Used string instead single char to support UTF8 
/// with/or smooth fonts glyphs. Also it makes implementation easier.
struct RangeHorizontalInput : public Button
{
	/// Optional (null to skip) label to be displayed above value string representation.
	const char* centerLabel;

	/// Label for the left side.
	const char* leftLabel = "-";
	/// Label for the right side.
	const char* rightLabel = "+";
	/// Font size to be used to draw the side labels. 0 to disable side labels.
	uint8_t sidesFontSize = 1;
	/// Padding, from border, to the side labels.
	uint8_t sidesPadding = 4;
	/// If true, sides labels will be vertically aligned with value string. 
	/// If false, they will be vertically aligned in button center.
	bool sidesAlignToValue = false;

	RangeHorizontalInput(
		uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* centerLabel,
		const char* leftLabel = "-", const char* rightLabel = "+", 
		uint8_t sidesFontSize = 255, // 255 (-1) defaults to 1, or 2 if center label is present
		uint8_t sidesPadding = 255,  // 255 (-1) defaults to height divided by 4
		bool sidesAlignToValue = false
	) :
		Button(x, y, w, h), centerLabel(centerLabel),
		leftLabel(leftLabel), rightLabel(rightLabel), 
		sidesFontSize(sidesFontSize == 255 ? (centerLabel ? 2 : 1) : sidesFontSize), 
		sidesPadding(sidesPadding == 255 ? (h / 4) : sidesPadding), 
		sidesAlignToValue(sidesAlignToValue)
	{};

	virtual void draw() override;

	/// Ignore, use `onLeftAction` or `onRightAction` instead.
	virtual void action() override {};

	virtual void onPressDown(uint16_t sx, uint16_t sy) override;
	virtual void onPressUp(uint16_t sx, uint16_t sy) override;
	virtual void onPressMove(uint16_t sx, uint16_t sy) override;

	/// Returns string representation of the current value, to be displayed in the center of the button.
	virtual const char* valueString() = 0;

	/// Action to be executed on valid full press on left-side of the button.
	virtual void onLeftAction() = 0;

	/// Action to be executed on valid full press on right-side of the button.
	virtual void onRightAction() = 0;

protected:
	bool pressStartedOnLeft;
	inline bool isLeft(uint16_t sx)  { return sx < x + w / 2; }
private:
	void drawForeground();
};

}
