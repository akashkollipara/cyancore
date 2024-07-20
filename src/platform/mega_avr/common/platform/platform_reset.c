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
#include <platform.h>
#include <driver/watchdog.h>
#include <visor/platform.h>

extern uint8_t reset_syndrome;

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
	sysdbg5("Reset Syndrome = %u\n", reset_syndrome);
	if(!reset_syndrome)
		return not_reset;
	else if(reset_syndrome & 1)		/* Power on Reset */
		return power_on_reset;
	else if(reset_syndrome & 2)	/* External Reset */
		return external_reset;
	else if(reset_syndrome & 4)	/* Brown out Reset */
		return brownout_reset;
	else if(reset_syndrome & 8)	/* WDog Reset */
		return wdog_reset;
	else
		return inval_reset;
}


TODO(This Function needs to be replaced with brownout detection driver api)
_WEAK void brownout_reset_handler()
{
	plat_panic_handler();
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
	if((rsyn == power_on_reset) || (rsyn == external_reset))
		return;
	else if(rsyn ==  brownout_reset)
		brownout_reset_handler();
	else if(rsyn ==  wdog_reset)
		wdog_reset_handler();
	else
		plat_panic_handler();
}

