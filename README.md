
# Project?

Microcontroller: [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html).
Framework: Arduino
Display: [4'' TFT SPI ST7796 480x320](http://www.lcdwiki.com/4.0inch_SPI_Module_ST7796).
Display driver: [ST7796(S)](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf) using [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library for Arduino.
Touch chipset: XPT2046



## Notes

### To-do

+ Use C++17 (or better). Sadly, the [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed) doesn't work with it, because `<Arduino.h>` has `abs` macro, and `chrono` (and other C++ headers) define them as templated functions which messes everything up. Solution would be to strategically put `#undef abs` in various places.


