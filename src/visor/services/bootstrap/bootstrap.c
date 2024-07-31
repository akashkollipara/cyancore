/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: bootstrap.c
 * Description		: This file contains sources of bootstrap routine
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <arch.h>
#include <visor/platform.h>
#include <visor/bootstrap.h>


status_t bootstrap()
{
	reset_t resetSyndrome;

	arch_early_setup();

	platform_early_setup();

	arch_setup();

	platform_setup();

	resetSyndrome = platform_get_reset_syndrome();
	platform_reset_handler(resetSyndrome);

	platform_cpu_setup();

	return success;
}

#if CCSMP
status_t bootstrap_secondary()
{
	arch_early_setup();

	arch_setup2();

	platform_cpu_setup();
	return success;
}
#endif
