/*
 * ============================================================
 * File		: init.c (AVR-8)
 * Author	: Akash Kollipara
 * ============================================================
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mmio.h>
#include <plat_arch.h>
#include <lattix.h>

extern uint16_t _stack_start;

extern void zero_reg(void);
extern void copy_data(void);

void init()
{
	uint16_t *stack_location = &_stack_start;
	MMIO8(SPL) = (uint8_t)((uint16_t)stack_location & 0x00ff);
	MMIO8(SPH) = (uint8_t)(((uint16_t)stack_location >> 8) & 0x00ff);
	zero_reg();
	lattix();
	exit(EXIT_FAILURE);
}
