/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: init.c
 * Description		: This file consists of init routine of the framework.
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*

1. vector table for El2 Define
2. MPU config for EL2,EL1 define
3. Vector table for EL1
4. MP
*/


__asm__ __volatile__("mov r1, #2\n"
			     "mcr p15, 4, r1, c12, c0, 2\n"); 
