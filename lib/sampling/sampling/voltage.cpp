#include "sampling/voltage.hpp"

namespace sampling::voltage
{

Shifter shifter[2] {
	{ 18, 19 }, // First channel uses GPIO 18 & 19
	{ 16, 17 }, // Second channel uses GPIO 16 & 17
};

void init()
{
	for (auto& s : shifter) 
		s.init();
}

}
