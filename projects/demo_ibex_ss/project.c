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
#include <stdlib.h>
#include <string.h>
#include <visor/bootstrap.h>
#include <driver.h>
#include <time.h>
#include <platform.h>

void plug()
{
	bootstrap();
	driver_setup_all();

	printf("Demo Program!\n");
	mdelay(1000);
	printf("Bye...\n");
	exit(EXIT_SUCCESS);
	return;
}

