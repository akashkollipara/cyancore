/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_sem.c
 * Description		: CC OS Semaphore function definations
 * Primary Author	: Pranjal Chanda [pranjalchandaHELIOS_FALSE8@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

/*****************************************************
 *	INCLUDES
 *****************************************************/
#include <visor/helios/helios.h>

/*****************************************************
 *	GLOBAL/STATIC VARIABLE DECLARATIONS
 *****************************************************/

/*****************************************************
 *	GLOBAL EXTERNS
 *****************************************************/
extern helios_sched_ctrl_t g_sched_ctrl;
/*****************************************************
 *	STATIC FUNCTION DEFINATIONS
 *****************************************************/

/*****************************************************
 *	USER FUNCTION DEFINATIONS
 *****************************************************/

status_t helios_sem_create (helios_sem_t ** sem_ptr, size_t init_val)
{
#if HELIOS_DYNAMIC == false
	HELIOS_ASSERT_IF_FALSE((*sem_ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE((*sem_ptr)->sem_init == false);
#else
	HELIOS_ASSERT_IF_FALSE(*sem_ptr == HELIOS_NULL_PTR);
	/* Create semaphore */
	*sem_ptr = helios_malloc(sizeof(helios_sem_t));
	if (*sem_ptr == HELIOS_NULL_PTR)
	{
		HELIOS_ERR("Memory low for sem create");
		return error_memory_low;
	}
#endif 	/* HELIOS_DYNAMIC */
	/* Fill up the init details */
	(*sem_ptr)->sem_val = init_val;
	(*sem_ptr)->sem_init = true;

	return success;
}
status_t helios_sem_take (helios_sem_t * sem_ptr, size_t wait_ticks)
{
	HELIOS_ASSERT_IF_FALSE(sem_ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(sem_ptr->sem_init != false);

	if (sem_ptr->sem_val == false)
	{	/* If sem already locked */
		if (wait_ticks == false) 	/* ||_IS_ISR */
		{
			/* If Thread can not wait on resource to get unlocked */
			HELIOS_ERR("Semaphore already locked");
			return error_os_sem_get;
		}
		else
		{
			/* Set the TCB to point to locked resource */
			g_sched_ctrl.curr_task->wait_res.wait_on_resource = (uintptr_t) sem_ptr;
			/* Send to wait state */
			helios_task_wait(wait_ticks);
		}
	}
	else
	{
		/* Else Decrement semaphore value */
		sem_ptr->sem_val--;
	}
	return success;
}
status_t helios_sem_give (helios_sem_t * sem_ptr)
{
	HELIOS_ASSERT_IF_FALSE(sem_ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(sem_ptr->sem_init != false);

	/* Increment Semaphore value */
	sem_ptr->sem_val++;

	return success;
}
status_t helios_sem_delete (helios_sem_t ** sem_ptr)
{
	HELIOS_ASSERT_IF_FALSE(sem_ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE((*sem_ptr)->sem_init != false);

	(*sem_ptr)->sem_init = false;

#if HELIOS_DYNAMIC == true
	/* Free up allocated space */
	helios_free(*sem_ptr);
#endif

	return success;
}
status_t helios_sem_get_val (const helios_sem_t * sem_ptr, size_t * val)
{
	HELIOS_ASSERT_IF_FALSE(sem_ptr != HELIOS_NULL_PTR);
	HELIOS_ASSERT_IF_FALSE(sem_ptr->sem_init != false);
	HELIOS_ASSERT_IF_FALSE(val != HELIOS_NULL_PTR);

	*val = sem_ptr->sem_val;

	return success;
}
