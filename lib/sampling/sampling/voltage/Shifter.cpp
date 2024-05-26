#include "sampling/voltage/Shifter.hpp"
#include <hardware/gpio.h>

namespace sampling::voltage {

void Shifter::init()
{ 
	uint32_t mask = (1 << s0Pin) | (1 << s1Pin);
	gpio_init_mask(mask);
	gpio_set_dir_out_masked(mask);
	gpio_put_masked(mask, 0);
}

void Shifter::set(Range range)
{
	uint32_t value = (range.s1Value() << s1Pin) | (range.s0Value() << s0Pin);
	uint32_t mask = (1 << s1Pin) | (1 << s0Pin);
	gpio_put_masked(mask, value);
}

Range Shifter::get()
{
	return Range((!!gpio_get(s0Pin)) | ((!!gpio_get(s1Pin)) << 1));
}

}
