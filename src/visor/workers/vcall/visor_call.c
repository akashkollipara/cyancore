/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019-2023, Cyancore Team
 *
 * File Name		: machine_call.c
 * Description		: This file contains sources of machine call handler
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <syslog.h>
#include <visor_call.h>
#include <platform.h>
#include <resource.h>

/* vcall Table start and end  defined in linker script using VCALL_TABLE */
extern vcall_t _vcall_table_start;
extern vcall_t _vcall_table_end;

typedef void (*vcb_t)(call_arg_t, call_arg_t, call_arg_t, vret_t *);
static vcb_t get_callback_from_table(const vcall_t *start, const vcall_t *end, vcall_id_t id)
{
	vcall_t *ptr = (vcall_t *)start;
	while(ptr < end)
	{
		if(ptr->id == id)
			return (ptr->callback != NULL) ? ptr->callback : NULL;
		ptr++;
	}
	return (vcb_t) 0;
}

void vcall_handler(vcall_id_t id, call_arg_t a0, call_arg_t a1, call_arg_t a2, vret_t *ret)
{
	sysdbg4("VCall: id=%p, a0=%p, a1=%p, a2=%p\n", id, a0, a1, a2);
	/* Callback function pointer */
	vcb_t cb;

	/* Assign "ret" parameters to default error case,
	 * on execution of the call, the ret will updated.
	 */
	ret->p = (uintptr_t) 0;
	ret->size = 0;
	ret->status = error_vcall_code_inval;

	cb = get_callback_from_table(&_vcall_table_start, &_vcall_table_end, id);
	if(cb != 0)
		cb(a0, a1, a2, ret);
	return;
}
