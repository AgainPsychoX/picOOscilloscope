
# Project?



## Hardware 

Microcontroller: [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html).
Display: [4'' TFT SPI ST7796 480x320](http://www.lcdwiki.com/4.0inch_SPI_Module_ST7796).
Display driver: [ST7796(S)](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf)
Touch chipset: [XPT2046](https://grobotronics.com/images/datasheets/xpt2046-datasheet.pdf)

### Microcontroller pins usage

| Pin | GPIO | Notes                                                           |
|-----|------|-----------------------------------------------------------------|
|    1 |    0 | Reserved for UART0 TX                                          |
|    2 |    1 | Reserved for UART0 RX                                          |
|    3 |  GND | Ground                                                         |
|    4 |    2 | SPI0 SCK, used for display & touch (shared)                    |
|    5 |    3 | SPI0 TX (MOSI), used for display & touch (shared)              |
|    6 |    4 | SPI0 RX (MISO), used for display & touch (shared)              |
|    7 |    5 | Chip select for the display                                    |
|    8 |  GND | Ground                                                         |
|    9 |    6 | Reset for the display & touch (shared?)                        |
|   10 |    7 | Register select (Data/Command) for the display                 |
|   11 |    8 | Chip select for the touch                                      |
|      |      |                                                                |



## Software

Environment: PlatformIO inside VS Code
Framework: Arduino <small>(I dislike it a bit, but required for the library)</small>
Library [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) used to support the display, with the touch support built-in.



## Notes

### To-do

+ Consider integrating new touch related code to into TFT library (refactor & create pull request): 
	+ Could reduce memory usage tiny bit of us (no unused built-in code & calibration)
	+ Would require more testing, at least more rotations etc., but also other devices - which we don't have any at the moment.
	+ By the way, library could use more examples with touch along the way, like saving calibration settings to EEPROM, hand-writing with smart lines, maybe simple paint etc.
	+ Macros could be used to provide options (dynamic deadband error limit and other valid-touch filtering parameters).
+ Use C++17 (or better). Sadly, the [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed) doesn't work with it, because `<Arduino.h>` has `abs` macro, and `chrono` (and other C++ headers) define them as templated functions which messes everything up. Solution would be to strategically put `#undef abs` in various places.


