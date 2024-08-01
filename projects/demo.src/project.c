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
#include <stdio.h>
#include <string.h>
#include <visor/bootstrap.h>
#include <driver.h>
#include <driver/onboardled.h>
#include <time.h>

void plug()
{
	bootstrap();
	driver_setup_all();

	printf("Demo Program!\n");
	return;
}


void play()
{
	static unsigned char i = 0;
	const char progress[] = "-\\|/";
	uint64_t time;
	char c = progress[(i++) % strlen(progress)];
	get_timestamp(&time);
	time /= 1000U;

	printf("[%012llu] Running Blinky ... [%c]", time, c);

	onboardled_toggle();

	mdelay(500);

	printf("\r");
	return;
}
