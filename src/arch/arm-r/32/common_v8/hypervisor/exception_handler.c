/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: exception_handler.c
 * Description		: This file consists of sources for exception
 *			  handling.
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <status.h>
#include <assert.h>
#include <arch.h>
#include <mmio.h>

static void (* exhandler[N_CORES][N_EXCEP])(void) = {{[0 ... N_EXCEP-1] = arch_panic_handler}};
static void (* irqhandler[N_CORES][N_IRQ])(void) = {{[0 ... N_IRQ-1] = arch_unhandled_irq}};
static context_frame_t *local_frame[N_CORES];

bool in_isr(void)
{
	unsigned int cpuid = arch_core_index();
	return (local_frame[cpuid] != NULL) ? true : false;
}

static void set_context_frame(context_frame_t *frame)
{
	unsigned int cpuid = arch_core_index();
	local_frame[cpuid] = frame;
	fence(w, w);
}

context_frame_t *get_context_frame(void)
{
	unsigned int cpuid = arch_core_index();
	return local_frame[cpuid];
}

void arch_register_interrupt_handler(unsigned int id, void (*handler)(void))
{
	unsigned int cpuid = arch_core_index();
	assert(id < N_EXCEP);
	exhandler[cpuid][id] = handler;
	fence(ow, ow);
}

void local_register_interrupt_handler(unsigned int id, void (*handler)(void))
{
	unsigned int cpuid = arch_core_index();
	assert(id < N_IRQ);
	irqhandler[cpuid][id] = handler;
	fence(ow, ow);
}

void exception_handler(uint32_t mcause, context_frame_t *frame)
{
	unsigned int cause = mcause & ~(1U << 31);
	unsigned int cpuid = arch_core_index();

	set_context_frame(frame);

	if(mcause & (1U << 31))
		irqhandler[cpuid][cause]();
	else
	{
		exhandler[cpuid][cause]();
		frame->mepc += (MMIO8(frame->mepc) & 0x2) ? 4 : 2;
	}
	set_context_frame(NULL);
	fence(ow, ow);
}
