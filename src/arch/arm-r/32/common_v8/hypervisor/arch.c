/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: arch.c
 * Description		: This file consists of architecture specific function that
 *			  cannot be inlined. Hence, present in c file.
 * Primary Author	: Mayuri Lokhande [mayurilokhande01@gmail.com],
 *			  Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <status.h>
#include <syslog.h>
#include <arch.h>
#include <interrupt.h>
#include <visor/workers.h>
#include <rand.h>
#include <lock/lock.h>

/**
 * arch_vcall_handler
 *
 * @brief This function handles svc calls. In Cyancore all exception calls
 * will be called as visor-calls.
 */
static void arch_vcall_handler()
{
	context_frame_t *frame = get_context_frame();
	vret_t vres;
	vcall_handler(frame->r0, frame->r1, frame->r2, frame->r3, &vres);
	frame->r0 = vres.p;
	frame->r1 = vres.size;
	frame->r2 = vres.status;
	return;
}

/* This the key for boot lock */
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
 *arch_early_setup - This function is called in the early stages of boot
 *
 * @brief This function is respinsible to clean reset cpu status/control registers.
 *
 */
void arch_early_setup()
{
	arch_di();
	/* Set the MPU registers*/
}

/**
 * arch_setup - This function is called after initial setup is done
 *
 * @brief This function is called after initial setup is done.
 */
void arch_setup()
{
	link_interrupt(int_arch, 10 ,&arch_vcall_handler);
	return;
}

void arch_di_save_state(istate_t *istate _UNUSED)
{
}

void arch_ei_restore_state(istate_t *istate _UNUSED)
{

}

/**
 * arch_core_index - Returns code index
 */

_WEAK unsigned int arch_core_index()
{

	unsigned int id = 0;
	asm volatile("mrc p15 0, %0, c0, c0, 0" : "=r" (id));

	return id;

}

void _NORETURN arch_panic_handler_callback()
{
	context_frame_t *frame;
	frame = get_context_frame();
	if(!frame)
		goto panic;
	syslog_stdout_enable();
	sysdbg("r0=%p\tr1=%p\tr2=%p\tr3=%p\n",
		frame->r0, frame->r1, frame->r2, frame->r3);
	sysdbg("r12=%p\tLR=%p\tPSR=%p\tReturns To=%p\n",
		frame->r12, frame->lr, frame->psr, frame->ret_addr);
#if DEBUG==0
	syslog(info, "PSR=%p\n", frame->psr);
#endif
panic:
	while(1) arch_wfi();
}

static cpu_sleep_t sleep_flag;

/**
 * arch_suspended_state_was
 *
 * @brief This function checks for the suspended state
 * and returns true or flase based on arg.
 *
 * @param[in] state: suspended state
 * @return bool: True/False
 */

bool arch_suspended_state_was(cpu_sleep_t state)
{
	assert(state != resume);
	if(!sleep_flag)
		return false;
	return (sleep_flag == state);
}

/**
 * arch_signal_suspend
 *
 * @brief This function is intended to be called before cpu enters
 * suspend state. By passing the state, we store and use to check while
 * exiting resume routine.
 *
 * @param[in] state: Suspend state of cpu
 */
void arch_signal_suspend(cpu_sleep_t state)
{
	sleep_flag = state;
}

/**
 * @brief This function signals resume of cpu. It is intended
 * to be called after exiting resume routine.
 */
void arch_signal_resume(void)
{
	sleep_flag = resume;
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

_WEAK void arch_panic_handler()
{
	syslog_stdout_enable();
	syslog(fail, "Arch Panic!\n");
	arch_panic_handler_callback();
}

_WEAK void arch_unhandled_irq()
{
	syslog_stdout_enable();
	syslog(fail, "Arch Unhandled IRQ!\n");
	arch_panic_handler_callback();
}
