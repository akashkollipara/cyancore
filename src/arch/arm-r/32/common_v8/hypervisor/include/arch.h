/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019-2022, Cyancore Team
 *
 * File Name		: arch.h
 * Description		: This file prototypes arch related functions and
 *			  defines inline-able arch functions.
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

#define _ARCH_H_
#include <stdbool.h>
#include <resource.h>
#include "armv8_32.h"
#include <visor_call.h>

/**
 * arch_early_setup - This needs to be called in early stages of boot
 */
void arch_early_setup();

/**
 * arch_setup - This needs to be called after inital setup is completed
 */
void arch_setup();
void arch_setup2();

void arch_di_save_state();
void arch_ei_restore_state();

/**
 * arch_panic_handler - Executes when arch error occurs
 */
void arch_panic_handler() _NORETURN;
void arch_unhandled_irq() _NORETURN;

void arch_early_signal_boot_start();
void arch_wait_till_boot_done();
void arch_signal_boot_done();

/**
 * arch_register_interrupt_handler - Registers interrtup handler
 * for arch specific exception vectors
 */
void arch_register_interrupt_handler(unsigned int, void (*)(void));

/**
 * local_register_interrupt_handler - Registers interrupt handler
 * for cpu specific interrupt vectors
 */
void local_register_interrupt_handler(unsigned int, void (*)(void));

/**
 * arch_core_index - Returns code index
 */
static inline unsigned int arch_core_index()
{
	unsigned int id;
	asm volatile("csrr %0, mhartid" : "=r" (id));
	return id;
}

/**
 * arch_visor_call - perform machine call
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
static inline void arch_visor_call(unsigned int code, unsigned int arg0, unsigned int arg1, unsigned int arg2, vret_t *ret)
{
	if(ret == NULL)
		return;
	register uint32_t a0 asm("a0") = code;
	register uint32_t a1 asm("a1") = arg0;
	register uint32_t a2 asm("a2") = arg1;
	register uint32_t a3 asm("a3") = arg2;
	asm volatile("ecall"
				: "+r" (a0), "+r" (a1), "+r"(a2)
				: "r" (a0), "r" (a1), "r" (a2), "r" (a3)
				: "memory");
	ret->p = a0;
	ret->size = a1;
	ret->status = a2;
	return;
}

/**
 * arch_ei - arch enable global interrupts
 */
static inline void arch_ei()
{
	unsigned int bits = (1 << 3) | (1 << 7);
	asm volatile("csrs mstatus, %0" : : "r" (bits));
}

static inline void arch_ei_mei()
{
	unsigned int bits = (1 << 11);
	asm volatile("csrs mie, %0" : : "r" (bits));
}

static inline void arch_di_mei()
{
	unsigned int bits = (1 << 11);
	asm volatile("csrc mie, %0" : : "r" (bits));
}

static inline void arch_ei_mtime()
{
	unsigned int bits = (1 << 7);
	asm volatile("csrs mie, %0" : : "r" (bits));
}

static inline void arch_di_mtime()
{
	unsigned int bits = (1 << 7);
	asm volatile("csrc mie, %0" : : "r" (bits));
}

static inline void arch_ei_msoftirq()
{
	unsigned int bits = (1 << 3);
	asm volatile("csrs mie, %0" : : "r" (bits));
}

static inline void arch_di_msoftirq()
{
	unsigned int bits = (1 << 3);
	asm volatile("csrc mie, %0" : : "r" (bits));
}

/**
 * arch_di - arch disable global interrupts
 */
static inline void arch_di()
{
	unsigned int bits = (1 << 3) | (1 << 7);
	asm volatile("csrc mstatus, %0" : : "r" (bits));
	bits |= (1 << 11);
	asm volatile("csrc mie, %0" : : "r" (bits));

}

static inline void arch_nop()
{
	asm volatile("nop");
}

static inline void arch_wfi()
{
#ifndef RV_NO_WFI
	asm volatile("wfi");
#if ERRATA_CIP578
	arch_nop();
	arch_nop();
#endif
#endif
}

static inline void arch_isb()
{
	asm volatile("fence.i");
}

static inline void arch_dsb()
{
	asm volatile("fence");
}

static inline void arch_dmb()
{
	fence(rw, rw);
}

bool arch_suspended_state_was(cpu_sleep_t);
void arch_signal_suspend(cpu_sleep_t);
void arch_signal_resume(void);
void arch_rseed_capture();
