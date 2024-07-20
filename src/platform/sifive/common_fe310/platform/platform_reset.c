/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: platform_reset.c
 * Description		: This file contains sources for platform
 *			  reset apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <assert.h>
#include <reset.h>
#include <resource.h>
#include <arch.h>
#include <aon.h>
#include <driver/watchdog.h>
#include <platform.h>
#include <visor/platform.h>

/**
 * platform_get_reset_syndrome - returns the cause of reset
 *
 * @brief This function returns the information related to
 * the reset sources. 
 *
 * @return reset_cause: This function returs the reset cause
 */
reset_t platform_get_reset_syndrome()
{
	vret_t vres;
	const module_t *dp;
	uint32_t pmucause_val;
	aon_port_t aonport;

	arch_visor_call(fetch_dp, aon, 0, 0, &vres);
	assert(vres.status == success);

	dp = (module_t *)vres.p;
	aonport.baddr = dp->baddr;

	aon_pmucause(&aonport, &pmucause_val);
	sysdbg5("Reset Syndrome = %u\n", pmucause_val);
	if(!(pmucause_val & 0x1))			/* Check PMU cause for reset */
		return not_reset;

	if(pmucause_val & (1 << 8))		/* Power on Reset */
		return power_on_reset;
	else if(pmucause_val & (1 << 9))	/* External Reset */
		return external_reset;
	else if(pmucause_val & (1 << 10))	/* Watchdog Reset */
		return wdog_reset;
	else
		return inval_reset;
}


/**
 * platform_reset_handler - handles the reset conditions
 *
 * @brief This function is responsible to handle the reset
 * sources like watchdog, brownout, external reset, etc.
 *
 * @param[in] rsyn: Reset syndrome
 *
 * @return void
 */
void platform_reset_handler(reset_t rsyn)
{
	if(rsyn == not_reset)
		return;
	if(rsyn == power_on_reset)
		return;
	else if(rsyn == external_reset)
		return;
	else if(rsyn == wdog_reset)
		wdog_reset_handler();
	else
		plat_panic_handler();
}

