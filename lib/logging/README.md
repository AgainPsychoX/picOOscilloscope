
# My logging library

Features:
+ 5 log levels (Error, Warning, Info, Debug, Trace).
+ Release optimization (disabling certain logs freeing program memory).
+ Per tag (module/component) control of log level in compile-time.

Inspired by [certain discussion/brainstorming about logging](https://github.com/earlephilhower/arduino-pico/discussions/2066).

Supports:
+ Arduino (printing to specified `LOG_ARDUINO_PORT`, default: `Serial`) _(untested)_
+ Arduino-Pico (see Arduino; also )
+ Pico C SDK _(untested)_
+ ESP-IDF (including option to build on top of use original ESP-IDF logging solution) _(untested)_
+ AVR _(untested)_





## How to use

### Basic

Create header file in your project files, like `logging.hpp` and `#include <logging_base.hpp>` from there. Then, define select default logging level as well as levels for next tags (modules/components) of your project.

Example:
```c++
#include <logging_base.hpp>

USE_LOG_LEVEL_DEFAULT(LOG_LEVEL_INFO);

USE_LOG_LEVEL("Touch", LOG_LEVEL_INFO);
USE_LOG_LEVEL("Sampling", LOG_LEVEL_INFO);
USE_LOG_LEVEL("Graphing", LOG_LEVEL_DEBUG);
```

Then, you can include this file from your project files and use `LOG_INFO(tag, ...)` and similar to log. You can also use `CHECK_LOG_LEVEL(tag, level)` as condition for whenever log should be logged, which is useful

Note, that you (or the framework) are responsible for initialization of the logging interface. 


#### Arduino

`Serial` (or other custom Arduino `Print`-compatible target set via `LOG_ARDUINO_PORT`) must be properly initialized. Usually, you need to do simple `Serial.begin(115200);` in your `setup()` function.

Some Arduino cores (implementations of Arduino standard), like [MCUdude's cores](https://github.com/MCUdude/MCUdude_corefiles), [Earle Philhower's cores](https://github.com/earlephilhower/ArduinoCore-API) and [ESP8266 Arduino core](https://github.com/esp8266/Arduino) have `printf` implemented on the `Print` interface/class.

However, base/original/old Arduino core might have no `printf` support, then you need to use some library to implement `printf`, like [Arduino LibPrintf](https://github.com/embeddedartistry/arduino-printf/tree/master) (make sure to read the library docs). If some library provides support via custom 


#### Arduino-Pico

When using the [Earle Philhower's Arduino-Pico core](https://github.com/earlephilhower/arduino-pico) for Raspberry Pi Pico (RP2040) if you want to use `stdout` things you have to specify custom Arduino `Print`-compatible target `DEBUG_RP2040_PORT` (usually `Serial`, or `Serial1/2`)[^1]. However it's not necessary, as Arduino has `Print::printf` extension which is used by the logging library by default.

[^1]: See [this comment](https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673) and [this comment](https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109) at Arduino-Pico issue about `printf`. Also you can visit [standard output related `_write` implementation](https://github.com/earlephilhower/arduino-pico/blob/11814823ed4ace4224fe9ea03686de53387fb193/cores/rp2040/posix.cpp#L36-L48) using `DEBUG_RP2040_PORT` in the Arduino-Pico sources.


#### ESP-IDF

No setup is needed for `stdout` based solution, but you can define `LOG_USE_ESPIDF` to build logs on top of ESP-IDF specific logging solution (`ESP_LOGx` functions). In such case you might want to use `esp_log_level_set` to set the log level to the lowest most detailed level that you want to log, like `esp_log_level_set("*", LOG_LEVEL_DEBUG)` in your `app_main()` function. 

#### AVR

Depending on platform & desired output channel, you need to prepare `stdout` correctly and enable `printf` using defines/compiler flags:
+ light version: `-Wl,-u,vfprintf -lprintf_min` (~1.3KB @ ATtiny1634) 
+ full version: `-Wl,-u,vfprintf -lprintf_flt -lm` (with floating point) (~3KB @ ATtiny1634)
+ see your [AVR LIBC docs](https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaa3b98c0d17b35642c0f3e4649092b9f1) for details.

Helpful article: [AVR and printf by Jason Bauer](https://efundies.com/avr-and-printf/) or [forum post on AVRFreaks](https://www.avrfreaks.net/s/topic/a5C3l000000UHT5EAO/t080941).

#### Custom output

You can provide custom output function using `LOG_PRINTF` macro, which need to be `printf` like function, taking variadic arguments. By default it's platform specific: on Arduino it's `Serial.print`-based, and other (e.g. ESP32, Pico, AVR) it's `stdout` based: regular `printf` and `puts`.



### Inside library

If used inside library, the library could use own header itself, maybe utilizing macros in order to give the library user the some control. 

<!-- TODO: Also, if you include if from header files, you might want to define `LOG_???` before including `logging_base.hpp` to avoid variables names collisions if the `logging_base` is included multiple times (like, by two libraries and/or project). -->

Example:
```c++
#include <logging_base.hpp>

#ifndef MY_LIB_LOG_DEFAULT
#define MY_LIB_LOG_DEFAULT LOG_LEVEL_NONE
#endif
USE_LOG_LEVEL_DEFAULT(MY_LIB_LOG_DEFAULT);

#ifndef MY_LIB_LOG_FOO_LEVEL 
#define MY_LIB_LOG_FOO_LEVEL MY_LIB_LOG_DEFAULT
#endif
USE_LOG_LEVEL("MyLib-Foo", MY_LIB_LOG_FOO_LEVEL);

#ifndef MY_LIB_LOG_BAR_LEVEL 
#define MY_LIB_LOG_BAR_LEVEL MY_LIB_LOG_DEFAULT
#endif
USE_LOG_LEVEL("MyLib-Bar", MY_LIB_LOG_BAR_LEVEL);
```

If you want user to be able to have all logging configuration in single place, there is `logging_somewhere.hpp` header available that tries to look around for your project `logging.hpp`. It should work in PlatformIO based environments, and you should have some `#ifndef LOG_INFO` to guard for a case when the project doesn't have proper `logging.hpp` file (or it couldn't be found).



## Notes

+ It seems like ESP8266 Arduino core has better optimized logging for program size, than the ESP8266 DevKitC (ESP-IDF?) framework, thanks to `PROGMEM`/`PSTR` macros and `vsnprintf_P` and alike functions. Tho, some modern versions of LIBC support reading from program memory directly; testing would be required for specific cases I guess.

### To-do

+ Separate as standalone project
+ Examples
+ Support base Arduino (no `printf` extension)
+ Testing (at least in simulators)
+ Configurable log format
+ Colors
+ Timestamps
+ Memory dumping (HEX/BIN/ASCII)
+ etc; and [more](https://github.com/earlephilhower/arduino-pico/discussions/2066#discussioncomment-9399913)
