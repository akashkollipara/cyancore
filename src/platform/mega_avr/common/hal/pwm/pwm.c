/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: pwm.c
 * Description		: This file contains sources for mega-avr
 *			  HAL pwm apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <status.h>
#include <stdbool.h>
#include <stdlib.h>
#include <syslog.h>
#include <resource.h>
#include <visor_call.h>
#include <arch.h>
#include <hal/timer.h>
#include <hal/pwm.h>

/**
 * pwm_to_timer - Convert PWM port to Timer port
 *
 * @brief Converts PWM port structure to Timer port structure
 *
 * @param[in] p: Pointer to the PWM port structure
 * @param[out] t: Pointer to the Timer port structure
 */

static void pwm_to_timer(const pwm_port_t *p, timer_port_t *t)
{
	t->port_id = p->port_id;
	t->clk_id = p->clk_id;
	t->baddr = p->baddr;
	t->stride = p->stride;
	t->tmr_irq = p->pwm_irq->id;
	t->tmr_handler = p->pwm_handler;
}

/**
 * pwm_get_properties - Get PWM properties
 *
 * @brief Retrieves PWM properties and maps them to the PWM port structure.
 *
 * @param[out] port: Pointer to the PWM port structure
 * @param[in] dev: Software device ID
 *
 * @return status: Status of PWM properties retrieval operation
 */

status_t pwm_get_properties(pwm_port_t *port, sw_devid_t dev)
{
	vret_t vres;
	swdev_t *sp;
	module_t *dp;
	hw_devid_t devid;

	arch_visor_call(fetch_sp, dev, 0, 0, &vres);
	if(vres.status != success)
	{
		sysdbg3("%p - sp node could not be found!\n", dev);
		return vres.status;
	}

	sp = (swdev_t *) vres.p;
	devid = sp->hwdev_id;
	port->pmux = sp->pmux;

	arch_visor_call(fetch_dp, (devid & 0xff00), (devid & 0xf0), 0, &vres);
	if(vres.status != success)
	{
		sysdbg3("PWM (timer) Device %d not found!\n", devid);
		return vres.status;
	}
	dp = (module_t *) vres.p;
	port->port_id = dp->id;
	port->clk_id = dp->clk_id;
	port->baddr = dp->baddr;
	port-> stride = dp->stride;
	port->pwm_irq = &dp->interrupt[0];
	return success;
}

/**
 * pwm_setup - Configure PWM settings
 *
 * @brief Configures PWM settings using the associated Timer port
 *
 * @param[in] port: Pointer to the PWM port structure
 * @param[in] mode: PWM mode (e.g., PWM, timer)
 *
 * @return status: Status of PWM setup operation
 */

status_t pwm_setup(const pwm_port_t *port, unsigned int mode)
{
	status_t ret;
	/* Allocate timer port */
	timer_port_t *tport = (timer_port_t *)malloc(sizeof(timer_port_t));
	RET_ON_FAIL(tport, error_memory_low);

	/* Convert pwm to timer */
	pwm_to_timer(port, tport);

	/* Configure timer for PWM mode */
	ret = timer_setup(tport, mode, port->prescale);

	/* Release the memory */
	free(tport);

	return ret;
}

/**
 * pwm_set - Set PWM value
 *
 * @brief Sets the PWM value using the associated Timer port
 *
 * @param[in] port: Pointer to the PWM port structure
 * @param[in] value: PWM value to set
 * @param[in] invert: Invert PWM signal if true
 *
 * @return status: Status of PWM set operation
 */

status_t pwm_set(const pwm_port_t *port, size_t value, bool invert)
{
	status_t ret;
	/* Allocate timer port */
	timer_port_t *tport = (timer_port_t *)malloc(sizeof(timer_port_t));
	RET_ON_FAIL(tport, error_memory_low);

	/* Convert pwm to timer */
	pwm_to_timer(port, tport);

	/* Set PWM value */
	ret = timer_pwm_set(tport, invert, value);

	/* Release the memory */
	free(tport);

	return ret;
}
