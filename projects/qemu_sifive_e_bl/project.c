/*
 * CYANCORE LICENSE
 * Copyrights (C) 2023, Cyancore Team
 *
 * File Name		: project.c
 * Description		: This file consists of project srouces
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <status.h>
#include <syslog.h>
#include <visor/bootstrap.h>
#include <arch.h>
#include <driver.h>
#include <interrupt.h>
#include <platform.h>

void no_program(void);
void plug()
{
	bootstrap();
	driver_setup_all();
	syslog_stdout_enable();
	link_interrupt(int_arch, 2, no_program);
	syslog(info, "Bootloader!\n");
	syslog(info, "Jumping to XIP...\n");
	driver_exit_all();
	arch_di();
	platform_jump_to_user_code();
	return;
}

void no_program()
{
	driver_setup_all();
	syslog(fail, "No User program found!\n");
	plat_panic_handler();
}

