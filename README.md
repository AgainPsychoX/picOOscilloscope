
# DIY Oscilloscope using Raspberry Pico and touch display

<!-- TODO: short description, images, features -->



## Hardware 

Microcontroller: [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)<br/>
Display: [4'' TFT SPI ST7796 480x320](http://www.lcdwiki.com/4.0inch_SPI_Module_ST7796)<br/>
Display driver: [ST7796(S)](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf)<br/>
Touch chipset: [XPT2046](https://grobotronics.com/images/datasheets/xpt2046-datasheet.pdf)<br/>

### Microcontroller pins usage

| Pin | GPIO | Notes                                                           |
|-----|------|-----------------------------------------------------------------|
|   1 |    0 | Reserved for UART0 TX                                          |
|   2 |    1 | Reserved for UART0 RX                                          |
|   3 |  GND | Ground                                                         |
|   4 |    2 | SPI0 SCK, used for display & touch (shared)                    |
|   5 |    3 | SPI0 TX (MOSI), used for display & touch (shared)              |
|   6 |    4 | SPI0 RX (MISO), used for display & touch (shared)              |
|   7 |    5 | Chip select for the display                                    |
|   8 |  GND | Ground                                                         |
|   9 |    6 | Reset for the display & touch (shared?)                        |
|  10 |    7 | Register select (Data/Command) for the display                 |
|  11 |    8 | Chip select for the touch                                      |
| ... |      |                                                                |
|  21 |   16 | S0 control for 74HC4052 for channel 2 (see below for details)  |
|  22 |   17 | S1 control for 74HC4052 for channel 2 (see below for details)  |
|  23 |  GND | Ground                                                         |
|  24 |   18 | S0 control for 74HC4052 for channel 1 (see below for details)  |
|  25 |   19 | S1 control for 74HC4052 for channel 1 (see below for details)  |
|     |      |                                                                |

### Analog front-end

Based on [_Scoppy_](https://github.com/fhdm-dev/scoppy) (other closed source Pico oscilloscope) [analog front-end examples](https://oscilloscope.fhdm.xyz/wiki/Analog-Front-End-Examples), including [FSCOPE-500K Rev 4e](https://oshwlab.com/fruitloop57/fscope-250k5-v2_copy_copy_copy_copy).

The front-end this project uses the more complex, dual channel with 4 voltage ranges, portection, 10X probe compatibility, input impedance of 1M||22pF, etc. [Simulation using CircuitJS](https://www.falstad.com/circuit/circuitjs.html?ctz=CQAgjCAMB0l3BWcMBsBmALAJhQdgWgJwAcYKCFI6ISCkNApgLRhgBQAhiE1pMSGl7dCKAUMIDoablmhIw8eFFiLcZXIUgYUO4nkLT6CxWwDmMvgK0ysGK3fqQ2YXFm4Is-LAlFMEuUW9RelxlESpuYOUkADUAewAbABcOUwY2AGN3TxAg7gwMLx8ocGZQxxV2LKY0YjtefiYCr0t6WSxmVm4YQgRCfs9BSF60UYgYOHYAd2yi3w8W-icZpsLc4tXFqDYVhdzLTf2ltgAnfLW8mrr1qOMTFdr6g8eb7YATbjJwQjcWQOK3G8GAAzDgAV2SbA+NQ8rxhbjAP1yICBoIhSTYACVuGhYYjfriEQUSvQMPQ0FISdEsTjrPjaXYwMTHCAyTI5FSYAhTp8AnDwnlyWgTNUXKIyPQmOEJSUMNBcJ1xlJ1IQApg+kzgpUdrzAgcxa9ljIARs8qNgjqeJZzcbRDajTwTb4DYLMrrwGBftLPSUwHJFcpJoHINN3TKWHyGtsVtKUJKBa0eVL-s6UyUfHAoQzvgTrHlASDwZDzH8c59xXzHJwQMRyUJ6UMQBIKYZkIoKsKyLjhbX-FhbFhylIQ+QXMMiPhcRA7pmVl8zW5XTNBK8V-adWu0KJawIt9GxK89q6zghiTu4-wd0ZeJmT8TWG4Lx63PQM7eaPe+U+DW1cPAeaeTz8E+Uakn+ThnE+jZ4G4jZGLOZbQdaQhOAA8lQrhiCEQiNuMZjgPecbgERMpVlkO5RkyQG+mUyidqOf4GKqBAIEqkxurG178KRwbwP0-ECYJ-TBmxIY6lRxFGMSoE0juK47qWLJsi2nJyDyO4PjWdYlKM-5nHkBq4ZWu7vgZxK4cyNBxhBrxyUIV7gDeNl5GsjZrNeCESVG+aJjMLlzFp+5mfUxQ-paLyXIShrhbCkV5sUDrCiFvj5glOoaURFG+YFUZydYRpPueliaQVxU+t+lY6hVoggdltXAchVYzBl9AaT6DoJo0AppTGabJlh2zof1jY1LB9b7nsEmsUSDg6kexSTcyc0+lGgFHBNPoGmtYUzGtO7TYFRoHZp23tWwAAyA2lp4jRfEYICggkADODCSIYF0DfOxDcVE9CPS9b1BcUFGhUtfmhZGALZbhPptS+H0HWsezuSU-2vSpTjQCAuAYAAEgAwmSHgAPpoDEaDEwAyuANA1uA0hgAAmkYzbIkz078EgCKMwioSBOAjMM-zCIAFrgEg9S5Mz+zYwIUsQLYcvSFgPO5BAdpS8r0h2MrYv9rT0gAKIADoABay7BICU0YDPIpT04OMiACCAB2bwAOIcEkDCGwkAC2xtPQggcoIHYDGycrse17Pv+4HwdPcQgdYBHUee97vsB0HgeEIHaCp276ex1nCcKIHGAF9HGdx09rBhynT2h5Hhcx5nYfh7X+eJ5XRdt7XFe1w3ufN1XxdhwPYBd2XJwAJIuwAbgwJzeycfeT-XEdz4vy9L2vE-h83HAJHEpiU1MACWSQZKba+l6HT0V7PC9Lyvfe4PHBdHyfZ+X9ft9h+-J6+dD7H1PhfK+N8a5gFLknJ6KcQHf3AX-KBpdc610-qAn+EC+79mTpAMOGDEG-0gVnXBcCB7wNdl-MBxCcENywB3YBVDMFIJIcnehXcHDG2TtwoBUheEtkDvgnhQiBH8KesIuBvDJGCIkbwlOoi87iMkQovh+clFdxUdIsRmj5HaI0YoqRhjZGSK5DowxZiDFyLwbwmc7Z8HWIkXIcxjjeC2PsYoQxbiw4ePgF4yRdj2z+Pcb4hxKiAmhLCfIiJoTgk+NCWwDACgrpfC3LdcU2M8aE0gCTMmFNqYOIbg4jGPC5FqKgKU2RJSpFlOqUUjRFSanFMpIUqxdTRHlLCc4zpQjul1IcYExQjSzE9LgQMyJjS3G1wmYU8ZoTJlzN8Qs+JSzZkrI8cs6ZCSgA) was created, for fun and exploration of the idea. Thanks to that, one can easily understand how the voltage is shifted. It uses two [74HC4052](https://www.sparkfun.com/datasheets/Components/General/74HC4052.pdf) dual 4-channel analog switch/(de)multiplexer; I even [recreated the 74HC4052 inside the simulation](https://github.com/pfalstad/circuitjs1/issues/76#issuecomment-2019215612) too (custom compontent). Basically, the compontent takes 2 digital inptus from the microproessing, which allow for selecting pair of resistors: for voltage division and shift.

| Range ID | Min voltage | Max voltage | S1 | S0 | 
|:--------:|:-----------:|:-----------:|:--:|:--:|
|        0 | -5.872 V    | 5.917 V     | 0  | 0  |
|        1 | -2.152 V    | 2.497 V     | 0  | 1  |
|        2 | -1.120 V    | 0.949 V     | 1  | 0  |
|        3 | -0.404 V    | 0.585 V     | 1  | 1  |

<!-- TODO: consider adding resistor values to the table above -->

### Display hardware fix

[An issue was reported](https://github.com/Bodmer/TFT_eSPI/issues/3261) to the library for controlling the display (and touch) for weird behaviour of display and touch SPI bus sharing misbehaving, resulting in visual gliches affecting last draw instructions right before switching SPI to talk with the touch controller. It turned out, the display was affected by a hardware bug. Explanation, origin and workaround (hardware modification) for [the bug are described in TFT_eSPI issues by Bodmer](https://github.com/Bodmer/TFT_eSPI/discussions/898) (maintainer of the library). The proposed modification was applied as workaround in this project: shorting one of diodes on the display board. 



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

+ Consider moving graphing stuff to separate namespace
+ Prefered voltage lines for graphs (symetrical by 0V):
	+ Range 0: 0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00 V
	+ Range 1: 0.00, 0.50, 1.00, 1.50, 2.00, 2.50 V
	+ Range 2: 0.00, 0.25, 0.50, 0.75, 1.00, 1.25 V
	+ Range 3: 0.00, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60 V
+ Consider using lower-level graphical functions to draw graph parts
+ Try fix touch issues
	+ limit button hitbox?
	+ detect jumping/deformations, maybe Z jumps around?
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
		+ Some modes are parameterized 
			+ Level: (V +/-)
			+ Width: (time +/-)
		+ Repeat: ON/OFF/1 (1 == "Next", for next single capture when not repeating)
		+ Hold off (time +/-)
	+ Sample rate? Lower = less detail, longer data
		+ Technical
		+ Can be derived from time-base somewhat
		+ With enough memory seems like more detail is more important anyway, especially because max sample rate isn't that high anyway.
		+ On other hand, maybe we can make ADC work with lower resolution/accuracy, but more speed (tradeoff)
	+ ...
	+ Interesting read/watch: 
		+ https://www.tek.com/en/documents/primer/oscilloscope-systems-and-controls
		+ https://www.youtube.com/watch?v=1b3ivEZo7hw&list=PL2XuMA5AwNUznkBE46tcZAF3p5Edxgm-z&pp=iAQB
+ User Interface outputs:
	+ Voltages (max/min/avg)
	+ Frequency, Duty
	+ ...
+ Watch https://www.youtube.com/watch?v=rDDiPzJpI18 , try to understand "unofficial" speeds
+ Multi-language support, most likely configured on compile time
	+ Polish - including speical characters outside ASCII (custom fonts or fixing by overlapping characters/pixel drawings)
	+ English
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
+ Unit tests :^)
	+ See https://piolabs.com/blog/insights/unit-testing-part-1.html
	+ See https://docs.platformio.org/en/latest/advanced/unit-testing/index.html
	+ Consider refactoring some code into [private libraries](https://docs.platformio.org/en/latest/projectconf/sections/platformio/options/directory/lib_dir.html). 


