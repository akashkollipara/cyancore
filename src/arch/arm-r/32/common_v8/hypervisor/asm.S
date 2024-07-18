/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: asm.S
 * Description		: This file consists of all the function written in asm
 *			  like ISR, context management, zero_reg and panic handler
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <asm.inc>
/* This need updation */
.macro PROLOGUE
	addi	sp, sp, -96	/* - size(context_frame) * 4 */
	sw	t6, 68(sp)
	sw	t5, 64(sp)
	sw	t4, 60(sp)
	sw	t3, 56(sp)
	sw	a7, 52(sp)
	sw	a6, 48(sp)
	sw	a5, 44(sp)
	sw	a4, 40(sp)
	sw	a3, 36(sp)
	sw	a2, 32(sp)
	sw	a1, 28(sp)
	sw	a0, 24(sp)
	sw	t2, 20(sp)
	sw	t1, 16(sp)
	sw	t0, 12(sp)
	sw	ra, 8(sp)
	csrr	t0, mstatus
	sw	t0, 4(sp)
	csrr	t0, mepc
	sw	t0, 0(sp)
.endm

.macro EPILOGUE
	lw	t0, 0(sp)
	csrw	mepc, t0
	lw	t0, 4(sp)
	csrw	mstatus, t0
	lw	ra, 8(sp)
	lw	t0, 12(sp)
	lw	t1, 16(sp)
	lw	t2, 20(sp)
	lw	a0, 24(sp)
	lw	a1, 28(sp)
	lw	a2, 32(sp)
	lw	a3, 36(sp)
	lw	a4, 40(sp)
	lw	a5, 44(sp)
	lw	a6, 48(sp)
	lw	a7, 52(sp)
	lw	t3, 56(sp)
	lw	t4, 60(sp)
	lw	t5, 64(sp)
	lw	t6, 68(sp)
	addi	sp, sp, 96	/* size(context_frame) * 4 */
.endm

function exception_handler_base
.p2align 2			/* Needs 4Byte Alignment, refer mtvec reg */
	PROLOGUE
	csrr	a0, mcause
	sw	a0, 72(sp)
	mv	a1, sp
	call	exception_handler
	EPILOGUE
	mret

#ifndef RV_VEC_MODE
function riscv_update_vector
	la	a0, exception_handler_base
	csrw	mtvec, a0
	mv	a0, zero
	fence.i
	ret
#else
vectors vtors
.rept 32
	j	exception_handler_base
.endr
#endif
