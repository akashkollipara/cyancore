/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: arch.h
 * Description		: This file prototypes arch related functions and
 *			  defines inline-able arch functions.
 * Primary Author	: Mayuri Lokhande [mayurilokhande01@gmail.com],
 *			  Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once
#define _ARCH_H_

#include <arm.h>
#include <resource.h>
#include <visor_call.h>

/**
 * arch_early_setup - This needs to be called in early stages of boot
 */
void arch_early_setup();

/**
 * arch_setup - This needs to be called after intial setup is completed.
 */
void arch_setup();

void arch_di_save_state(istate_t *);
void arch_ei_restore_state(istate_t *);
unsigned int arch_core_index();

/**
 * arch_panic_handler - Executes when arch error occurs
 */
void arch_panic_handler();
void arch_unhandled_irq();

void arch_early_signal_boot_start();
void arch_wait_till_boot_done();
void arch_signal_boot_done();

/**
 * arch_register_interrupt_handler - Registers interrupt handler for
 * arch specific interrupt vectors
 */
void arch_register_interrupt_handler(unsigned int, void(*)(void));

/**
 * arch_visor_call - perform supervisor call
 *
 * @brief This function performs svc call
 *
 * @param[in] code: universal call code
 * @param[in] r0: first argument
 * @param[in] r1: second argument
 * @param[in] r2: third argument
 * @param[in] *ret: return struct
 */
static inline void arch_visor_call(unsigned int code, unsigned int arg0, unsigned int arg1, unsigned int arg2, vret_t *ret)
{
	if(ret == NULL)
		return;
	register uint32_t r0 asm("r0") = code;
	register uint32_t r1 asm("r1") = arg0;
	register uint32_t r2 asm("r2") = arg1;
	register uint32_t r3 asm("r3") = arg2;
	asm volatile("svc 0"
				:"+r" (r0), "+r" (r1), "+r"(r2)
				:"r" (r0), "r" (r0), "r" (r1), "r" (r2), "r" (r3)
				:"memory");

	ret->p = r0;
	ret->size = r1;
	ret->status = r2;
	return;
}

 /**
 *  arch_ei - arch enable global interrupts
 */
static inline void arch_ei()
{
	asm volatile("cpsie if");
}

/**
 *  arch_ei - arch enable global interrupts
 */
static inline void arch_di()
{
	asm volatile("cpsid if");
}

static inline void arch_nop()
{
	asm volatile("nop");
}

static inline void arch_wfi()
{
	asm volatile("wfi");
}

static inline void arch_isb()
{
	asm volatile("isb");
}

static inline void arch_dsb()
{
	asm volatile("dsb");
}

static inline void arch_dmb()
{
	asm volatile("dmb");
}

bool arch_suspended_state_was(cpu_sleep_t);
void arch_signal_suspend(cpu_sleep_t);
void arch_signal_resume(void);
