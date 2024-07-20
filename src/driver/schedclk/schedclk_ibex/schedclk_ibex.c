/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: plat_timer.c
 * Description		: This file contains sources for platform
 *			  timer
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <stdlib.h>
#include <assert.h>
#include <arch.h>
#include <driver.h>
#include <interrupt.h>
#include <visor_call.h>
#include <resource.h>
#include <platform.h>
#include <hal/clint.h>
#include <driver/schedclk.h>

/**
 * tmr_cb - Timer event call back
 */
static void (*tmr_cb)(void);

/**
 * ticks - Driver variable for keeping track of timer ticks
 * for event
 */
static uint64_t ticks;

/**
 * tm - timer device module
 */
static module_t *tm;

/**
 * plat_tmr_isr - platform timer isr handler
 *
 * @brief This function is timer event handler. This function
 * updated timercmp reg to raise event after 'ticks' have elapsed.
 * Also, this function executes registered timer callback additionally
 * which will be used by scheduler.
 */
static void plat_tmr_isr(void)
{
	arch_di_mtime();
	uint64_t t = clint_read_time();
	status_t ret = clint_config_tcmp(arch_core_index(), (t + ticks));
	if(ret)
	{
		syslog_stdout_enable();
		syslog(fail, "Failed to configure timer, Err = %p\n", ret);
		plat_panic_handler();
	}
	arch_ei_mtime();

	if(tmr_cb != NULL)
		tmr_cb();
}

/**
 * plat_timer_reg_cb - Funtion to register call back
 */
static void plat_timer_reg_cb(void *cb)
{
	arch_di_mtime();
	tmr_cb = cb;
	arch_ei_mtime();
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
	/* Compute ticks needed for 1 msec */
	return (uint64_t)(freq/1000);
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
	uint64_t nt;
	arch_di_mtime();
	ticks = plat_get_timer_ticks_msec(tm->clk);
	ticks *= p;
	nt = ticks + clint_read_time();
	status_t ret = clint_config_tcmp(arch_core_index(), nt);
	if(ret)
	{
		syslog_stdout_enable();
		syslog(fail, "Failed to configure timer, Err = %p\n", ret);
		plat_panic_handler();
	}
	arch_ei_mtime();
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
	uint64_t stamp = clint_read_time();
	stamp *= 1000000U;
	return (uint64_t)(stamp / tm->clk);
}

/**
 * Driver ops for linking timer
 */
static schedclk_t *plat_timer_port;

/**
 * plat_timer_setup - Timer driver setup function
 * To be exported to driver table.
 */
status_t plat_timer_setup(void)
{
	status_t ret = success;
	const irqs_t *irq;
	arch_di_mtime();

	plat_timer_port = (schedclk_t *)malloc(sizeof(schedclk_t));
	if(!plat_timer_port)
		return error_memory_low;
	plat_timer_port->read_ticks = &clint_read_time;
	plat_timer_port->read_time = &plat_read_time;
	plat_timer_port->set_period = &plat_timer_set_period;
	plat_timer_port->reg_cb = &plat_timer_reg_cb;

	/* This funcition fetches device properties */
	ret |= plat_get_timer_prop();

	irq = &tm->interrupt[0];

	/* Link timer isr handle */
	ret |= link_interrupt(irq->module, irq->id, &plat_tmr_isr);
	ret |= schedclk_attach_device(ret, plat_timer_port);
	plat_timer_set_period(1);
	return ret;
}

/**
 * plat_timer_exit - Timer driver shutdown function
 * To be exported to driver table.
 */
status_t plat_timer_exit(void)
{
	const irqs_t *irq;
	arch_di_mtime();
	tmr_cb = (void *) 0;
	ticks = 0;
	irq = &tm->interrupt[0];
	unlink_interrupt(irq->module, irq->id);
	free(plat_timer_port);
	return schedclk_release_device();
}

#if USE_TIMER
INCLUDE_DRIVER(schedclk, plat_timer_setup, plat_timer_exit, 0, 1, 1);
#endif
