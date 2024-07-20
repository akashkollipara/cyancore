/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: platform.c
 * Description		: This file contains sources for platform apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <rand.h>
#include <status.h>
#include <arch.h>
#include <visor_call.h>
#include <visor/platform.h>
#include <visor/workers.h>
#include <driver.h>
#include <syslog.h>
#include <insignia.h>
#include <platform.h>

/**
 * reset_syndrome - This varible is consists of reset syndrome in
 *		    bss section of memory. This variable is updated
 *		    in the platform_early_setup as this is the
 *		    earliest function which resets bss section.
 */
uint8_t reset_syndrome;

/**
 * platform_early_setup - Executes pre-setup functions
 *
 * @brief This function performs calls to certain function that
 * are necessary to be called to make the plaform ready for setup.
 * Function like bss clear, data copy, clock reset, etc are called
 * in this function. As reset_syndrome is bss variable, it must be
 * updated only after the bss is cleared. Ideally it should be
 * updated at last. < ! > This function should be made to run on
 * boot core only!
 */
void platform_early_setup()
{
	status_t ret = success;
	unsigned int temp_randomnumber = rand();
	/* Setup platform memories */
	ret |= platform_copy_data();
	ret |= platform_bss_clear();
	ret |= platform_init_heap();
	ret |= platform_resources_setup();
	srand(temp_randomnumber);
	/* Setup memory syslogger */
	driver_setup("mslog");

	reset_syndrome = MMIO8(MCUSR) & 0x1f;
	MMIO8(MCUSR) = 0;

	ret |= platform_clk_reset();
	ret |= platform_wdt_reset();

	if(ret != success)
		exit(EXIT_FAILURE);
	return;
}

#if PRINT_MEMORY_LAYOUT
static void platform_memory_layout()
{
	extern uint8_t _text_start, _text_size, _text_end,
	_data_vstart, _data_size, _data_vend,
	_stack_start, _stack_end, _stack_size,
	_bss_start, _bss_size, _bss_end,
	_heap_start, _heap_size, _heap_end,
	_flash_size, _ram_size;

	syslog(info, "Memory Info >\n");
	syslog(info, "Flash Size  : %u\n", &_flash_size);
	syslog(info, "RAM Size    : %u\n", &_ram_size);
	syslog(info, "\n");
	syslog(info, "Program Memory Layout >\n");
	syslog(info, "text Region\t: %06p - %06p : Size: %u\n",
			&_text_start, &_text_end, &_text_size);
	syslog(info, "bss Region\t: %06p - %06p : Size: %u\n",
			&_bss_start, &_bss_end, &_bss_size);
	syslog(info, "data Region\t: %06p - %06p : Size: %u\n",
			&_data_vstart, &_data_vend, &_data_size);
	syslog(info, "stack Region\t: %06p - %06p : Size: %u\n",
			&_stack_start, &_stack_end, &_stack_size);
	syslog(info, "heap Region\t: %06p - %06p : Size: %u\n",
			&_heap_start, &_heap_end, &_heap_size);
	syslog(info, "\n");
}
#endif

/**
 * platform_setup - Executes function to make platform read to init
 *
 * @brief This function performs calls to function which make the
 * framework ready to execute. In this case (MegaAVR), it is dp_setup.
 * < ! > This function should be made to run on boot core only!
 */
void platform_setup()
{
	sysdbg3("In %s\n", __func__);
	driver_setup("earlycon");
	bootmsgs_enable();
	cyancore_insignia_lite();
#if PRINT_MEMORY_LAYOUT
	platform_memory_layout();
#endif
	return;
}

/**
 * platform_cpu_setup - Perform platform setup calls on all cpus
 *
 * @brief This function perform calls to functions that must be executed
 * on all corea to make the cpu ready for the platform drivers.
 */
void platform_cpu_setup()
{
	sysdbg3("In %s\n", __func__);
	arch_ei();
	return;
}

void _NAKED plat_panic_handler_callback()
{
	context_frame_t *frame;
	sysdbg3("In %s\n", __func__);
	frame = get_context_frame();
	syslog(info, "SP=%p\tSREG = %p\n", frame, frame->sreg);
	exit(EXIT_FAILURE);
}
