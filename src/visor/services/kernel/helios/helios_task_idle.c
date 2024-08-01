/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_task_idle.c
 * Description		: CC OS IDLE Task definitions
 * Primary Author	: Pranjal Chanda [pranjalchanda08@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*****************************************************
 *	INCLUDES
 *****************************************************/
#include <visor/helios/helios.h>
#include <arch.h>

/*****************************************************
 *	EXTERN FUNCTION DECLARATION
 *****************************************************/
extern status_t _helios_sched_node_detach(helios_sched_tcb_t *node_ptr, uint8_t link_type);

/*****************************************************
 *	STATIC FUNCTION DECLARATION
 *****************************************************/
/**
 * @brief	This function cleans up the terminated task form the TCB list
 *
 * @param  ptr[in]		Pointer to the TCB being cleaned
 * @return helios_sched_tcb_t *	Pointer to the next TCB
 */
static helios_sched_tcb_t * __free_terminated_task(helios_sched_tcb_t * ptr)
{
	helios_sched_tcb_t * next_ptr = ptr->ready_link.next;
	if (ptr->ready_link.next->task_status == helios_sched_task_status_exit)
	{
		_helios_sched_node_detach(ptr, false);

#if HELIOS_DYNAMIC == true
		helios_free((void *)ptr->stack_ptr);
		helios_free(ptr);
#endif
	}

	return next_ptr;
}

#if HELIOS_POWER_SAVE_EN
static void __helios_power_save_callback(void)
{
	arch_wfi();
}
#endif
/*****************************************************
 *	USER FUNCTION DEFINATION
 *****************************************************/
_NORETURN void _helios_idle_task_fn(void)
{
	static helios_sched_tcb_t * ptr  = HELIOS_NULL_PTR;
	helios_sched_ctrl_t * sched_ctrl = (helios_sched_ctrl_t *) helios_get_args();
	ptr = sched_ctrl->ready_list_head;
#if HELIOS_POWER_SAVE_EN
	if (sched_ctrl->cb_hooks_reg.sleep_cb == HELIOS_NULL_PTR)
	{
		sched_ctrl->cb_hooks_reg.sleep_cb = __helios_power_save_callback;
	}
#endif
	while (true)
	{
		/* Clean up task if terminated */
		ptr = __free_terminated_task(ptr);

#if HELIOS_POWER_SAVE_EN
		/* Power Save code */
		if (sched_ctrl->cb_hooks_reg.pre_sleep_cb != HELIOS_NULL_PTR)
		{
			sched_ctrl->cb_hooks_reg.pre_sleep_cb();
		}
		if (sched_ctrl->cb_hooks_reg.sleep_cb != HELIOS_NULL_PTR)
		{
			sched_ctrl->cb_hooks_reg.sleep_cb();
		}
		if (sched_ctrl->cb_hooks_reg.post_sleep_cb != HELIOS_NULL_PTR)
		{
			sched_ctrl->cb_hooks_reg.post_sleep_cb();
		}
#endif
		/* Yield for next available task */
		helios_task_yield();
	}
}
