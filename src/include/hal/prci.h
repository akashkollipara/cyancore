/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: prci.h
 * Description		: This file consists of SYSCLK prototypes
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*
 * Note:
 * - For SiFive HAL, this header file is associated with prci.c
 */

#pragma once

#include <status.h>
#include <stdint.h>
#include <resource.h>
#include <lock/lock.h>
#include <platform.h>

typedef struct sysclk_port
{
	hw_devid_t port_id;
	unsigned int base_clk;
	uintptr_t baddr;
	uintptr_t stride;
	clock_type_t type;
	lock_t key;
} sysclk_port_t;

void prci_pll_set_clk(const sysclk_port_t *port, unsigned int clk);
status_t prci_pll_get_clk(const sysclk_port_t *port, unsigned int *clk);
void prci_pll_select_xosc(const sysclk_port_t *port);
void prci_pll_select_rosc(const sysclk_port_t *port);
void prci_pll_select_pll(const sysclk_port_t *port);
void prci_pll_deselect_pll(const sysclk_port_t *port);
void prci_pll_bypass(const sysclk_port_t *port);
void prci_pll_inline(const sysclk_port_t *port);

status_t prci_hfxocs_enable(const sysclk_port_t *port);
status_t prci_hfxocs_disable(const sysclk_port_t *port);

status_t prci_hfosc_enable(const sysclk_port_t *port);
status_t prci_hfosc_disable(const sysclk_port_t *port);
status_t prci_hfosc_set_clk(const sysclk_port_t *port, unsigned int clk);
status_t prci_hfosc_get_clk(const sysclk_port_t *port, unsigned int *clk);
#ifdef OEM
status_t prci_hfosc_set_trim(const sysclk_port_t *port, unsigned int trim);
#endif
