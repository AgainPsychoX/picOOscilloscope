#pragma once
#include <vector>
#include "common.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////

struct Element
{
	/// Preforms full draw of the element
	virtual void render() = 0;

	/// Preforms partial draw of the element, updating small part at the time,
	/// without necessarily redrawing unaffected areas. Defaults to `render()`.
	virtual void partialRender()
	{
		render();
	}
};

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
	/// @param hx Hit ray horizontal position on the screen
	/// @param hy Hit ray vertical position on the screen
	/// @return True if the button is hit, false otherwise.
	bool isHit(uint16_t hx, uint16_t hy)
	{
		return x <= hx && hx < x + w && y <= hy && hy < y + h;
	}
};

////////////////////////////////////////////////////////////////////////////////

/// Base button class
struct Button : public Rectangle
{
	Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		: Rectangle(x, y, w, h)
	{}

	virtual void render() override;

	/// @brief Action for the button to be executed on valid full press.
	virtual void action() = 0;

	/// @brief Called when the button is pressed down.
	/// @param hx Horizontal position on the screen (not relative to the button)
	/// @param hy Vertical position on the screen (not relative to the button)
	virtual void onPressDown(uint16_t hx, uint16_t hy);

	/// @brief Called when press is removed from the button.
	///	@note Use `isHit` to determine if last touch was still on the button.
	/// @param hx Horizontal position on the screen (not relative to the button)
	/// @param hy Vertical position on the screen (not relative to the button)
	virtual void onPressUp(uint16_t hx, uint16_t hy);

	// TODO: make button background change smoother (simple animation)
};

////////////////////////////////////////////////////////////////////////////////

/// Default button with text inside
struct TextButton : public Button
{
	const char* text;

	TextButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* text)
		: Button(x, y, w, h), text(text)
	{}

	virtual void render() override;
	virtual void onPressDown(uint16_t hx, uint16_t hy) override;

private:
	void render(uint16_t backgroundColor);
};

////////////////////////////////////////////////////////////////////////////////

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

	virtual void render() override;
	virtual void action() override { /* allow to be omitted */ }
	virtual void onPressDown(uint16_t hx, uint16_t hy) override;
	virtual void onPressUp(uint16_t hx, uint16_t hy) override;

	/// Returns string representation of the current value, to be displayed in the center of the button.
	virtual const char* valueString() = 0;

	/// Action to be executed on valid full press on left-side of the button.
	virtual void onLeftAction() = 0;

	/// Action to be executed on valid full press on right-side of the button.
	virtual void onRightAction() = 0;

protected:
	bool pressStartedOnLeft;
	inline bool isLeft(uint16_t hx)  { return hx < x + w / 2; }
private:
	void renderForeground();
};

////////////////////////////////////////////////////////////////////////////////

struct Text : public Element
{
	uint16_t x;
	uint16_t y;
	const char* text;

	virtual void render() override;
};

////////////////////////////////////////////////////////////////////////////////

template <template<typename...> typename TContainer>
class Group : public Element
{
private:
	bool hidden = false; // skip rendering & interactions if true
public:
	void hide()
	{
		hidden = true;
		if (pressedButton) {
			pressedButton->onPressUp(-1, -1);
			pressedButton = nullptr;
		}
	}
	void unhide()
	{
		hidden = true;
	}
	inline bool isHidden() const 
	{
		return hidden;
	}

protected:
	Button* pressedButton = nullptr; // TODO: need to be invalidated/fixed after modifying buttons collection
	TContainer<Button*> buttons;

	TContainer<Element*> others;

public:
	/// Construct empty group
	Group()
	{}

	/// Constructs group with specifies elements
	template<typename... Ts>
	inline Group(Ts&&... args)
	{
		// TODO: separate buttons and elements to separate containers
		buttons.reserve(sizeof...(args));
		(buttons.emplace_back(std::forward<Ts>(args)), ...);
	}

	// TODO: figure out how to make more static thing (no allocs)

	~Group()
	{
		// TODO: ? free the pointers? or use smart pointers?
	}

public:
	virtual void render() override
	{
		if (hidden) return;

		for (auto& other : others) {
			other->render();
		}
		for (auto& button : buttons) {
			button->render();
		}
	}

	void onPressDown(uint16_t x, uint16_t y)
	{
		if (hidden) return;

		pressedButton = nullptr;
		for (auto& button : buttons) {
			if (button->isHit(x, y)) {
				button->onPressDown(x, y);
				pressedButton = button;
			}
		}
	}

	void onPressUp(uint16_t x, uint16_t y)
	{
		if (hidden) return;

		if (pressedButton) {
			pressedButton->onPressUp(x, y);
			pressedButton = nullptr;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

using RootGroup = Group<std::vector>;

extern RootGroup root;

}

// TODO: Font management?
