#include <stdint.h>
#include <status.h>
#include <mega_avr_platform.h>

void platform_early_setup()
{
	platform_clk_reset();
	platform_dp_setup();
	return;
}

void platform_setup()
{
	return;
}

void platform_cpu_setup()
{
	return;
}

