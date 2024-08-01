/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: platform_reset.c
 * Description		: This file contains sources for platform
 *			  reset apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <reset.h>
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
	return power_on_reset;
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
	if(rsyn == power_on_reset)
		return;
	else
		plat_panic_handler();
}

