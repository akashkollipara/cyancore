/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_sched.c
 * Description		: CC OS Kernel scheduler definations
 * Primary Author	: Pranjal Chanda [pranjalchanda08@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*****************************************************
 *	INCLUDES
 *****************************************************/
#include <visor/helios/utils/helios_sched.h>
#include <platform.h>
#include <syslog.h>

/*****************************************************
 *	DEFINES
 *****************************************************/
#define CC_SCHED_ALGO(_id, _fn) {	\
	.helios_selected_algo = _id,	\
	.algo_function = _fn}

#define HELIOS_LINK_WAIT		true
#define HELIOS_LINK_READY		false

/*****************************************************
 *	STATIC FUNCTION DECLARATION
 *****************************************************/
static void __helios_sched_deadlock_adjustment_and_detection(const helios_sched_ctrl_t * sched_ctrl);
static void __helios_sched_wait_list_adjustment(helios_sched_ctrl_t * sched_ctrl);
static void __helios_sched_algo_round_robin_fn(helios_sched_ctrl_t * sched_ctrl);
static void __helios_sched_algo_priority_driven_fn(helios_sched_ctrl_t * sched_ctrl);

/*****************************************************
 *	GLOBAL DECLARATIONS
 *****************************************************/
#if !HELIOS_USE_DYNAMIC
helios_sched_tcb_t g_helios_tcb_list [HELIOS_MAX_THREAD];
#else
helios_sched_tcb_t * g_helios_tcb_list	= HELIOS_NULL_PTR;
#endif

helios_sched_t g_helios_sched_list [] =
{
	CC_SCHED_ALGO(helios_sched_algo_round_robin, __helios_sched_algo_round_robin_fn),
	CC_SCHED_ALGO(helios_sched_algo_priority_driven, __helios_sched_algo_priority_driven_fn),
};

helios_sched_ctrl_t g_sched_ctrl =
{
#if !HELIOS_USE_DYNAMIC
	.ready_list_head 	= &(g_helios_tcb_list[false]),
	.curr_task 		= &(g_helios_tcb_list[false]),
#else
	.ready_list_head 	= HELIOS_NULL_PTR,
	.curr_task 		= HELIOS_NULL_PTR,
#endif
	.wait_list_head		= HELIOS_NULL_PTR,
	.selected_sched		= &(g_helios_sched_list[helios_sched_algo_round_robin])
};

/*****************************************************
 *	INTERNAL USED FUNCTIONS (NON-STATIC)
 *****************************************************/
/**
 * @brief	Insert a node after a reference pointer
 *
 * @param	ptr[in_out]	Reference pointer address
 * @param	new_node[in]	Node to be inserted
 * @param 	link_type[in]	HELIOS_LINK_READY / HELIOS_LINK_WAIT
 *
 * @return	None
*/
status_t _helios_sched_insert_after(helios_sched_tcb_t ** ptr, helios_sched_tcb_t * new_node, uint8_t link_type)
{
	HELIOS_ASSERT_IF_FALSE(new_node != HELIOS_NULL_PTR);
	if (link_type == HELIOS_LINK_WAIT)
	{
		/* Wait Link */
		if (*ptr == HELIOS_NULL_PTR)
		{
			*ptr = new_node;
			new_node->wait_link.next = new_node;
			new_node->wait_link.prev = new_node;
		}
		else
		{
			new_node->wait_link.next = (*ptr)->wait_link.next;
			new_node->wait_link.prev = *ptr;
			new_node->wait_link.next->wait_link.prev = new_node;
			(*ptr)->wait_link.next = new_node;
		}
	}
	else
	{
		/* Ready Link */
		if (*ptr == HELIOS_NULL_PTR)
		{
			*ptr = new_node;
			new_node->ready_link.next = new_node;
			new_node->ready_link.prev = new_node;
		}
		else
		{
			new_node->ready_link.next = (*ptr)->ready_link.next;
			new_node->ready_link.prev = *ptr;
			new_node->ready_link.next->ready_link.prev = new_node;
			(*ptr)->ready_link.next = new_node;
		}
	}
	return success;
}

/**
 * @brief	Insert a node before a reference pointer
 *
 * @param	ptr[in_out]	Reference pointer address
 * @param	new_node[in]	Node to be inserted
 * @param 	link_type[in]	HELIOS_LINK_READY / HELIOS_LINK_WAIT
 *
 * @return	None
*/
status_t _helios_sched_insert_before(helios_sched_tcb_t ** ptr, helios_sched_tcb_t * new_node, uint8_t link_type)
{
	HELIOS_ASSERT_IF_FALSE(ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(new_node != HELIOS_NULL_PTR);

	if (link_type == HELIOS_LINK_WAIT)
	{
		/* Wait Link */
		if (*ptr == HELIOS_NULL_PTR)
		{
			*ptr = new_node;
			new_node->wait_link.next = new_node;
			new_node->wait_link.prev = new_node;
		}
		else
		{
			new_node->wait_link.next = *ptr;
			new_node->wait_link.prev = (*ptr)->wait_link.prev;
			(*ptr)->wait_link.prev = new_node;
			new_node->wait_link.prev->wait_link.next = new_node;
		}
	}
	else
	{
		/* Ready Link */
		if (*ptr == HELIOS_NULL_PTR)
		{
			*ptr = new_node;
			new_node->ready_link.next = new_node;
			new_node->ready_link.prev = new_node;
		}
		else
		{
			new_node->ready_link.next = *ptr;
			new_node->ready_link.prev = (*ptr)->ready_link.prev;
			(*ptr)->ready_link.prev = new_node;
			new_node->ready_link.prev->ready_link.next = new_node;
		}
	}
	return success;
}

/**
 * @brief	A function to detach an existing node from a link
 *
 * @param	node_ptr[in_out]	Address of the node to be detached
 * @param	link_type[in]		HELIOS_LINK_READY / HELIOS_LINK_WAIT
 *
 * @return	None
 */
status_t _helios_sched_node_detach(helios_sched_tcb_t *node_ptr, uint8_t link_type)
{
	HELIOS_ASSERT_IF_FALSE(node_ptr != HELIOS_NULL_PTR);

	if (link_type == HELIOS_LINK_WAIT)
	{
		/* Wait Link */
		if (node_ptr == g_sched_ctrl.wait_list_head)
		{
			g_sched_ctrl.wait_list_head = node_ptr->ready_link.next;
		}
		node_ptr->wait_link.prev->wait_link.next = node_ptr->wait_link.next;
		node_ptr->wait_link.next->wait_link.prev = node_ptr->wait_link.prev;
		node_ptr->wait_link.prev = node_ptr->wait_link.next = HELIOS_NULL_PTR;
	}
	else
	{
		if (node_ptr == g_sched_ctrl.ready_list_head)
		{
			g_sched_ctrl.ready_list_head = node_ptr->ready_link.next;
		}
		/* Ready Link */
		node_ptr->ready_link.prev->ready_link.next = node_ptr->ready_link.next;
		node_ptr->ready_link.next->ready_link.prev = node_ptr->ready_link.prev;
		node_ptr->ready_link.prev = node_ptr->ready_link.next = HELIOS_NULL_PTR;
	}
	return success;
}

/**
 * @brief	Send a task to wait state
 *
 * @param	sched_ctrl[in_out]	Current scheduler control
 * @param	ptr[in_out]		Pointer to task TCB
 * @param	ticks[in]		Ticks to wait
 *
 * @return	None
 */
void _helios_sched_send_to_wait(helios_sched_ctrl_t * sched_ctrl, helios_sched_tcb_t * ptr, const size_t ticks)
{
	if (ptr->task_status == helios_sched_task_status_wait)
	{
		return;
	}
	if(_helios_sched_insert_before(&(sched_ctrl->wait_list_head), ptr, HELIOS_LINK_WAIT) == success)
	{
		ptr->wait_res.task_delay_ticks = ticks;
		ptr->task_status = helios_sched_task_status_wait;
	}
}

/**
 * @brief	Send a task to pause state
 *
 * @param	sched_ctrl[in_out]	Current scheduler control
 * @param	ptr[in_out]		Pointer to task TCB
 *
 * @return	None
 */
void _helios_sched_send_to_pause(helios_sched_ctrl_t * sched_ctrl, helios_sched_tcb_t * ptr)
{
	if (ptr->task_status == helios_sched_task_status_pause)
	{
		return;
	}
	if(_helios_sched_insert_before(&(sched_ctrl->wait_list_head), ptr, HELIOS_LINK_WAIT) == success)
	{
		ptr->wait_res.task_delay_ticks = HELIOS_DELAY_MAX;
		ptr->task_status = helios_sched_task_status_pause;
	}
}

/**
 * @brief	Send a task to ready state
 *
 * @param	sched_ctrl[in_out]	Current scheduler control
 * @param	ptr[in_out]		Pointer to task TCB
 *
 * @return	None
 */
void _helios_sched_send_to_resume(helios_sched_ctrl_t * sched_ctrl, helios_sched_tcb_t * ptr)
{
	if (ptr->task_status < helios_sched_task_status_wait)
	{
		return;
	}

	if (ptr == sched_ctrl->wait_list_head)
	{
		/* First in the list */
		sched_ctrl->wait_list_head = ptr->wait_link.next;
		if (ptr->wait_link.next == ptr && ptr->wait_link.prev == ptr)
		{
			/* Last Wait task left */
			sched_ctrl->wait_list_head = HELIOS_NULL_PTR;
		}
	}
	_helios_sched_node_detach(ptr, HELIOS_LINK_WAIT);
	ptr->wait_res.task_delay_ticks = false;
	ptr->task_status = helios_sched_task_status_ready;
}

/**
 * @brief	Send a task to the back of its own priority list
 *
 * @param	node_ptr[in_out]	Pointer to task TCB
 *
 * @return	None
 */
status_t _helios_sched_send_back_of_task_prio(helios_sched_tcb_t *node_ptr)
{
	HELIOS_ASSERT_IF_FALSE(node_ptr != HELIOS_NULL_PTR);

	helios_sched_tcb_t * ref_ptr = g_sched_ctrl.ready_list_head;
	if(ref_ptr != HELIOS_NULL_PTR)
	{
		if (node_ptr == ref_ptr)
		{
			/* Very first node */
			return success;
		}

		bool least_prio = true;
		/* Find an optimal space */
		while(node_ptr->priority >= ref_ptr->priority)
		{
			least_prio = false;
			ref_ptr = ref_ptr->ready_link.next;
			if (ref_ptr == g_sched_ctrl.ready_list_head)
			{
				/* traversal complete */
				break;
			}
		}
		if (least_prio)
		{
			/* Set ready list head if least prio detected */
			g_sched_ctrl.ready_list_head = node_ptr;
		}
		if(node_ptr->ready_link.next == node_ptr->ready_link.prev)
		{
			/**
			 * New Node
			 * Insert it behind the same priority
			 */
			_helios_sched_insert_before(&ref_ptr, node_ptr, HELIOS_LINK_READY);
		}
		else
		{
			/**
			 * Detach the current node from link and insert it at the end
			 * of priority
			 * This would help to round robin over the same priority when
			 * scheduling
			 */
			_helios_sched_node_detach(node_ptr, HELIOS_LINK_READY);
			_helios_sched_insert_before(&ref_ptr, node_ptr, HELIOS_LINK_READY);
		}
	}
	return success;
}

/**
 * @brief	HELIOS pre scheduler callback being called before any task scheduling is done
 *
 * @param	args[in]	helios_args (provides scheduler ctrl)
 *
 * @return	None
 */
void _helios_pre_sched(helios_args args)
{
	helios_sched_ctrl_t * sched_ctrl = (helios_sched_ctrl_t *) args;
	sched_ctrl->curr_task->task_status = helios_sched_task_status_ready;
	__helios_sched_wait_list_adjustment(sched_ctrl);
	__helios_sched_deadlock_adjustment_and_detection(sched_ctrl);
	_helios_sched_send_back_of_task_prio(sched_ctrl->curr_task);
}

/**
 * @brief	Despatch a scheduling as per selected algorithm
 *
 * @return	None
 */
void _helios_scheduler_despatch(void)
{
	if (g_sched_ctrl.cb_hooks_reg.pre_sched != NULL)
	{
		/* Call Pre_sched Function */
		g_sched_ctrl.cb_hooks_reg.pre_sched((helios_args) &g_sched_ctrl);
	}
	else
	{
		/* A System Error case */
		HELIOS_SCHED_PANIC(error_os_panic_presched_cb_null);
	}

	if (g_sched_ctrl.selected_sched != NULL)
	{
		/* Call the scheduler */
		g_sched_ctrl.selected_sched->algo_function(&g_sched_ctrl);
	}
	else
	{
		/* A System Error case */
		HELIOS_SCHED_PANIC(error_os_panic_sched_algo_null);
	}

}
/*****************************************************
 *	STATIC FUNCTION DEFINATIONS
 *****************************************************/
static void __helios_sched_context_switch(helios_sched_tcb_t * next_task)
{
	next_task->task_status = helios_sched_task_status_running;
}

static void __helios_sched_deadlock_adjustment_and_detection(const helios_sched_ctrl_t * sched_ctrl _UNUSED)
{
#if HELIOS_ANTI_DEADLOCK
	helios_sched_tcb_t * ptr = sched_ctrl->ready_list_head;
	static helios_sched_anti_deadlock_t anti_deadlock_notify;
	while (ptr != HELIOS_NULL_PTR)
	{
		if (ptr->task_status != helios_sched_task_status_pause)
		{
			ptr->task_wd_ticks--;
		}
		if ((ptr->task_wd_ticks == false) && (sched_ctrl->cb_hooks_reg.deadlock_notify != HELIOS_NULL_PTR))
		{
			/* Create notification params */
			anti_deadlock_notify.name = ptr->name;
			anti_deadlock_notify.task_func = ptr->task_func;

			if (ptr->task_status != helios_sched_task_status_exit)
			{
				ptr->task_status = helios_sched_task_status_exit;
			}
			/* Notify the user that the task pointed by ptr is dead and has been terminated */
			sched_ctrl->cb_hooks_reg.deadlock_notify((helios_args) &anti_deadlock_notify);
		}
		if (ptr->ready_link.next == sched_ctrl->ready_list_head)
		{
			break;
		}
	}
#else
	return;
#endif /* HELIOS_ANTI_DEADLOCK */
}
static void __helios_sched_wait_list_adjustment(helios_sched_ctrl_t * sched_ctrl)
{
	helios_sched_tcb_t * ptr = sched_ctrl->wait_list_head;
	if (ptr == HELIOS_NULL_PTR)
	{
		return;
	}

	const int * wait_res = (int *)ptr->wait_res.wait_on_resource;
	while(ptr != HELIOS_NULL_PTR)
	{
		if (ptr->task_status == helios_sched_task_status_wait)
		{
			ptr->wait_res.task_delay_ticks--;	/* Tick caliberations required */

			if ((wait_res != HELIOS_NULL_PTR) && *wait_res > false)
			{
				/* The resource is available can can go to ready state */
				ptr->wait_res.task_delay_ticks = false;
				ptr->wait_res.wait_on_resource = false;
			}
			if(ptr->wait_res.task_delay_ticks == false)
			{
				_helios_sched_send_to_resume(sched_ctrl, ptr);
			}
			if (ptr->wait_link.next == sched_ctrl->wait_list_head)
			{
				break;
			}
			else
			{
				ptr = ptr->wait_link.next;
			}
		}
	}
}

/*****************************************************
 *	SCHEDULER ALGORITHMS
 *****************************************************/
static void __helios_sched_algo_round_robin_fn(helios_sched_ctrl_t * sched_ctrl)
{
	/* do waitlist adjustment */
	helios_sched_tcb_t * ptr = sched_ctrl->curr_task->ready_link.next;

	if (ptr == sched_ctrl->ready_list_head)
	{
		/* IDLE Task */
		_helios_sched_send_to_resume(&g_sched_ctrl, ptr);
	}
	/* Context switch to next task */
	if (ptr->task_status == helios_sched_task_status_ready)
	{
		__helios_sched_context_switch(ptr->ready_link.next);
	}
}

static void __helios_sched_algo_priority_driven_fn(helios_sched_ctrl_t * sched_ctrl)
{
	/* do waitlist adjustment */
	helios_sched_tcb_t * ptr = sched_ctrl->ready_list_head->ready_link.prev;
	if(ptr != HELIOS_NULL_PTR)
	{
		while (ptr->task_status != helios_sched_task_status_ready)
		{
			ptr = ptr->wait_link.prev;
			if (ptr == sched_ctrl->ready_list_head)
			{
				/* IDLE Task */
				_helios_sched_send_to_resume(&g_sched_ctrl, ptr);
				break;
			}
		}
		__helios_sched_context_switch(ptr);
	}
}
