#pragma once
#include <utility>
#include <metaprogramming.hpp>
#include "ui/base/Button.hpp"

namespace ui {

template <template<typename...> typename TContainer>
class Group : public Element
{
	////////////////////////////////////////
	// Elements
public:
	TContainer<Button*> buttons; // buttons, also have press events
	TContainer<Element*> others; // other elements, only drawing and updating

protected:
	/// Helper struct to check if the type is a Button.
	/// Used in templated constructor.
	template <typename T>
	struct IsButton : std::is_base_of<Button, std::remove_pointer_t<T>> {};

	////////////////////////////////////////
	// Constructors
public:
	/// Construct empty group
	Group()
	{}

	/// Constructs group with specified elements
	template<typename... Ts>
	inline Group(Ts&&... args)
	{
		using TButtons = mp::filter<IsButton, Ts...>;
		buttons.reserve(TButtons::length);
		others.reserve(sizeof...(args) - TButtons::length);

		([&] {
			if constexpr (std::is_base_of_v<Button, std::remove_pointer_t<std::remove_reference_t<decltype(args)>>>)
				buttons.emplace_back(std::forward<Ts>(args));
			else
				others.emplace_back(std::forward<Ts>(args));
		} (), ...);
	}

	// TODO: figure out how to make more static thing (no allocs)

	~Group()
	{
		// TODO: ? free the pointers? or use smart pointers?
	}

	////////////////////////////////////////
	// Hiding (skipping drawing & interactions)
private:
	bool hidden = false; // skip drawing & interactions if true
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

	////////////////////////////////////////
	// Element drawing & update cascading
public:
	virtual void draw() override
	{
		if (hidden) return;

		for (auto& other : others) {
			other->draw();
		}
		for (auto& button : buttons) {
			button->draw();
		}
	}

	virtual void update() override
	{
		if (hidden) return;

		for (auto& other : others) {
			other->update();
		}
		for (auto& button : buttons) {
			button->update();
		}
	}

	////////////////////////////////////////
	// Press state
public:
	/// Holds pointer to currently pressed button or `nullptr` if none.
	/// Might need to be invalidated/fixed after modifying buttons collection.
	Button* pressedButton = nullptr;

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

	void onPressMove(uint16_t x, uint16_t y)
	{
		if (hidden) return;

		if (pressedButton) {
			pressedButton->onPressMove(x, y);
		}
	}
};

}
