/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: schedclk.c
 * Description		: This file contains sources of visor schedclk
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <status.h>
#include <syslog.h>
#include <arch.h>
#include <lock/lock.h>
#include <time.h>
#include <driver/schedclk.h>

/**
 * *port - Timer driver pointer
 *
 * This variable is updated by the schedclk_attach_device
 * when the device driver is initialised
 */
static schedclk_t *port[N_CORES];

/**
 * schedclk_attached - Flag to indicate the initialization status
 */
static bool schedclk_attached[N_CORES] = {[0 ... N_CORES-1] = false};

/**
 * tlock - Locks for sync in multi thread env
 */
static lock_t tlock[N_CORES];

/**
 * schedclk_attach_device - This function links hardware driver
 * and device driver.
 *
 * @brief This function links the hardware driver and schedclk device.
 * On successful linking, it updates the status flag so that
 * other functions know schedclk is attached.
 *
 * @param[in] dev_status: device init status
 * @param[in] *ptmr: device schedclk port
 * @return status: status of device/hardware driver
 */
status_t schedclk_attach_device(status_t dev_status, schedclk_t *ptmr)
{
	status_t ret;
	size_t cpu_index = arch_core_index();
	lock_t *lock = &tlock[cpu_index];

	lock_acquire(lock);
	/* Link the driver instance */
	port[cpu_index] = ptmr;

	if(port[cpu_index] != NULL)
	{
		ret = dev_status;
		schedclk_attached[cpu_index] = true;
	}
	else
		ret = error_device_inval;
	lock_release(lock);
	return ret;
}

/**
 * schedclk_release_device - This function delinks hardware driver
 * and device driver.
 *
 * @brief This function simply clear hardware driver pointer and updates
 * schedclk status flag
 *
 * @return status: status of device/hardware driver
 */
status_t schedclk_release_device()
{
	size_t cpu_index = arch_core_index();
	lock_t *lock = &tlock[cpu_index];
	lock_acquire(lock);
	port[cpu_index] = NULL;
	schedclk_attached[cpu_index] = false;
	lock_release(lock);
	return success;
}

/**
 * schedclk_link_callback - This is a helper function which lets other
 * programs to link schedclk event call back functions.
 *
 * @brief This is a helper function which lets other programs to link
 * schedclk event call back functions. It allows to link only 1 callback
 * which will be executed as part of schedclk ISR handler.
 *
 * @param[in] p: period of schedclk irq
 * @param[in] cb: call back function pointer
 * @return status
 */
status_t schedclk_link_callback(unsigned int p, void *cb)
{
	size_t cpu_index = arch_core_index();
	lock_t *lock = &tlock[cpu_index];
	schedclk_t *ptr = port[cpu_index];
	if(!schedclk_attached[cpu_index])
		return error_driver_init_failed;
	lock_acquire(lock);
	ptr->set_period(p);
	ptr->reg_cb(cb);
	lock_release(lock);
	return success;
}

/**
 * get_timestamp - This funtion reads timestamp
 *
 * @brief This function reads time from lower level schedclk
 * functions and updates the input potiner. If the driver
 * is not initialised, then pointer is updated to 0.
 *
 * This function's prototype is located in libc.
 *
 * @param[in] *t: pointer to store time
 * @return status
 */
status_t get_timestamp(uint64_t *t)
{
	size_t cpu_index = arch_core_index();
	lock_t *lock = &tlock[cpu_index];
	schedclk_t *ptr = port[cpu_index];
	if(!schedclk_attached[cpu_index])
	{
		*t = 0;
		return error_driver_init_failed;
	}
	lock_acquire(lock);
	*t = ptr->read_time();
	lock_release(lock);
	return success;
}

/**
 * get_timeticks - This funtion reads schedclk ticks
 *
 * @brief This function reads ticks from lower level schedclk
 * functions and updates the input potiner. If the driver
 * is not initialised, then pointer is updated to 0.
 *
 * This function's prototype is located in libc.
 *
 * @param[in] *t: pointer to store ticks
 * @return status
 */
status_t get_timeticks(uint64_t *t)
{
	size_t cpu_index = arch_core_index();
	lock_t *lock = &tlock[cpu_index];
	schedclk_t *ptr = port[cpu_index];
	if(!schedclk_attached[cpu_index])
	{
		*t = 0;
		return error_driver_init_failed;
	}
	lock_acquire(lock);
	*t = ptr->read_ticks();
	lock_release(lock);
	return success;
}
