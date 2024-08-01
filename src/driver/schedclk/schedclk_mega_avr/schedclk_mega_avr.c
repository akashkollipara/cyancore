/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: platform_timer.c
 * Description		: This file contains sources for platform timer apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <resource.h>
#include <visor_call.h>
#include <arch.h>
#include <hal/timer.h>
#include <driver/schedclk.h>

#define PS	4
#define PSVALUE	256

/**
 * tmr_cb - Timer event call back
 */
static void (*tmr_cb)(void);

/**
 * tport - Timer HAL device instance
 */
static timer_port_t *tport;

/**
 * ticks - Driver variable for keeping track of timer ticks
 * for event
 */
static uint64_t ticks;

/**
 * cntr - Timer counter
 */
static uint64_t cntr;

/**
 * tm - timer device module
 */
static module_t *tm;

/**
 * plat_tmr_isr - platform timer isr handler
 *
 * @brief This function is timer event handler. It updated timer counter.
 * Also, this function executes registered timer callback additionally
 * which will be used by scheduler.
 */
static void plat_tmr_isr(void)
{
	cntr += ticks;
	if(tmr_cb != NULL)
		tmr_cb();
}

/**
 * plat_timer_reg_cb - Funtion to register call back
 */
static void plat_timer_reg_cb(void *cb)
{
	tmr_cb = cb;
}

/**
 * plat_get_timer_prop - Helper function to fetch timer properties
 */
static status_t plat_get_timer_prop(void)
{
	vret_t vres;
	swdev_t *sp;
	hw_devid_t devid;

	arch_visor_call(fetch_sp, sched_timer, 0, 0, &vres);
	if(vres.status != success)
	{
		sysdbg3("%p - sp node could not be found!\n", sched_timer);
		return vres.status;
	}
	sp = (swdev_t *) vres.p;
	devid = sp->hwdev_id;

	arch_visor_call(fetch_dp, (devid & 0xff00), (devid & 0xff), 0, &vres);
	if(vres.status != success)
	{
		sysdbg3("Timer Device %d not found!\n", devid);
		return vres.status;
	}
	tm = (module_t *) vres.p;
	return success;
}

/**
 * plat_get_timer_ticks_msec - Helper function to get tick/msec
 */
static uint64_t plat_get_timer_ticks_msec(uint64_t freq)
{
	uint64_t nt = freq / (2 * PSVALUE);
	nt /= 1000;
	return nt;
}

/**
 * plat_timer_set_period - Sets period for the timer events
 *
 * @brief This function configures timer to raise event after the
 * programmed value.
 *
 * @param[in] p: Period of events in milli seconds
 */
static void plat_timer_set_period(unsigned int p)
{
	uint64_t nt = plat_get_timer_ticks_msec(tm->clk);
	ticks = (nt * p) - 1;
	tport->value = (size_t) ticks;
	timer_setup(tport, 2, PS);
}

/**
 * plat_read_ticks - Returns timer ticks
 */
static uint64_t plat_read_ticks(void)
{
	return cntr;
}

/**
 * plat_read_time - This function returns time
 *
 * @brief This function returns time based on timer events.
 * The value might not be accurate to wall clock.
 *
 * @return time in microseconds
 */
static uint64_t plat_read_time(void)
{
	uint64_t stamp = cntr;
	stamp <<= 10U;
	stamp /= ((tport->value + 1) << 1);
	return stamp;
}

/**
 * Driver ops for linking timer
 */
static schedclk_t *plat_timer_port;

static status_t plat_timer_exit(void);

/**
 * plat_timer_setup - Timer driver setup function
 * To be exported to driver table.
 */
static status_t plat_timer_setup()
{
	status_t ret = success;
	ret |= plat_get_timer_prop();

	tport = (timer_port_t *)malloc(sizeof(timer_port_t));
	if(!tport)
		return error_memory_low;

	plat_timer_port = (schedclk_t *)malloc(sizeof(schedclk_t));
	if(!plat_timer_port)
		return error_memory_low;

	if(!tm->clk)
	{
		ret = error_system_clk_caliberation;
		goto cleanup_exit1;
	}

	tport->port_id = tm->id;
	tport->clk_id = tm->clk_id;
	tport->baddr = tm->baddr;
	tport->stride = tm->stride;
	tport->tmr_irq = (size_t) tm->interrupt[0].id;
	tport->tmr_handler = plat_tmr_isr;

	ret |= timer_setup(tport, 2, PS);
	if(ret)
		goto cleanup_exit2;
	plat_timer_set_period(1);

	plat_timer_port->read_ticks = &plat_read_ticks;
	plat_timer_port->read_time = &plat_read_time;
	plat_timer_port->set_period = &plat_timer_set_period;
	plat_timer_port->reg_cb = &plat_timer_reg_cb;

	ret = schedclk_attach_device(ret, plat_timer_port);
	if(!ret)
		goto exit;
cleanup_exit2:
	plat_timer_exit();
cleanup_exit1:
	if(tport)
		free(tport);
exit:
	return ret;
}

/**
 * plat_timer_exit - Timer driver shutdown function
 * To be exported to driver table.
 */
static status_t plat_timer_exit(void)
{
	status_t ret = timer_shutdown(tport);
	ticks = 0;
	tmr_cb = (void *) 0;
	memset(tport, 0, sizeof(timer_port_t));
	ret |= schedclk_release_device();
	free(tport);
	free(plat_timer_port);
	return ret;
}

INCLUDE_DRIVER(schedclk, plat_timer_setup, plat_timer_exit, 0, 1, 1);
