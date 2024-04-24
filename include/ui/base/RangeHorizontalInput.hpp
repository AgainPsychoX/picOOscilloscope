#pragma once
#include "Button.hpp"
#include "ui/common.hpp"

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
	/// Padding, from border, to the side labels.
	uint8_t sidesPadding = 4;
	/// Font size to be used to draw the side labels. 0 to disable side labels.
	uint8_t sidesFontSize : 4 = 1;
	/// If true, sides labels will be vertically aligned with value string. 
	/// If false, they will be vertically aligned in button center.
	bool sidesAlignToValue : 1 = false;

	RangeHorizontalInput(
		uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* centerLabel,
		const char* leftLabel = "-", const char* rightLabel = "+", 
		uint8_t sidesFontSize = 255, // 255 (-1) defaults to 1, or 2 if center label is present
		uint8_t sidesPadding = 255,  // 255 (-1) defaults to height divided by 4
		bool sidesAlignToValue = false
	) :
		Button(x, y, w, h), centerLabel(centerLabel),
		leftLabel(leftLabel), rightLabel(rightLabel), 
		sidesPadding(sidesPadding == 255 ? (h / 4) : sidesPadding), 
		sidesFontSize(sidesFontSize == 255 ? (centerLabel ? 2 : 1) : sidesFontSize), 
		sidesAlignToValue(sidesAlignToValue)
	{};

	virtual void draw() override;
	virtual void update() override;

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
	/// Specifies if the press started on the left (true) or right side (false).
	bool pressStartedOnLeft : 1;

	/// Returns true if given X position is on the left side, false otherwise.
	inline bool isLeft(uint16_t sx) const { return sx < x + w / 2; }

	/// Indicates if the press on the original side is still continued.
	/// NOT static, because it's used in `update` to fire the action even if the press is not moved.
	bool originalSidePressContinued : 1 = false;

	// TODO: maybe allow cross over from one side to another (via flag too?), 
	//  allowing change in both directions by just moving around the center, 
	//  maybe reseting the pressStartTime to longPressTimeThreshold 
	//  to reset longPressTickInterval if it uses it.

	bool enableLongPressSpeedUp : 1 = true;

	/// Duration in milliseconds for long-press detection, 
	/// after which firing the left or right action handler is speeded up.
	constexpr static uint32_t longPressTimeThreshold = 500;

	/// Returns true if the current press is long enough.
	inline bool isLongPress() const
	{
		return now - pressStartTime > longPressTimeThreshold;
	}

	/// Last time the action handler was called due to long press.
	/// Static, because only one button can be pressed at the same time.
	static uint32_t lastLongPressTickTime;

	/// Interval in milliseconds between long press ticks (firing the actions).
	/// The higher the value, the slower the value changes on the long press.
	/// Virtual to allow customization by passed time, by value, or other factors.
	virtual uint16_t longPressTickInterval() const { return 250; }

	inline bool isTimeForLongPressTick() const
	{
		return now - lastLongPressTickTime > longPressTickInterval();
	}
private:
	void drawForeground();
	void drawBackgroundPressed();
};

}
