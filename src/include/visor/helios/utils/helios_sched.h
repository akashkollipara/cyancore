/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_sched.h
 * Description		: CC OS Kernel scheduler declaration
 * Primary Author	: Pranjal Chanda [pranjalchanda08@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

/*****************************************************
 *	INCLUDES
 *****************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <status.h>
#include <syslog.h>
#include <platform.h>

/*****************************************************
 *	DEFINES
 *****************************************************/
#define HELIOS_NULL_PTR			NULL
#define HELIOS_DELAY_MAX		SIZE_MAX

#define	HELIOS_DYNAMIC 			HELIOS_USE_DYNAMIC

#ifdef HELIOS_DEBUG
#define HELIOS_ERR(fmt, ...)		sysdbg2("[HELIOS ERR]: "fmt"\n", ##__VA_ARGS__)
#define HELIOS_DBG(fmt, ...)		sysdbg3("[HELIOS DBG]: "fmt"\n", ##__VA_ARGS__)
#else
#define HELIOS_ERR(fmt,...)
#define HELIOS_DBG(fmt,...)
#endif  /* HELIOS_DEBUG */

#define HELIOS_ASSERT_IF_FALSE(con)					\
	do{								\
		if(!(con)){						\
			HELIOS_ERR("(%s) : Invalid ARG\n", __func__);	\
			return error_func_inval_arg;			\
		}							\
	} while (false)

#define HELIOS_SCHED_PANIC(err)					\
	do{							\
		syslog(fail, "[HELIOS EXC]: %s\n", #err);	\
		plat_panic_handler();				\
	} while (false)
/*****************************************************
 *	TYPEDEFS
 *****************************************************/
typedef struct helios_sched_tcb helios_sched_tcb_t;
typedef struct helios_sched helios_sched_t;
typedef const char c_char;
typedef uintptr_t helios_args;
typedef void (*task_fn_t)(void);
typedef void (*helios_cb_hook_t)(helios_args args);
typedef enum
{
	helios_sched_task_status_exit	= 0x00,			///> Initial State
	helios_sched_task_status_running	= 0x01,			///> Task currently running
	helios_sched_task_status_ready	= 0x02,			///> Task Ready to despatch
	helios_sched_task_status_wait	= 0x03,			///> Task in wait state
	helios_sched_task_status_pause	= 0x04,			///> Task in pause state
	helios_sched_task_status_max 	= 0xff,			///> Do Nt Use
} helios_sched_task_status_t;

/*
 * @note	All negative callbacks are internally used by the scheduler
 */
typedef enum
{
	helios_sched_cb_power_pre_sleep		= 0x00,
	helios_sched_cb_power_post_sleep	= 0x01,
	helios_sched_cb_power_sleep		= 0x02,
	helios_sched_cb_deadlock_notify		= 0x03,
	helios_sched_cb_max			= 0xff
}helios_sched_cb_t;

typedef enum
{
	helios_task_flag_set_anti_deadlock	= (1 << 0)		///> Enable Antideadlock for the task
}helios_task_flag_t;
typedef struct link
{
	helios_sched_tcb_t * prev;
	helios_sched_tcb_t * next;
}link_t;

typedef struct wres
{
	uintptr_t wait_on_resource;				///> Resource on hich the task is waiting on
	size_t 	  task_delay_ticks;				///> Time delay in ticks
}wres_t;

struct helios_sched_tcb
{

	c_char  * name;						///> Name of the Current Task
	uint8_t   priority;					///> Priority of the task
	uint8_t	  task_flags;					///> Task Flags
	uintptr_t stack_ptr;					///> Stack Pointer
	task_fn_t task_func;					///> Task Call Function
	uintptr_t args_ptr;					///> Task Call argument ptr
	wres_t	  wait_res;					///> Wait Task resource
	link_t	  ready_link;					///> Ready Linked List Pointers
	link_t    wait_link;					///> Wait Linked List Pointers
	uint16_t  task_id;					///> Task ID assigned
#if HELIOS_ANTI_DEADLOCK
	size_t	  task_wd_ticks;				///> Tick down counter for Anti Deadlock system
#endif /* HELIOS_ANTI_DEADLOCK */
	helios_sched_task_status_t task_status;			///> Current state of the task
};

typedef struct helios_sched_func_cb
{
	helios_cb_hook_t pre_sched;
#if HELIOS_POWER_SAVE_EN
	helios_cb_hook_t pre_sleep_cb;
	helios_cb_hook_t post_sleep_cb;
	helios_cb_hook_t sleep_cb;
#endif /* HELIOS_POWER_SAVE_EN */
#if HELIOS_ANTI_DEADLOCK
	helios_cb_hook_t deadlock_notify;
#endif /* HELIOS_ANTI_DEADLOCK */
}helios_sched_func_cb_t;

typedef struct helios_sched_ctrl
{
	helios_sched_tcb_t 	* ready_list_head;
	helios_sched_tcb_t 	* curr_task;
	helios_sched_tcb_t 	* wait_list_head;
	helios_sched_t 		* selected_sched;
	helios_sched_func_cb_t cb_hooks_reg;
}helios_sched_ctrl_t;

/**
 * @brief Prototype of scheduler algorithm function
 */
typedef void (* algo_fn)(helios_sched_ctrl_t * sched_ctrl);

typedef enum
{
	helios_sched_algo_round_robin		= 0x00,			///> Round Robin scheduling algorithm
	helios_sched_algo_priority_driven	= 0x01,			///> Priority driven Scheduling
	helios_sched_algo_max			= 0xff
}helios_sched_algo_t;

typedef struct helios_sched
{
	helios_sched_algo_t helios_selected_algo;			///> Selected Algorithm ID
	algo_fn algo_function;						///> Pointer to algorithm function
}helios_sched_t;

typedef struct helios_sched_anti_deadlock
{
	c_char * name;
	task_fn_t task_func;
}helios_sched_anti_deadlock_t;
