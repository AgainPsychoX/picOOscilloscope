#pragma once
#include <vector>
#include "common.hpp"

namespace ui {

////////////////////////////////////////////////////////////////////////////////

struct Element
{
	/// Preforms full re-render of the element
	virtual void render() = 0;
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
	bool isHit(uint16_t hx, uint16_t hy) {
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
	/// @param x Horizontal position on the screen (not relative to the button)
	/// @param y Vertical position on the screen (not relative to the button)
	virtual void onPressDown(uint16_t x, uint16_t y);

	/// @brief Called when press is removed from the button.
	///	@note Use `isHit` to determine if last touch was still on the button.
	/// @param x Horizontal position on the screen (not relative to the button)
	/// @param y Vertical position on the screen (not relative to the button)
	virtual void onPressUp(uint16_t x, uint16_t y);

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

	virtual void onPressDown(uint16_t x, uint16_t y) override;
	virtual void render() override;

private:
	void render(uint16_t backgroundColor);
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
	Group(Ts&&... args)
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
