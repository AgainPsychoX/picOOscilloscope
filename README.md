
# Project?



## Hardware 

Microcontroller: [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)<br/>
Display: [4'' TFT SPI ST7796 480x320](http://www.lcdwiki.com/4.0inch_SPI_Module_ST7796)<br/>
Display driver: [ST7796(S)](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf)<br/>
Touch chipset: [XPT2046](https://grobotronics.com/images/datasheets/xpt2046-datasheet.pdf)<br/>

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

Environment: [PlatformIO](https://platformio.org/) inside VS Code<br/>

Framework: Arduino ([`earlephilhower`](https://arduino-pico.readthedocs.io/en/latest/) version)
+ Two main versions of Arduino core for Raspberry Pico exist: [One from PlatformIO team](https://github.com/platformio/platform-raspberrypi), using [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed); and other, [Arduino-Pico](https://arduino-pico.readthedocs.io/en/latest/) aka [`earlephilhower` version](https://github.com/earlephilhower/arduino-pico).
+ The used version seems to support more stuff (like [EEPROM library](https://arduino-pico.readthedocs.io/en/latest/eeprom.html)), and [also allows using RP2040 C SDK](https://community.platformio.org/t/status-of-baremetal-pico-sdk-as-a-platformio-framework/35918/2).
+ I dislike it a bit, but required for the display library.

Library [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) was used to support the display, with the touch support built-in.<br/>



## Notes

### ADC

Without overclocking, ADC uses USB PLL clock with is 48MHz. The ADC apparently needs 96 clock cycles for sample conversion. See RP2040 datasheet at chapter 4.9. for ADC details.

Sample rate? Depending on frequency: 
+ fastest: 48MHz / 96 = 500 kS/s (without overclocking)
+ slowest: 48Mhz / 65536 = 732.421875 S/s
+ slowest rounded: 48Mhz / 48000 = 1000 S/s

Time between samples?
+ fastest 1/500KHz = 2us (without overclocking)
+ slowest: 1/732.421875Hz = 0.00136533333s (a uneven bit over 1.3ms)
+ slowest rounded: 1/1000Hz = 1ms

Total recording time? Assuming 40'000 samples:
+ fastest: 40'000 / 500KHz = 0.08s = 80ms
+ slowest: 40'000 / 1000Hz = 40s

### To-do

1. Przyciski
2. Default wartości w `sampling` oraz ładowanie i zapisywanie na EEPROM
3. Graf

+ User Interface inputs:
	+ Channels: `CH1`, `CH2` (single graph), `CH1+2=` (separate graphs, stacked on top of each other), `CH1+2-` (both on single graph)
	+ Time-base (multiple levels, +/- buttons) aka "horizontal scale"
	+ Horizontal position
	+ Voltage-base aka "vertical scale"
	+ No vertical position - channels selection takes care of it if we assume only up to 2 channels
	+ Trigger: 
		+ Channel: CH1, CH2 or both
		+ Mode:
			+ None/Free running
			+ Positive/Negative Edge (voltage rises/falls through set level within set time frame)
			+ Positive/Negative Pulse (pulse up/down to at least set level of at least set width)
			+ Positive/Negative Glitch (pulse up/down to at least set level of less than set width)
			+ Logic AND/NAND/OR/NOR/XOR ?
		+ Level: (V +/-)
		+ Width: (time +/-)
		+ Repeat: ON/OFF/1 (1 == "Next", for next single capture when not repeating)
	+ Sample rate? Lower = less detail, longer data
		+ Technical
		+ Can be derived from time-base somewhat
		+ With enough memory seems like more detail is more important anyway, especially because max sample rate isn't that high anyway.
		+ On other hand, maybe we can make ADC work with lower resolution/accuracy, but more speed (tradeoff)
	+ ...
	+ Interesting read: https://www.tek.com/en/documents/primer/oscilloscope-systems-and-controls
+ User Interface outputs:
	+ Voltages (max/min/avg)
	+ Frequency, Duty
	+ ...
+ Watch https://www.youtube.com/watch?v=rDDiPzJpI18 , try to understand "unofficial" speeds
+ Overclocking? 
	+ https://forums.raspberrypi.com/viewtopic.php?t=340691
	+ https://www.youtube.com/watch?v=G2BuoFNLoDM
	+ `check_sys_clock_khz` https://github.com/raspberrypi/pico-sdk/blob/6a7db34ff63345a7badec79ebea3aaef1712f374/src/rp2_common/pico_stdlib/stdlib.c#L88
	+ `set_sys_clock_pll` https://github.com/raspberrypi/pico-sdk/blob/6a7db34ff63345a7badec79ebea3aaef1712f374/src/rp2_common/pico_stdlib/stdlib.c#L48
	+ `clock_configure` https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_clocks/clocks.c#L48
+ Generating reference signal
	+ See datasheet 2.15.6.3. "Configuring a GPIO output clock"
	+ Or use some timers based generator
	+ Maybe PIO generator?
+ Read https://raspberrypi.stackexchange.com/questions/143394/improving-sampling-frequency-of-raspberry-pi-picos-adc-pin-to-sample-at-frequen
+ If ADC2 (free pin) & ADC3 (required board modding) were to be used, they would also share the SAR ADC unit so also the sample rate (max 500kS/s divided by 4; at least by official clock speeds)
+ If external ADCs were to be used, they would require separate mini-drivers to be written; Then, they should be aggregated into single controller. User would have control over sampling frequency (and other settings like clock) per ADC-unit (which can be shared by multiple channels). Changing the frequency or desired capture time window might require changes across all other ADC-units, to make it all fit with given memory.
	+ Let's say we have 2 CH ADC (Pico) with max 500kS/s (split between channels) and 1 CH external ADC with max 1 MS/s, and total 100kB memory for sampling data. Memory should be split between ADCs to have them record the same window of time. When changing sampling rate, other ADC sampling rate should change proportionally OR the time windows need to shorten. Or simply show error unless user fix settings to have it fit.
+ Consider external fast ADC 
	+ https://www.digikey.pl/en/products/filter/data-acquisition/analog-to-digital-converters-adc/700?s=N4IgjCBcoMwCwA4qgMZQGYEMA2BnApgDQgD2UA2uAGxhgDsdIxcVVAnAAxVPgcwcAmAKw84AtlRgRi4uGA5Ji9cUJg96dWTwEC6QodJADEcIQNEI6bYTyEMwbNqJgx9cbWbgT1mtme0cDpLaiHZOMqwM3DJ0XPLabHAuasR0rkLRIGl09DzW4hzqfPLmSkkccIxl-Krq5ULu1Rz6dWxgQorgHZqdBgIV4dT6-OoIcFzmALrEAA4ALlAgAMpzAE4AlgB2AOYgAL57xNzQIOsAJosAtPKG84s8cwCeM-iLmLhoB0A
	+ https://www.mouser.pl/c/semiconductors/data-converter-ics/analog-to-digital-converters-adc/?sampling%20rate=10%20MS%2Fs~~10.4%20GS%2Fs&rp=semiconductors%2Fdata-converter-ics%2Fanalog-to-digital-converters-adc%7C~Sampling%20Rate&sort=sampling%20rate&qty=1
	+ https://www.mouser.pl/c/semiconductors/data-converter-ics/analog-to-digital-converters-adc/?package%20%2F%20case=QSOP-20~~SSOP-36%7C~TSSOP-20~~TSSOP-48&sampling%20rate=10%20MS%2Fs~~10.4%20GS%2Fs&sort=sampling%20rate&rp=semiconductors%2Fdata-converter-ics%2Fanalog-to-digital-converters-adc%7C~Sampling%20Rate%7C~Package%20%2F%20Case&qty=1
		+ 90zł, 200 MSps, 8 bit, 1 CH https://www.mouser.pl/ProductDetail/Texas-Instruments/ADC08200CIMT-NOPB?qs=7X5t%252BdzoRHAf23TNwec6Xw%3D%3D
		+ 45zł, 100 MSps, 8 bit, 1 CH https://www.mouser.pl/ProductDetail/Texas-Instruments/ADC08100CIMTC-NOPB?qs=7X5t%252BdzoRHDOkS2%252B9%2FZhUQ%3D%3D	
	+ https://niconiconi.neocities.org/posts/list-of-mcu-with-fast-adc/
	+ TLC5510 ? 
+ Consider support for logical channels
+ Consider integrating new touch related code to into TFT library (refactor & create pull request): 
	+ Could reduce memory usage tiny bit of us (no unused built-in code & calibration)
	+ Would require more testing, at least more rotations etc., but also other devices - which we don't have any at the moment.
	+ By the way, library could use more examples with touch along the way, like saving calibration settings to EEPROM, hand-writing with smart lines, maybe simple paint etc.
	+ Macros could be used to provide options (dynamic deadband error limit and other valid-touch filtering parameters).
+ Use C++17 (or better). Sadly, the [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed) doesn't work with it, because `<Arduino.h>` has `abs` macro, and `chrono` (and other C++ headers) define them as templated functions which messes everything up. Solution would be to strategically put `#undef abs` in various places.


