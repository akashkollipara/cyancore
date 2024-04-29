/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: gpio.c
 * Description		: This file contains sources for mega-avr
 *			  HAL gpio apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <stdbool.h>
#include <syslog.h>
#include <assert.h>
#include <mmio.h>
#include <resource.h>
#include <visor_call.h>
#include <arch.h>
#include <lock/lock.h>
#include <hal/gpio.h>
#include "gpio_private.h"

/**
 * gpio_pin_alloc - Allocates the needed GPIO pin
 *
 * @brief Allocates the GPIO pin and looks for availability
 *
 * @param[out] port: Pointer to the GPIO port structure
 * @param[in] portID: Port ID of the GPIO pin
 * @param[in] pinID: Pin ID of the GPIO pin
 *
 * @return status: Status of allocation operation
 */

status_t gpio_pin_alloc(gpio_port_t *port, uint8_t portID, uint8_t pinID)
{
	vret_t vres;
	gpio_module_t *dp;
	unsigned char flag;

	STATUS_CHECK_POINTER(port);

	lock_acquire(&gpio_lock);
	flag = 0;
	if(port_status[portID] & (1 << pinID))
		flag = 1;
	else
		port_status[portID] |= (1 << pinID);
	lock_release(&gpio_lock);

	if(flag)
	{
		sysdbg4("GPIO Pin %d on Port %d is already taken\n", pinID, portID);
		return error_driver_busy;
	}

	port->pin = pinID;
	port->port = portID;
	arch_visor_call(fetch_dp, gpio, portID, 0, &vres);
	if(vres.status != success)
	{
		sysdbg4("GPIO Port %d not found in DP\n", portID);
		return vres.status;
	}
	dp = (gpio_module_t *)vres.p;
	port->pbaddr = dp->baddr;
	sysdbg4("GPIO engine @ %p\n", dp->baddr);
	sysdbg4("Using GPIO Pin %d on Port %d\n", port->pin, port->port);
	return success;
}

/**
 * gpio_pin_mode - Sets GPIO pin mode
 *
 * @brief Sets the mode of a GPIO pin (ex: Input, Output)
 *
 * @param[in] port: Pointer to the GPIO port structure
 * @param[in] mode: Specified GPIO setting
 *
 * @return status: Status of mode setting operation
 */

status_t gpio_pin_mode(const gpio_port_t *port, gpio_mode_t mode)
{
	uintptr_t pbaddr;
	STATUS_CHECK_POINTER(port);
	uint8_t pin = port->pin;
	pbaddr = port->pbaddr;

	switch(mode)
	{
		case out:
			MMIO8(pbaddr + DDR_OFFSET) |= (1 << pin);
			break;
		case pull_up:
			MMIO8(pbaddr + PORT_OFFSET) |= (1 << pin);
		case in:
			MMIO8(pbaddr + DDR_OFFSET) &= ~(1 << pin);
			break;
		default:
			return error_func_inval_arg;
	}
	return success;
}

/**
 * gpio_pin_free - Frees specific GPIO pin
 *
 * @brief Frees a previously allocated GPIO pin
 *
 * @param[out] port: Pointer to the GPIO port structure
 *
 * @return status: Status of freeing operation
 */

status_t gpio_pin_free(gpio_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	lock_acquire(&gpio_lock);
	sysdbg4("Releasing GPIO Pin %d on Port, %d", port->pin, port->port);
	port_status[port->port] &= ~(1 << port->pin);
	port->pbaddr = 0x00;
	port->port = 0;
	port->pin = 0;
	lock_release(&gpio_lock);
	return success;
}

/**
 * gpio_pin_set - Sets a GPIO pin high
 *
 * @brief Sets a GPIO pin to high logic level by setting appropriate bit in the control register
 *
 * @param[in] port: Pointer to the GPIO port structure
 *
 * @return status: Status of setting operation
 */

status_t gpio_pin_set(const gpio_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->pbaddr + PORT_OFFSET) |= (1 << port->pin);
	return success;
}

/**
 * gpio_pin_clear - Sets a GPIO pin low
 *
 * @brief Sets a GPIO pin to low logic level by setting appropriate bit in the control register
 *
 * @param[in] port: Pointer to the GPIO port structure
 *
 * @return status: Status of clearing operation
 */

status_t gpio_pin_clear(const gpio_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->pbaddr + PORT_OFFSET) &= ~(1 << port->pin);
	return success;
}

/**
 * gpio_pin_toggle - Toggles a GPIO pin state
 *
 * @brief Toggles the state of a GPIO pin (high to low)
 *
 * @param[in] port: Pointer to the GPIO port structure
 *
 * @return status: Status of toggling operation
 */

status_t gpio_pin_toggle(const gpio_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->pbaddr + PORT_OFFSET) ^= (1 << port->pin);
	return success;
}

/**
 * gpio_pin_read - Reads a GPIO pin state
 *
 * @brief Reads the state of a GPIO pin (high or low)
 *
 * @param[in] port: Pointer to the GPIO port structure
 *
 * @return state: State of the GPIO pin depending on high, low for true, false
 */

bool gpio_pin_read(const gpio_port_t *port)
{
	assert(port);
	return (MMIO8(port->pbaddr + PIN_OFFSET) & (1 << port->pin)) ? true : false;
}

/**
 * gpio_port_alloc - Allocates a GPIO port
 *
 * @brief Allocates a GPIO port and checks for availability different from needing pinID
 *
 * @param[out] port: Pointer to the GPIO port structure
 * @param[in] portID: Port ID of the GPIO port
 *
 * @return status: Status of allocation operation
 */

status_t gpio_port_alloc(gpio_port_t *port, uint8_t portID)
{
	vret_t vres;
	gpio_module_t *dp;
	unsigned char flag = 0;

	STATUS_CHECK_POINTER(port);

	lock_acquire(&gpio_lock);
	if(port_status[portID])
		flag = 1;
	else
		port_status[portID] = (uint8_t)((uint16_t)(1 << BIT) - 1);
	lock_release(&gpio_lock);

	if(flag)
	{
		sysdbg4("GPIO Port %d is already taken\n", portID);
		return error_driver_busy;
	}

	port->pin = (uint8_t)((uint16_t)(1 << BIT) - 1);
	port->port = portID;
	arch_visor_call(fetch_dp, gpio, portID, 0, &vres);
	if(vres.status != success)
	{
		sysdbg4("GPIO Port %d not found in DP\n", portID);
		return vres.status;
	}
	dp = (gpio_module_t *)vres.p;
	port->pbaddr = dp->baddr;
	sysdbg4("GPIO engine @ %p\n", dp->baddr);
	return success;
}

/**
 * gpio_port_mode - Sets GPIO port mode
 *
 * @brief Sets the mode of a GPIO port (ex: input, output, pull_up)
 *
 * @param[in] port: Pointer to the GPIO port structure
 * @param[in] mode: GPIO mode to set
 *
 * @return status: Status of mode setting operation
 */

status_t gpio_port_mode(const gpio_port_t *port, gpio_mode_t mode)
{
	uintptr_t pbaddr;
	STATUS_CHECK_POINTER(port);
	uint8_t value = port->pin;
	pbaddr = port->pbaddr;

	switch(mode)
	{
		case out:
			MMIO8(pbaddr + DDR_OFFSET) = value;
			break;
		case pull_up:
			MMIO8(pbaddr + PORT_OFFSET) = value;
		case in:
			MMIO8(pbaddr + DDR_OFFSET) &= ~value;
			break;
		default:
			return error_func_inval_arg;
	}
	return success;
}

/**
 * gpio_port_free - Frees a GPIO port
 *
 * @brief Frees a previously allocated GPIO port different from freeing pin
 *
 * @param[out] port: Pointer to the GPIO port structure
 *
 * @return status: Status of freeing operation
 */

status_t gpio_port_free(gpio_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	lock_acquire(&gpio_lock);
	sysdbg4("Releasing GPIO Port, %d", port->port);
	port_status[port->port] = 0;
	port->pbaddr = 0x00;
	port->port = 0;
	port->pin = 0;
	lock_release(&gpio_lock);
	return success;
}

/**
 * gpio_port_write - Writes to a GPIO port
 *
 * @brief Writes data to a GPIO port
 *
 * @param[in] port: Pointer to the GPIO port structure
 * @param[in] val: Data to write to the port
 *
 * @return status: Status of writing operation
 */

status_t gpio_port_write(const gpio_port_t *port, gpio_parallel_t val)
{
	STATUS_CHECK_POINTER(port);
	// MegaAVR has 8-bit i/o modules, value to write should be 8-bit wide
	MMIO8(port->pbaddr + PORT_OFFSET) = (uint8_t)(val & 0xff);
	return success;
}

/**
 * gpio_port_read - Reads from a GPIO port
 *
 * @brief Reads data from a GPIO port
 *
 * @param[in] port: Pointer to the GPIO port structure
 * @param[out] val: Variable to store the read data
 *
 * @return status: Status of reading operation
 */

status_t gpio_port_read(const gpio_port_t *port, gpio_parallel_t *val)
{
	STATUS_CHECK_POINTER(port);
	// MegaAVR has 8-bit i/o modules, value returned is 8-bit wide
	*val = (gpio_parallel_t)MMIO8(port->pbaddr + PIN_OFFSET);
	return success;
}
