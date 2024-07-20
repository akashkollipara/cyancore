/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
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
#include <driver/onboardled.h>
#include <platform.h>
#include <time.h>

void no_program(void);
void plug()
{
	bootstrap();
	driver_setup_all();
	link_interrupt(int_arch, 2, no_program);

	syslog(info, "Bootloader!\n");
	syslog(info, "Waiting for 2secs ...\n");
	for(int i = 0; i < 40; i++)
	{
		onboardled_toggle();
		mdelay(50);
	}
	arch_di();
	onboardled_off();
	driver_exit_all();
	platform_jump_to_user_code();
	return;
}

void no_program()
{
	driver_setup_all();
	syslog(fail, "No User program found!\n");
	while(1)
	{
		onboardled_toggle();
		mdelay(1000);
	}
}

