/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: platform.h
 * Description		: This file consists of platform
 *			  and related prototypes
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

#include <reset.h>

reset_t platform_get_reset_syndrome();
void platform_reset_handler();
void platform_early_setup();
void platform_setup();
void platform_cpu_setup();
status_t platform_init_heap();
