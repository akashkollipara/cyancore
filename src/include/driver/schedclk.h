/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: schedclk.h
 * Description		: This file consists of prototypes for schedclk driver
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

#define _VISOR_TIMER_T_

typedef struct schedclk
{
	uint64_t (*read_ticks)(void);
	uint64_t (*read_time)(void);
	void (*set_period)(unsigned int);
	void (*reg_cb)(void *);
} schedclk_t;

status_t schedclk_attach_device(status_t, schedclk_t *);
status_t schedclk_release_device();
status_t schedclk_link_callback(unsigned int, void *);
