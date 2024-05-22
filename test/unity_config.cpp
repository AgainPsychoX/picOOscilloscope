
////////////////////////////////////////////////////////////////////////////////
#ifdef ARDUINO

////////////////////////////////////////
// Clock, time measurement fixes

#include <pico/time.h>

extern "C" unsigned int unityClock()
{
	return to_ms_since_boot(get_absolute_time());
}

////////////////////////////////////////
// Fix Unity logging for Arduino-Pico

#ifdef DEBUG_RP2040_PORT
// If set, the standard output (incl. `printf`) is properly redirected
// by Arduino-Pico core to the selected serial port, so tests work properly.
// Otherwise, we need tell Unity how to use serial port directly.
#else
#	include <Arduino.h>
#	ifndef LOG_ARDUINO_PORT
#		define LOG_ARDUINO_PORT Serial
#	endif

extern "C" void unityOutputChar(char c)
{
	LOG_ARDUINO_PORT.write(c);
}
extern "C" void unityOutputFlush()
{
	LOG_ARDUINO_PORT.flush();
}
extern "C" void unityOutputStart()
{
	delay(500);
	LOG_ARDUINO_PORT.begin(115200);
	constexpr int msStep = 100;
	for (int msLeft = 2000; msLeft > 0; msLeft -= msStep) {
		delay(msStep);
		LOG_ARDUINO_PORT.print(F("Testing starts in "));
		LOG_ARDUINO_PORT.print(msLeft);
		LOG_ARDUINO_PORT.println(F("ms"));
	}
}

#endif /* ifndef DEBUG_RP2040_PORT */

////////////////////////////////////////////////////////////////////////////////
#else /* ifndef ARDUINO */

////////////////////////////////////////
// Arduino program entry compatibility from native

extern "C" void setup();
extern "C" void loop();

int main()
{
	setup();
	for (;;) loop();
}

////////////////////////////////////////////////////////////////////////////////
#endif /* ifndef ARDUINO */
