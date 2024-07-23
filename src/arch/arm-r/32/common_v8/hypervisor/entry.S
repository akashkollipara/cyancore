/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: entry.S
 * Description		: This file consists of entry point of the framework.
 *			  After the reset of system, program in this file gets
 *			  executed.
 * Primary Author	: Debadarshana Parida [debadarshanaparida@yahoo.com}
 * Organisation		: Cyancore Core-Team
 */

#include <asm.inc>

/*
* Cortesx-R AArch-32 have 3 exception state (EL0, EL1, EL2)
* Cortex- R will boot with the Hypervisor mode( EL2 Mode)
* This require to have the EL2 Rest Handle and Exception Vectr to be configured
*/
function entry
INIT_SET_UP:
	/* check the CPUID, branch to Primary CPU (cpu0) */
	MRC p15, 0, r0, c0, c0, 5       // Read MPIDR
	ANDS r0, r0, #0xF
    	BEQ PRIMARY_CPU
LOOP:
	B LOOP
PRIMARY_CPU:
	/* Set Up the MPU Memory Register */
	/* et set up the MPU and other required register in the early setup */


	/*Set up the Stack Memory For the HYP Mode */
	BL SET_HYP_SP



	/* As the stack is created we can call the C function now */
	/* Jump to Init function */
	B init
SET_HYP_SP:
	LDR r0,0x4000 //HYP_STACK_ADD -> need to be updated in thelinker file. Hard coded for the time being
	MSR sp_hyp, r0
    	MOV pc, lr
