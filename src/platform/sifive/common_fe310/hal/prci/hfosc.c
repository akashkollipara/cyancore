/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: prci.c
 * Description		: This file contains prototypes of platform
 *			  power, reset, clock and interrupt module
 *			  functions and api calls
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <hal/prci.h>
#include <mmio.h>
#include <arch.h>
#include "prci_private.h"

#ifndef ICLK
#define ICLK	72000000U
INFO(< I > Using ICLK = 72Mhz)
#endif

status_t _NOINLINE prci_hfxocs_enable(const sysclk_port_t *port)
{
	/* Enable External Crystal Osc (xtal) */
	MMIO32(port->baddr + HFXOSCCFG_OFFSET) |= (1U << HFXOSCEN);
	/* Wait till ready */
	while(!(MMIO32(port->baddr + HFXOSCCFG_OFFSET) & (1U << HFXOSCRDY)));
	return (MMIO32(port->baddr + HFXOSCCFG_OFFSET) & (1U << HFXOSCEN)) ?
		success : error_system;
}

status_t _NOINLINE prci_hfxocs_disable(const sysclk_port_t *port)
{
	/* Disable External Crystal Osc (xtal) */
	MMIO32(port->baddr + HFXOSCCFG_OFFSET) &= ~(1U << HFXOSCEN);
	return (MMIO32(port->baddr + HFXOSCCFG_OFFSET) & (1U << HFXOSCEN)) ?
		error_system : success;
}

status_t _NOINLINE prci_hfosc_enable(const sysclk_port_t *port)
{
	MMIO32(port->baddr + HFROSCCFG_OFFSET) |= (1U << HFROSCEN);
	while(!(MMIO32(port->baddr + HFROSCCFG_OFFSET) & (1U << HFROSCRDY)));
	return (MMIO32(port->baddr + HFROSCCFG_OFFSET) & (1U << HFROSCEN)) ?
		success : error_system;
}

status_t _NOINLINE prci_hfosc_disable(const sysclk_port_t *port)
{
	MMIO32(port->baddr + HFROSCCFG_OFFSET) &= ~(1U << HFROSCEN);
	return (MMIO32(port->baddr + HFROSCCFG_OFFSET) & (1U << HFROSCEN)) ?
		error_system : success;
}

status_t _NOINLINE prci_hfosc_set_clk(const sysclk_port_t *port, unsigned int clk)
{
	unsigned int hfrosc;
	unsigned int divdr = ICLK / clk;
	divdr -= 1;
	hfrosc = MMIO32(port->baddr + HFROSCCFG_OFFSET);
	hfrosc &= ~HFROSCDIV_MASK;
	hfrosc |= (divdr & HFROSCDIV_MASK);
	MMIO32(port->baddr + HFROSCCFG_OFFSET) = hfrosc;
	while(!(MMIO32(port->baddr + HFROSCCFG_OFFSET) & (1U << HFXOSCRDY)));

	return ((MMIO32(port->baddr + HFROSCCFG_OFFSET) & HFROSCDIV_MASK) ==
		divdr) ? success : error_system;
}

status_t _NOINLINE prci_hfosc_get_clk(const sysclk_port_t *port, unsigned int *clk)
{
	unsigned int hfrosc;
	unsigned int divdr;
	hfrosc = MMIO32(port->baddr + HFROSCCFG_OFFSET);
	divdr = hfrosc & HFROSCDIV_MASK;
	*clk = ICLK / (divdr + 1);
	return success;
}

#ifdef OEM
/*
 * Note:
 * - This function to be used only by OEM
 * - Avoid calling this function
 * - This function is untested
 */
status_t _NOINLINE prci_hfosc_set_trim(const sysclk_port_t *port, unsigned int trim)
{
	unsigned int hfrosc;

	hfrosc = MMIO32(port->baddr + HFROSCCFG_OFFSET);
	hfrosc &= ~HFROSCTRIM_MASK;
	hfrosc |= (trim << HFROSCTRIM);
	MMIO32(port->baddr + HFROSCCFG_OFFSET) = hfrosc;
	while(!(MMIO32(port->baddr + HFROSCCFG_OFFSET) & (1U << HFXOSCRDY)));

	return (((MMIO32(port->baddr + HFROSCCFG_OFFSET) & HFROSCTRIM_MASK)
			>> HFROSCTRIM) == trim) ? success : error_system;
}
#endif
