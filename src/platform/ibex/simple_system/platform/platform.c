/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: platform.c
 * Description		: This file contains sources for platform functions
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <stdlib.h>
#include <rand.h>
#include <arch.h>
#include <driver.h>
#include <syslog.h>
#include <insignia.h>
#include <visor/platform.h>
#include <visor/workers.h>
#include <platform.h>
#include <mmio.h>

static void platform_sim_halt();

void platform_early_setup()
{
	status_t ret = success;
#if USE_PRNG
	unsigned int temp_randomnumber = rand();
#endif
	ret |= platform_bss_clear();
	ret |= platform_init_heap();
	ret |= platform_resources_setup();
#if USE_PRNG
	srand(temp_randomnumber);
#endif
	syslog_stdout_disable();
	driver_setup("mslog");

	if(ret != success)
		exit(EXIT_FAILURE);
	return;
}

static void platform_memory_layout()
{
	extern uint32_t _text_start, _text_size, _text_end,
	_rodata_start, _rodata_size, _rodata_end,
	_data_vstart, _data_size, _data_vend,
	_stack_start, _stack_end, _stack_size,
	_bss_start, _bss_size, _bss_end,
	_heap_start, _heap_size, _heap_end,
	_ram_size;

	syslog(info, "\n");
	syslog(info, "Program Memory Layout >\n");
	syslog(info, "RAM Size\t: %u\n", &_ram_size);
	syslog(info, "text Region\t: %010p - %010p : Size: %u\n",
			&_text_start, &_text_end, &_text_size);
	syslog(info, "rodata Region\t: %010p - %010p : Size: %u\n",
			&_rodata_start, &_rodata_end, &_rodata_size);
	syslog(info, "bss Region\t: %010p - %010p : Size: %u\n",
			&_bss_start, &_bss_end, &_bss_size);
	syslog(info, "data Region\t: %010p - %010p : Size: %u\n",
			&_data_vstart, &_data_vend, &_data_size);
	syslog(info, "stack Region\t: %010p - %010p : Size: %u\n",
			&_stack_end, &_stack_start, &_stack_size);
	syslog(info, "heap Region\t: %010p - %010p : Size: %u\n",
			&_heap_start, &_heap_end, &_heap_size);
	syslog(info, "\n");
}

void platform_setup()
{
	driver_setup("earlycon");
	bootmsgs_enable();
	cyancore_insignia();
	platform_print_cpu_info();
	platform_memory_layout();
	atexit(&platform_sim_halt);
	return;
}

void platform_cpu_setup()
{
	arch_ei();
	return;
}

static void platform_sim_halt()
{
	uintptr_t sim_ctrl_base = 0x20000;
	MMIO32(sim_ctrl_base + 0x8) = 1;
}
