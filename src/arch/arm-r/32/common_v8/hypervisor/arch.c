/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: arch.c
 * Description		: This file consists of architecture specific function that
 *			  cannot be inlined. Hence, present in c file.
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <arch.h>
#include <lock/lock.h>
#include <visor/workers.h>
#include <interrupt.h>
#include <rand.h>

static void arch_vcall_handler()
{
	context_frame_t *frame = get_context_frame();
	vret_t vres;
	vcall_handler(frame->a0, frame->a1, frame->a2, frame->a3, &vres);
	fence(w, w);
	frame->a0 = vres.p;
	frame->a1 = vres.size;
	frame->a2 = vres.status;
	return;
}

static lock_t boot_key = LOCK_INITAL_VALUE;
void arch_early_signal_boot_start()
{
	boot_key = LOCK_INITAL_VALUE;
	return;
}

void arch_wait_till_boot_done()
{
	lock_acquire(&boot_key);
	lock_release(&boot_key);
	return;
}

void arch_signal_boot_done()
{
	lock_release(&boot_key);
	return;
}

/**
 * arch_early_setup - This function is called in the early stages of boot
 *
 * @brief This function is responsible to clean reset cpu status/control registers.
 *
 */
void arch_early_setup()
{
	arch_di();
	arch_di_mei();
	arch_di_mtime();
	arch_di_msoftirq();
#ifndef RV_VEC_MODE
	riscv_update_vector();
#endif
	return;
}

/**
 * arch_setup - This function is called after initial setup is done
 *
 * @brief This function is called after initial setup is done.
 */
void arch_setup()
{
	link_interrupt(int_arch, 11, &arch_vcall_handler);
	return;
}

void arch_setup2()
{
	link_interrupt(int_arch, 11, &arch_vcall_handler);
	return;
}

void arch_di_save_state(istate_t *istate)
{
	istate_t temp;
	asm volatile("csrr %0, mie" : "=r" (temp));
	*istate = temp & (1 << 3 | 1 << 7 | 1 << 11);
	arch_di();
}

void arch_ei_restore_state(istate_t *istate)
{
	asm volatile("csrs mie, %0" : : "r" (*istate));
	arch_ei();
}

static cpu_sleep_t sleep_flag[N_CORES];

/**
 * arch_suspended_state_was
 *
 * @brief This function checks per core suspend state
 * and returns true/false based arg.
 *
 * @param[in] state: suspended state
 * @return bool: True/False
 */
bool arch_suspended_state_was(cpu_sleep_t state)
{
	cpu_sleep_t temp = sleep_flag[arch_core_index()];
	if(!temp)
		return false;
	return (temp == state);
}

/**
 * arch_signal_suspend
 *
 * @brief This function is intended to be called before
 * cpu enters suspend state. By passing the state, we store
 * and use the value to check while executing resume routine.
 *
 * @param[in] state: Suspend state of cpu
 */
void arch_signal_suspend(cpu_sleep_t state)
{
	sleep_flag[arch_core_index()] = state;
}

/**
 * arch_signal_resume
 *
 * @brief This function is intended ot be called after
 * execution of resume routine so that state of cpu is
 * updated to resume.
 */
void arch_signal_resume(void)
{
	sleep_flag[arch_core_index()] = resume;
}

_WEAK void arch_panic_handler()
{
	const context_frame_t *frame = get_context_frame();
	syslog_stdout_enable();
	syslog(fail, "Arch Panic!\n");
	syslog(info, "Info:\nCause\t: 0x%x\t Address\t: 0x%x\n", frame->mcause, frame->mepc);
	while(1)
		arch_wfi();
}

_WEAK void arch_unhandled_irq()
{
	const context_frame_t *frame = get_context_frame();
	syslog_stdout_enable();
	syslog(fail, "Arch Unhandled IRQ!\n");
	syslog(info, "Info:\nIRQ ID\t: 0x%x\n", frame->mcause & ~(1U << 31));
	while(1)
		arch_wfi();
}

/**
 * arch_rseed_capture
 *
 * @brief This function is intended to capture unique seed value
 */
void arch_rseed_capture()
{
	extern uintptr_t *_bss_start;
	srand((size_t)_bss_start);
}
