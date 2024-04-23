/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: onboardled.c
 * Description		: This file contains sources of onboardled driver
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <stdlib.h>
#include <resource.h>
#include <visor_call.h>
#include <lock/lock.h>
#include <driver.h>
#include <driver/onboardled.h>
#include <hal/gpio.h>

static gpio_port_t *obledPort;
static swdev_t *obled_sp;
static lock_t obledlock;

/**
 * @brief Toggle the state of the onboard LED.
 *
 * This function toggles the state of the onboard LED by each GPIO pin
 * It also works accordinlg with locks to protect the state
 *
 * @return status_t: Status of the operation
 */

status_t onboardled_toggle(void)
{
	status_t ret = success;
	lock_acquire(&obledlock);
	if(!obled_sp)
	{
		ret = error_driver_init_failed;
		goto exit;
	}
	for(uint8_t i = 0; i < obled_sp->pmux->npins; i++)
		ret |= gpio_pin_toggle(&obledPort[i]);
exit:
	lock_release(&obledlock);
	return ret;
}

/**
 * @brief Turning on the onboard LED
 *
 * This function turns on the onboard LED by setting each of the GPIO pin to high
 *
 * @return status_t: Status of the operation
 */

status_t onboardled_on(void)
{
	status_t ret = success;
	lock_acquire(&obledlock);
	if(!obled_sp)
	{
		ret = error_driver_init_failed;
		goto exit;
	}
	for(uint8_t i = 0; i < obled_sp->pmux->npins; i++)
		ret |= gpio_pin_set(&obledPort[i]);
exit:
	lock_release(&obledlock);
	return ret;
}

/**
 * @brief Turning off the onboard LED
 *
 * This function turns on the onboard LED by setting each of the GPIO pin to low (clearing)
 *
 * @return status_t: Status of the operation
 */

status_t onboardled_off(void)
{
	status_t ret = success;
	lock_acquire(&obledlock);
	if(!obled_sp)
	{
		ret = error_driver_init_failed;
		goto exit;
	}
	for(uint8_t i = 0; i < obled_sp->pmux->npins; i++)
		ret |= gpio_pin_clear(&obledPort[i]);
exit:
	lock_release(&obledlock);
	return ret;
}

/**
 * @brief Initializes the onboard LED driver
 *
 * This function initializes the onboard LED driver by allocating GPIO ports for the LED.
 *
 * @return status_t: Status of the initialization
 */

static status_t onboardled_setup(void)
{
	vret_t vres;
	status_t ret;

	lock_acquire(&obledlock);
	arch_visor_call(fetch_sp, onboard_led, 0, 0, &vres);
	if(vres.status != success)
	{
		sysdbg3("%p - sp node could not be found!\n", onboard_led);
		ret = vres.status;
		goto exit;
	}
	obled_sp = (swdev_t *) vres.p;
	ret = vres.status;

	obledPort = (gpio_port_t *)malloc(sizeof(gpio_port_t) *
			obled_sp->pmux->npins);

	if(!obledPort)
	{
		ret = error_memory_low;
		goto exit;
	}

	for(uint8_t i = 0; i < obled_sp->pmux->npins; i++)
	{
		ret |= gpio_pin_alloc(&obledPort[i], obled_sp->pmux->port,
				obled_sp->pmux->pins[i]);
		ret |= gpio_pin_mode(&obledPort[i], out);
	}
exit:
	lock_release(&obledlock);
	return ret;
}

/**
 * @brief Cleanup and exit the onboard LED driver.
 *
 * This function frees allocated GPIO ports and resources associated with the
 * onboard LED driver.
 *
 * @return status_t: Status of the cleanup and exit
 */

static status_t onboardled_exit(void)
{
	status_t ret = success;
	lock_acquire(&obledlock);
	if(!obled_sp)
	{
		ret = error_driver_init_failed;
		goto exit;
	}
	for(uint8_t i = 0; i < obled_sp->pmux->npins; i++)
		ret |= gpio_pin_free(&obledPort[i]);
	free(obledPort);
	obled_sp = NULL;
exit:
	lock_release(&obledlock);
	return ret;
}

INCLUDE_DRIVER(OBrdLED, onboardled_setup, onboardled_exit, 0, 255, 255);