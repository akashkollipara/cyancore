/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_tasks.c
 * Description		: CC OS Kernel tasks related definations definations
 * Primary Author	: Pranjal Chanda [pranjalchanda08@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*****************************************************
 *	INCLUDES
 *****************************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arch.h>
#include <visor/helios/helios.h>

/*****************************************************
 *	DEFINES
 *****************************************************/
#define HELIOS_PRIORITY_MAX 255

/*****************************************************
 *	INTERNAL EXTERNS FUNCTIONS
 *****************************************************/
extern status_t _helios_sched_insert_after(helios_sched_tcb_t **ptr, helios_sched_tcb_t *new_node, uint8_t link_type);
extern status_t _helios_sched_insert_before(helios_sched_tcb_t **ptr, helios_sched_tcb_t *new_node, uint8_t link_type);
extern void _helios_sched_send_to_wait(helios_sched_ctrl_t *sched_ctrl, helios_sched_tcb_t *ptr, const size_t ticks);
extern void _helios_sched_send_to_pause(helios_sched_ctrl_t * sched_ctrl, helios_sched_tcb_t * ptr);
extern void _helios_sched_send_to_resume(helios_sched_ctrl_t *sched_ctrl, helios_sched_tcb_t *ptr);
extern void _helios_sched_send_back_of_task_prio(helios_sched_tcb_t *node_ptr);
extern void _helios_pre_sched(helios_args args);
extern void _helios_scheduler_despatch(void);
/*****************************************************
 *	GLOBAL EXTERNS VARIABLES
 *****************************************************/
extern void _helios_idle_task_fn(void);
extern helios_sched_t g_helios_sched_list[];
extern helios_sched_ctrl_t g_sched_ctrl;

/*****************************************************
 *	GLOBAL DECLARATIONS
 *****************************************************/
#if HELIOS_DYNAMIC == false
extern helios_sched_tcb_t g_helios_tcb_list[];
#else
extern helios_sched_tcb_t *g_helios_tcb_list;
#endif

#if HELIOS_DYNAMIC == false
uint8_t _helios_stack[HELIOS_IDLE_TASK_STACK_LEN];
#else
uint8_t *_helios_stack = HELIOS_NULL_PTR;
#endif
helios_task_t helios_idle_task;
/*****************************************************
 *	STATIC VARIABLES
 *****************************************************/
static uint16_t __helios_task_id_gen = false;
/*****************************************************
 *	STATIC FUNCTION DEFINATIONS
 *****************************************************/
static void __helios_init_scheduler(void)
{
	g_sched_ctrl.cb_hooks_reg.pre_sched = &_helios_pre_sched;
	g_sched_ctrl.curr_task = g_sched_ctrl.ready_list_head;
	return;
}

static uint16_t __helios_task_id_generate()
{
	__helios_task_id_gen++;
	return __helios_task_id_gen;
}

static helios_sched_tcb_t * __helios_get_tcb_using_task_id(const uintptr_t task_id)
{
	helios_sched_tcb_t * ptr = g_sched_ctrl.ready_list_head;
	bool _is_id_found = false;
	while (ptr->ready_link.next != g_sched_ctrl.ready_list_head)
	{
		if (ptr->task_id == task_id)
		{
			_is_id_found = true;
			break;
		}
		ptr = ptr->ready_link.next;
	}
	if (_is_id_found == false)
	{
		ptr = HELIOS_NULL_PTR;
	}
	return ptr;
}

static status_t __helios_set_task_flag(helios_sched_tcb_t* helios_task, helios_task_flag_t task_flag, bool en)
{
	HELIOS_ASSERT_IF_FALSE( helios_task != HELIOS_NULL_PTR);

	if (en)
	{
		helios_task->task_flags |= (uint8_t)task_flag;
	}
	else
	{
		helios_task->task_flags &= (uint8_t)~task_flag;
	}
	return success;
}

static bool __helios_is_task_flag(const helios_sched_tcb_t *helios_task, helios_task_flag_t task_flag)
{
	if (helios_task != HELIOS_NULL_PTR)
	{
		return (helios_task->task_flags & task_flag) == false;
	}

	return false;
}

/*****************************************************
 *	USER FUNCTION DEFINATIONS
 *****************************************************/
status_t helios_add_task(
    helios_task_t *helios_task,
    const char *name,
    task_fn_t task_func,
    helios_args args,
    uint8_t priority,
    size_t stack_len,
#if HELIOS_DYNAMIC
    uintptr_t stack_ptr _UNUSED
#else
    uintptr_t stack_ptr
#endif /* HELIOS_DYNAMIC */
)
{
	HELIOS_ASSERT_IF_FALSE(*helios_task == (uintptr_t)HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(name != HELIOS_NULL_PTR);
#if HELIOS_DYNAMIC == false
	HELIOS_ASSERT_IF_FALSE(stack_ptr != (uintptr_t)HELIOS_NULL_PTR);
#endif
	HELIOS_ASSERT_IF_FALSE(task_func != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(stack_len != false);
	HELIOS_ASSERT_IF_FALSE(priority >= HELIOS_IDLE_TASK_PRIORITY);
	HELIOS_ASSERT_IF_FALSE(priority < HELIOS_PRIORITY_MAX);

	helios_pause_all_task();

	helios_sched_tcb_t *ptr = HELIOS_NULL_PTR;

#if HELIOS_DYNAMIC == false
	/* Static Task Allocation */
	for (size_t i = false; i < HELIOS_MAX_THREAD; i++)
	{
		/* Get an available node from global tcb list */
		if (g_helios_tcb_list[i].task_status == helios_sched_task_status_exit)
		{
			ptr = &(g_helios_tcb_list[i]);
			break;
		}
	}
	if (ptr != g_sched_ctrl.ready_list_head)
	{
		ptr->stack_ptr = stack_ptr;
#else
	/* Dynamic Task Declaration */
	ptr = (helios_sched_tcb_t *)helios_malloc(sizeof(helios_sched_tcb_t));
	if (ptr != HELIOS_NULL_PTR)
	{
		ptr->stack_ptr = (uintptr_t)helios_malloc(stack_len);
		if (ptr->stack_ptr == (uintptr_t)HELIOS_NULL_PTR)
		{
			helios_resume_all_task();
			HELIOS_ERR("Memory Low for STACK Pointer");
			return error_memory_low;
		}
#endif /* HELIOS_DYNAMIC */
	}
	else
	{
		helios_resume_all_task();
		HELIOS_ERR("Memory Low for Task Creation");
		return error_memory_low;
	}
	/* Fill tcb details */
	ptr->name = name;
	ptr->priority = priority;
	ptr->task_func = task_func;
	ptr->args_ptr = args;
#if HELIOS_ANTI_DEADLOCK
	ptr->task_wd_ticks = SIZE_MAX;
#endif /* HELIOS_ANTI_DEADLOCK */
	if (g_sched_ctrl.ready_list_head == HELIOS_NULL_PTR)
	{
		ptr->ready_link.next = ptr->ready_link.prev = g_sched_ctrl.ready_list_head = ptr;
	}
	/* Insert Tasks in assending order of its priority */
	_helios_sched_send_back_of_task_prio(ptr);
	ptr->task_status = helios_sched_task_status_ready;
	ptr->task_id = __helios_task_id_generate();
	*helios_task = ptr->task_id;
	helios_resume_all_task();
	return success;
}

helios_args helios_get_args(void)
{
	return g_sched_ctrl.curr_task->args_ptr;
}

status_t helios_del_task(helios_task_t helios_task)
{
	helios_sched_tcb_t * ptr = __helios_get_tcb_using_task_id(helios_task);

	if (ptr == HELIOS_NULL_PTR)
	{
		ptr = g_sched_ctrl.curr_task;
	}
	HELIOS_ASSERT_IF_FALSE(ptr->task_func != &_helios_idle_task_fn);
	/* Code to handle first node */
	if (ptr == g_sched_ctrl.ready_list_head)
	{
		/* IDLE Task can not be deleted */
		HELIOS_ERR("Trying to delete IDLE TASK");
		return error_os_invalid_op;
	}
	ptr->task_status = helios_sched_task_status_exit;

	if (ptr == g_sched_ctrl.curr_task)
	{
		helios_task_yield(); /* Yeild */
	}

	return success;
}

status_t helios_pause_task(helios_task_t helios_task)
{
	helios_sched_tcb_t *ptr = __helios_get_tcb_using_task_id(helios_task);
	if (ptr == HELIOS_NULL_PTR)
	{
		ptr = g_sched_ctrl.curr_task;
	}

	_helios_sched_send_to_pause(&g_sched_ctrl, ptr);

	return success;
}

status_t helios_pause_all_task(void)
{
	helios_sched_tcb_t *ptr = g_sched_ctrl.ready_list_head;
	if(ptr != HELIOS_NULL_PTR)
	{
		while (true)
		{
			if (ptr == g_sched_ctrl.curr_task)
			{
				/* Do not pause the current task */
				continue;
			}

			_helios_sched_send_to_pause(&g_sched_ctrl, ptr);
			ptr = ptr->ready_link.next;

			if(ptr == g_sched_ctrl.ready_list_head)
			{
				break;
			}
		}
	}

	return success;
}

status_t helios_resume_all_task(void)
{
	helios_sched_tcb_t *ptr = g_sched_ctrl.ready_list_head;
	if (ptr != HELIOS_NULL_PTR)
	{
		while (true)
		{
			ptr = ptr->ready_link.next;
			_helios_sched_send_to_resume(&g_sched_ctrl, ptr);
			if (ptr == g_sched_ctrl.ready_list_head)
			{
				break;
			}
		}
	}
	else
	{
		HELIOS_ERR("No Task added");
		return error_os_invalid_op;
	}

	return success;
}

status_t helios_resume_task(helios_task_t helios_task)
{
	helios_sched_tcb_t *ptr = __helios_get_tcb_using_task_id(helios_task);
	HELIOS_ASSERT_IF_FALSE(ptr != HELIOS_NULL_PTR);

	if (ptr->task_status != helios_sched_task_status_pause)
	{
		_helios_sched_send_to_resume(&g_sched_ctrl, ptr);
	}
	else
	{
		HELIOS_ERR("Invalid call for non-paused task");
		return error_os_invalid_op;
	}
	return success;
}

status_t helios_set_sched_algo(helios_sched_algo_t sched_algo)
{
	HELIOS_ASSERT_IF_FALSE(sched_algo != helios_sched_algo_max);

	g_sched_ctrl.selected_sched = &(g_helios_sched_list[sched_algo]);

	return success;
}

status_t helios_task_anti_deadlock_enable_and_feed(size_t task_wd_ticks _UNUSED)
{
#if HELIOS_ANTI_DEADLOCK
	HELIOS_ASSERT_IF_FALSE((task_wd_ticks > false) && (task_wd_ticks < SIZE_MAX));

	helios_sched_tcb_t *ptr = g_sched_ctrl.curr_task;
	if (__helios_is_task_flag(ptr, helios_task_flag_set_anti_deadlock))
	{
		__helios_set_task_flag(ptr, helios_task_flag_set_anti_deadlock, true);
	}

	ptr->task_wd_ticks = task_wd_ticks;
	return success;
#else
	helios_sched_tcb_t *ptr = g_sched_ctrl.curr_task;
	if (__helios_is_task_flag(ptr, helios_task_flag_set_anti_deadlock))
	{
		__helios_set_task_flag(ptr, helios_task_flag_set_anti_deadlock, false);
	}
	HELIOS_ERR("Enable HELIOS_ANTI_DEADLOCK Flag in build");
	return error_func_inval;
#endif /* HELIOS_ANTI_DEADLOCK */
}

status_t helios_task_anti_deadlock_disable(void)
{
	helios_sched_tcb_t *ptr = g_sched_ctrl.curr_task;

	if (__helios_is_task_flag(ptr, helios_task_flag_set_anti_deadlock))
	{
		__helios_set_task_flag(ptr, helios_task_flag_set_anti_deadlock, false);
	}

#if HELIOS_ANTI_DEADLOCK
	ptr->task_wd_ticks = SIZE_MAX;
	return success;
#else
	HELIOS_ERR("Enable HELIOS_ANTI_DEADLOCK Flag in build");
	return error_func_inval;
#endif /* HELIOS_ANTI_DEADLOCK */
}
status_t helios_set_callback(const helios_sched_cb_t cb_type, const helios_cb_hook_t cb_func _UNUSED)
{
	status_t ret = success;
	switch (cb_type)
	{
	case helios_sched_cb_power_post_sleep:
#if HELIOS_POWER_SAVE_EN
		g_sched_ctrl.cb_hooks_reg.post_sleep_cb = cb_func;
		break;
#else
		HELIOS_ERR("Enable HELIOS_POWER_SAVE_EN Flag in build");
		ret = error_func_inval;
		break;
#endif
	case helios_sched_cb_power_pre_sleep:
#if HELIOS_POWER_SAVE_EN
		g_sched_ctrl.cb_hooks_reg.pre_sleep_cb = cb_func;
		break;
#else
		HELIOS_ERR("Enable HELIOS_POWER_SAVE_EN Flag in build");
		ret = error_func_inval;
		break;
#endif
	case helios_sched_cb_power_sleep:
#if HELIOS_POWER_SAVE_EN
		g_sched_ctrl.cb_hooks_reg.power_sleep = cb_func;
		break;
#else
		HELIOS_ERR("Enable HELIOS_POWER_SAVE_EN Flag in build");
		ret = error_func_inval;
		break;
#endif
	case helios_sched_cb_deadlock_notify:
#if HELIOS_ANTI_DEADLOCK
		g_sched_ctrl.cb_hooks_reg.deadlock_notify = cb_func;
		break;
#else
		HELIOS_ERR("Enable HELIOS_ANTI_DEADLOCK Flag in build");
		ret = error_func_inval;
		break;
#endif
	case helios_sched_cb_max:
		HELIOS_ERR("Call for invalid callback registration");
		ret = error_func_inval;
		break;
	}
	return ret;
}

const char *helios_get_curr_task_name(void)
{
	return g_sched_ctrl.curr_task->name;
}

const char *helios_get_task_name(helios_task_t helios_task)
{
	const helios_sched_tcb_t *ptr = __helios_get_tcb_using_task_id(helios_task);
	if(ptr != HELIOS_NULL_PTR)
	{
		return ptr->name;
	}
	return HELIOS_NULL_PTR;
}

void helios_task_wait(const size_t ticks)
{
	helios_sched_tcb_t *ptr = g_sched_ctrl.curr_task;

	if (ticks > false)
	{
		_helios_sched_send_to_wait(&g_sched_ctrl, ptr, ticks);
	}

	helios_task_yield();
}

void helios_task_yield()
{
	_helios_scheduler_despatch();
	return;
}

 _NORETURN void helios_run(void)
{
	/* OS Init code */
	/* Initialise IDLE Task */
	helios_add_task(&helios_idle_task,
		       HELIOS_IDLE_TASK_NAME,
		       &_helios_idle_task_fn,
		       (helios_args)&g_sched_ctrl,
		       HELIOS_IDLE_TASK_PRIORITY,
		       HELIOS_IDLE_TASK_STACK_LEN,
		       (uintptr_t)_helios_stack);

	/* Initialise scheduler */
	__helios_init_scheduler();
	HELIOS_DBG("Starting HELIOS");
	helios_task_yield(); /* Yeild */
	HELIOS_SCHED_PANIC(error_os_panic_os_start_fail);
	while (true)
	{
		/* Code shall not reach here */
		arch_wfi();
	}
}
