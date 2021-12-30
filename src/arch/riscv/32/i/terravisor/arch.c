/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: arch.c
 * Description		: This file consists of architecture specific function that
 *			  cannot be inlined. Hence, present in c file.
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <arch.h>

/**
 * arch_early_setup - This function is called in the early stages of boot
 *
 * @brief This function is responsible to clean reset cpu status/control registers.
 *
 */
void arch_early_setup()
{
	arch_di();
	riscv_update_vector();
	return;
}

/**
 * arch_setup - This function is called after initial setup is done
 *
 * @brief This function is called after initial setup is done.
 */
void arch_setup()
{
	return;
}

void arch_di_save_state()
{
}

void arch_ei_restore_state()
{
}

/**
 * arch_machine_call - perform machine call
 *
 * @brief This function performs environment call
 * in m mode
 *
 * @param[in] code: machine call code
 * @param[in] a0: first argument
 * @param[in] a1: second argument
 * @param[in] a2: third argument
 * @param[in] *ret: return struct
 */
void arch_machine_call(unsigned int code, unsigned int a0, unsigned int a1, unsigned int a2, mret_t *ret)
{
	if(ret == NULL)
		return;
	asm volatile("mv	a0, %0" : : "r" (code));
	asm volatile("mv	a1, %0" : : "r" (a0));
	asm volatile("mv	a2, %0" : : "r" (a1));
	asm volatile("mv	a3, %0" : : "r" (a2));
	asm volatile("ecall");
	asm volatile("mv	%0, a0" : "=r" (ret->p));
	asm volatile("mv	%0, a1" : "=r" (ret->size));
	asm volatile("mv	%0, a2" : "=r" (ret->status));
	return;
}

_WEAK void arch_panic_handler()
{
	context_frame_t *frame _UNUSED = get_context_frame();
	while(1)
		arch_wfi();
}

_WEAK void arch_unhandled_irq()
{
	while(1)
		arch_wfi();
}
