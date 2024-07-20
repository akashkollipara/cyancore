/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: sysclk_prci.c
 * Description		: This file contains sources for sysclk based on
 *			  prci module
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <assert.h>
#include <resource.h>
#include <visor_call.h>
#include <driver.h>
#include <arch.h>
#include <platform.h>
#include <driver/sysclk.h>
#include <hal/prci.h>

static sysclk_port_t *sysclk;
static clock_type_t clk_type;
static lock_t sysclk_key;
static status_t sysclk_disable();

static status_t sysclk_setup()
{
	status_t ret = success;
	vret_t vres;
	module_t *dp;
	sysclk_port_t *port;
	istate_t ist;

	sysclk = (sysclk_port_t *)malloc(sizeof(sysclk_port_t));

	RET_ON_FAIL(sysclk, error_memory_low);

	port = sysclk;

	arch_visor_call(fetch_dp, clock, 0, 0, &vres);

	if(vres.status != success)
	{
		sysdbg2("Clock not found in DP!\n");
		port->base_clk = 0;
		ret = vres.status;
		goto cleanup_exit;
	}
	port->base_clk = *((unsigned int *)vres.p);

	arch_visor_call(fetch_dp, prci, 0, 0, &vres);

	if(vres.status != success)
	{
		sysdbg2("PRCI not found in DP!\n");
		ret = vres.status;
		goto cleanup_exit;
	}

	lock_acquire(&sysclk_key);
	arch_di_save_state(&ist);

	dp = (module_t *) vres.p;
	port->port_id = dp->id;
	port->baddr = dp->baddr;
	port->stride = dp->stride;
	port->type = dp->clk_id;
	goto exit;
cleanup_exit:
	free(sysclk);
exit:
	arch_ei_restore_state(&ist);
	lock_release(&sysclk_key);
	return ret;
}

static status_t sysclk_disable()
{
	status_t ret;
	istate_t ist;
	sysclk_port_t *port = sysclk;

	RET_ON_FAIL(port && port->baddr && port->base_clk,
			error_driver_data);

	lock_acquire(&sysclk_key);
	arch_di_save_state(&ist);

	ret = prci_hfosc_enable(port);
	ret |= prci_hfosc_set_clk(port, 16000000);
	prci_pll_deselect_pll(port);
	prci_pll_inline(port);
	memset(port, 0, sizeof(sysclk_port_t));
	free(sysclk);

	arch_ei_restore_state(&ist);
	lock_release(&sysclk_key);
	return ret;
}

INCLUDE_DRIVER(sysclk_prci, sysclk_setup, sysclk_disable, 0, 1, 1);

status_t sysclk_reset()
{
	status_t ret;
	const sysclk_port_t *port = sysclk;
	istate_t ist;

	RET_ON_FAIL(port && port->baddr && port->base_clk,
			error_driver_data);

	lock_acquire(&sysclk_key);
	arch_di_save_state(&ist);

	ret = prci_hfosc_enable(port);
	ret |= prci_hfxocs_enable(port);
	prci_pll_select_xosc(port);
	prci_pll_select_pll(port);
	prci_pll_bypass(port);
	ret |= prci_hfosc_disable(port);
	clk_type = external;

	arch_ei_restore_state(&ist);
	lock_release(&sysclk_key);
	return ret;
}

static inline void sysclk_set_internal(unsigned int clk _UNUSED)
{
	status_t ret;
	const sysclk_port_t *port = sysclk;
	ret = prci_hfxocs_enable(port);
	prci_pll_bypass(port);
	ret |= prci_hfosc_enable(port);
	ret |= prci_hfosc_set_clk(port, clk);
	prci_pll_select_rosc(port);
	prci_pll_deselect_pll(port);
	prci_pll_inline(port);
	ret |= prci_hfxocs_disable(port);
	if(ret)
		syslog(fail, "Failed to configure Internal clk!\n");
	return;
}

static inline void sysclk_set_external(void)
{
	status_t ret;
	const sysclk_port_t *port = sysclk;
	ret = prci_hfosc_enable(port);
	ret |= prci_hfxocs_enable(port);
	prci_pll_select_xosc(port);
	prci_pll_select_pll(port);
	prci_pll_bypass(port);
	ret |= prci_hfosc_disable(port);
	if(ret)
		syslog(fail, "Failed to configure External clk!\n");
	return;
}

static inline void sysclk_set_pll(unsigned int clk)
{
	status_t ret;
	const sysclk_port_t *port = sysclk;
	ret = prci_hfosc_enable(port);
	ret |= prci_hfxocs_enable(port);
	prci_pll_bypass(port);
	prci_pll_select_xosc(port);
	prci_pll_set_clk(port, clk);
	prci_pll_select_pll(port);
	prci_pll_inline(port);
	ret |= prci_hfosc_disable(port);
	if(ret)
		syslog(fail, "Failed to configure PLL clk!\n");
	return;
}

static void sysclk_configure_clk(call_arg_t a0, call_arg_t a1, call_arg_t a2 _UNUSED, vret_t *ret)
{
	const sysclk_port_t *port = sysclk;
	istate_t ist;
	clock_type_t type = (clock_type_t) a0;
	unsigned int clk = (unsigned int) a1;

	ret->status = error_inval_pointer;
	RET_ON_FAIL(port && port->baddr && port->base_clk,);

	ret->p = 0;
	ret->size = 0;
	ret->status = sysclk_execute_pre_config_clk_callback();

	lock_acquire(&sysclk_key);
	arch_di_save_state(&ist);
	switch(type)
	{
		case internal:
			sysclk_set_internal(clk);
			clk_type = internal;
			break;
		case external:
			sysclk_set_internal(clk);
			sysclk_set_external();
			clk_type = external;
			break;
		case pll:
			sysclk_set_pll(clk);
			clk_type = pll;
			break;
		default:
			ret->status = error_func_inval_arg;
			break;
	}


	arch_ei_restore_state(&ist);
	lock_release(&sysclk_key);
	ret->status |= sysclk_execute_post_config_clk_callback();
	return;
}

INCLUDE_VCALL(sysclk_set, config_clk, sysclk_configure_clk);

static void sysclk_get_freq(call_arg_t a0 _UNUSED, call_arg_t a1 _UNUSED, 
		call_arg_t a2 _UNUSED, vret_t *ret)
{
	const sysclk_port_t *port = sysclk;
	istate_t ist;
	unsigned int getclk;

	ret->status = error_inval_pointer;
	RET_ON_FAIL(port && port->baddr && port->base_clk,);

	lock_acquire(&sysclk_key);
	arch_di_save_state(&ist);

	ret->status = success;
	ret->p = sizeof(unsigned int);

	switch(clk_type)
	{
		case internal:
			ret->status = prci_hfosc_get_clk(port, &getclk);
			ret->p = (uintptr_t) getclk;
			break;
		case external:
			ret->p = (uintptr_t) port->base_clk;
			break;
		case pll:
			ret->status = prci_pll_get_clk(port, &getclk);
			ret->p = (uintptr_t) getclk;
			break;
		default:
			ret->p = (uintptr_t) 0;
			ret->size = 0;
			ret->status = error_func_inval_arg;
			break;
	}

	arch_ei_restore_state(&ist);
	lock_release(&sysclk_key);

	return;
}

INCLUDE_VCALL(sysclk_get, fetch_clk, sysclk_get_freq);
