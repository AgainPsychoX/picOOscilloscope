#include <unity.h> // testing framework
#include <Arduino.h>
#include <Bootsel.h> // boolean-like object that can be used to detect the BOOTSEL button state

void suiteSetUp()
{
	gpio_init(LED_BUILTIN);
	gpio_set_dir(LED_BUILTIN, GPIO_OUT);
}

int suiteTearDown(int num_failures)
{
	// Turn on LED if all tests passed, otherwise turn it off
	gpio_put(LED_BUILTIN, num_failures == 0);
	return num_failures;
}

void setUp() {}
void tearDown() {}

/// Simple interactive test to use BOOTSEL button and onboard LED.
/// To pass, the button must be pressed while the LED is on.
void test_the_button()
{
	Serial.println("Press the button while the LED is on! :)");
	auto start = to_ms_since_boot(get_absolute_time());

	// Loop, breaks on TEST_FAIL or TEST_PASS using the button or timeout
	while (true) {
		auto now = to_ms_since_boot(get_absolute_time());
		if (now - start > 10000) {
			TEST_FAIL_MESSAGE("Timeout");
		}

		bool light = now % 2048 > 1024;
		gpio_put(LED_BUILTIN, light);

		if (BOOTSEL) {
			if (light)
				TEST_PASS_MESSAGE("Button pressed on light!");
			else
				TEST_FAIL_MESSAGE("Button pressed on no light!");
		}

		yield(); // not necessary, but just in case
	}
}

void setup()
{
	suiteSetUp();
	UNITY_BEGIN();
	RUN_TEST(test_the_button);
	suiteTearDown(UNITY_END());
}

void loop() {}
