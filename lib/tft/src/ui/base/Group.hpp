#pragma once
#include <utility>
#include <metaprogramming.hpp>
#include "ui/base/Button.hpp"

namespace ui {

template <template<typename...> typename TContainer>
class Group : public Element
{
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

protected:
	Button* pressedButton = nullptr; // TODO: need to be invalidated/fixed after modifying buttons collection
	TContainer<Button*> buttons;

	TContainer<Element*> others;

	template <typename T>
	struct IsButton : std::is_base_of<Button, T> {};
	template <typename T>
	struct IsButtonPtr : std::is_base_of<Button, std::remove_pointer_t<T>> {};

public:
	/// Construct empty group
	Group()
	{}

	/// Constructs group with specifies elements
	template<typename... Ts>
	inline Group(Ts&&... args)
	{
		using TButtons = mp::filter<IsButtonPtr, Ts...>;
		buttons.reserve(TButtons::length);
		others.reserve(sizeof...(args) - TButtons::length);

		([&] {
			if constexpr (std::is_base_of_v<Button, std::remove_pointer_t<decltype(args)>>)
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
