
## Testing on Raspberry Pi Pico using PlatformIO with Arduino-Pico core and ThrowTheSwitch Unity testing framework

### `platformio.ini`
```ini
; ... 
; Base [env] should have all the base flags and stuff, [env:pico] will inherit;
; don't forget about upload tool & port, and monitor.

[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower

test_framework = unity
test_build_src = yes
test_filter =
	common/*
	embedded/*
```

If `test_build_src = yes`, make sure guard your `main`/`setup`/`loop` with `#ifndef PIO_UNIT_TESTING`, as most likely you will have the testing code should implement those.

+ [General PlatformIO docs about unit testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html#unit-testing)
+ [PlatformIO docs about options in the `[env*]` sections](https://docs.platformio.org/en/stable/projectconf/sections/env/options/build/build_flags.html)



#### Test hierarchy

Have good read about PlatformIO [test hierarchy](https://docs.platformio.org/en/stable/advanced/unit-testing/structure/hierarchy.html). I use `test/common/*` for tests that could run on both native (your PC) and target board; then `test/embedded/*` for test strictly for target board, including `test/embedded/interactive/*` for those which will require external user input to perform and hopefully succeed.



### Unity (testing framework)

You can use `unity_config.h` file to put extra configuration. Care: it should be C-style header, as it will be included by Unity build process which most likely will use C. You might want to guard for `extern "C"` where necessary. 

If you have some functions that need to be accessible for Unity (like me if you see sections below), in some source file (I use `unity_config.cpp`) you can put the implementation, but make sure to use `extern "C"` again for those functions.

_See [`unity_config.h`](./unity_config.h) and [`unity_config.cpp`](./unity_config.cpp) for my implementation._



### Arduino-Pico

Unity Testing framework uses standard output by default.

#### Option A) Define `DEBUG_RP2040_PORT`

When using the [Earle Philhower's Arduino-Pico core](https://github.com/earlephilhower/arduino-pico) for Raspberry Pi Pico (RP2040) if you want to use `stdout` things you have to specify custom Arduino `Print`-compatible target `DEBUG_RP2040_PORT` (usually `Serial`, or `Serial1/2`). Other method is using as Arduino has `Print::printf` extension which is used by the logging library by default. See [this comment](https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673) and [this comment](https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109) at Arduino-Pico issue about `printf`. Also you can visit [standard output related `_write` implementation](https://github.com/earlephilhower/arduino-pico/blob/11814823ed4ace4224fe9ea03686de53387fb193/cores/rp2040/posix.cpp#L36-L48) using `DEBUG_RP2040_PORT` in the Arduino-Pico sources.

You need to define the `DEBUG_RP2040_PORT` in the whole environment for whole project (easy, but then your normal logging also should go via standard output ways) OR figure out to do that only when running tests - which most likely (I didn't research this way enough) requires to use extra scripts.

#### Option B) Configure Unity to use Arduino Serial

In your `unity_config.h`, you can define [`UNITY_OUTPUT_CHAR(c)`, `UNITY_OUTPUT_FLUSH()`, `UNITY_OUTPUT_START()` and `UNITY_OUTPUT_COMPLETE()`](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityConfigurationGuide.md#unity_output_chara) which Unity will use to print stuff. With right macros it will be used only when building for target board, and will be omitted for native build.

#### Delay before testing on target board

Also, `UNITY_OUTPUT_START()` is good place to add extra delay. I use 2500 ms. 

_See [`unity_config.h`](./unity_config.h) and [`unity_config.cpp`](./unity_config.cpp) for my implementation._



### Showing execution time (or using clock at all)

Unity can be configured with `UNITY_INCLUDE_EXEC_TIME` to printing execution time of each test. Problem is, that Pico doesn't implement `_clock` for standard library `clock` to be working which the Unity needs by default. Fortunately, you can define `UNITY_CLOCK_MS` to provide your function for returning time in milliseconds.

_See [`unity_config.h`](./unity_config.h) and [`unity_config.cpp`](./unity_config.cpp) for my implementation._



### Writing the tests

Don't forget to add the `setUp` and `tearDown` (even if empty):
```c++
void setUp(void) {}

void tearDown(void) {}
```

If using Arduino-Pico you should also use `setup` and `loop`, including in shared tests. Then, since shared test run both native (local PC) and target board, we need `main` for native code that will run the `setup` and `loop`. Also, the Arduino might expect C-style while linking those, so use `extern "C"`:

_See [`unity_config.h`](./unity_config.h) and [`unity_config.cpp`](./unity_config.cpp) for my implementation._

The Arduino-Pico framework will add `main` when building for Pico (and do some setup for serial ports and so on).

I put the test in the `setup`:
```c++
extern "C" void setup()
{
	UNITY_BEGIN();
	RUN_TEST(test_basic);
	UNITY_END();
}

extern "C" void loop() {}
```

If you put it in `loop` it still should work - the test runner will just get first result from the logs and proceed with other tests.

#### Unresponsive Pico issue

If you ended up with unresponsive Pico, it might be because you implemented `main` somewhere by accident, and Arduino-Pico own `main` (with all the important setup) was not executed. You might want to manually write valid UF2 file (even simple blink) to your Pico.



### Running the tests

Aside from PlatformIO general "Test" button, you can use command line for more options.

`pio test --help`
```
Usage: pio test [OPTIONS]

Options:
  -e, --environment TEXT
  -f, --filter PATTERN         Filter tests by a pattern
  -i, --ignore PATTERN         Ignore tests by a pattern
  --upload-port TEXT
  --test-port TEXT
  -d, --project-dir DIRECTORY
  -c, --project-conf FILE
  --without-building
  --without-uploading
  --without-testing
  --no-reset
  --monitor-rts INTEGER RANGE  Set initial RTS line state for Serial Monitor
                               [0<=x<=1]
  --monitor-dtr INTEGER RANGE  Set initial DTR line state for Serial Monitor
                               [0<=x<=1]
  -a, --program-arg TEXT       A program argument (multiple are allowed)    
  --list-tests
  --json-output
  --json-output-path PATH
  --junit-output-path PATH
  -v, --verbose                Increase verbosity level, maximum is 3 levels
                               (-vvv), see docs for details
  -h, --help                   Show this message and exit.
```

Aside from the button I usually use `pio test -e pico -v --filter embedded/interactive/*` (or even more specific). At least single `-v` is required if you want see the log from the target device and the execution times for given tests.
