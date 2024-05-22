#include <unity.h> // testing framework
#include <Arduino.h>

void setUp(void) {}

void tearDown(void) {}

void test_basic()
{
	delay(123);
}

void setup()
{
	UNITY_BEGIN();
	RUN_TEST(test_basic);
	UNITY_END();
}

void loop() {}
