#include <unity.h>
#include <ui/base/ColoredTextButton.hpp>

namespace ui {

struct TestPassButton : public ColoredTextButton
{
	static constexpr color_t color = colors::to565(colors::RGB { 40, 167, 69 });

	TestPassButton(
		uint16_t x = 380, uint16_t y = 240, uint16_t w = 100, uint16_t h = 40,
		const char* text = "TEST PASS"
	) : 
		ColoredTextButton(x, y, w, h, text, color, color)
	{}

	virtual void action() override
	{
		TEST_PASS();
	}
};

struct TestFailButton : public ColoredTextButton
{
	static constexpr color_t color = colors::to565(colors::RGB { 220, 53, 69 });
	
	TestFailButton(
		uint16_t x = 380, uint16_t y = 280, uint16_t w = 100, uint16_t h = 40,
		const char* text = "TEST FAIL"
	) : 
		ColoredTextButton(x, y, w, h, text, color, color)
	{}

	virtual void action() override
	{
		TEST_FAIL();
	}
};

}
