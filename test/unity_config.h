#pragma once
#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// ThrowTheSwitch Unity testing framework configuration
// See https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityConfigurationGuide.md
// and its repository for details, descriptions and more information.
////////////////////////////////////////////////////////////////////////////////

// Enables printing execution time of each test when executed.
#define UNITY_INCLUDE_EXEC_TIME

////////////////////////////////////////////////////////////////////////////////
#ifdef ARDUINO

////////////////////////////////////////
// Clock, time measurement fixes

// Specified function to be used to get time in milliseconds.
// Default is standard library `clock` but Pico doesn't support it very well.
// TODO: add issue to Pico C SDK about `clock` support, should be trivial.
#define UNITY_CLOCK_MS unityClock

#ifdef __cplusplus
extern "C" {
#endif

/// Returns current time in milliseconds.
unsigned int unityClock();

#ifdef __cplusplus
}
#endif

////////////////////////////////////////
// Fix Unity logging for Arduino-Pico

// If `DEBUG_RP2040_PORT` is defined, the standard output is properly redirected
// to the serial port and everything is fine. If it's not, we need use Arduino
// some serial port to output the log. See links below for details:
// + https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673
// + https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109
// + https://github.com/earlephilhower/arduino-pico/blob/11814823ed4ace4224fe9ea03686de53387fb193/cores/rp2040/posix.cpp#L36-L48
#ifndef DEBUG_RP2040_PORT
#	define UNITY_OUTPUT_CHAR(c) unityOutputChar(c)
#	define UNITY_OUTPUT_FLUSH() unityOutputFlush()
#	define UNITY_OUTPUT_START() unityOutputStart()
//#	define UNITY_OUTPUT_COMPLETE() unityOutputComplete() // not used
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUG_RP2040_PORT
void unityOutputChar(char c);
void unityOutputFlush();
void unityOutputStart();
//void unityOutputComplete(); // not used
#endif

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* ifndef ARDUINO */

// Define `setup` & `loop` for Arduino program entry compatibility from native,
// and to avoid making `extern "C"` being needed to be added in common tests.

#ifdef __cplusplus
extern "C" {
#endif

void setup();
void loop();

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
// #endif /* ifndef ARDUINO */
