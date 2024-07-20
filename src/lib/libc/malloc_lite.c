/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: malloc_lite.c
 * Description		: This file contains sources of libc-malloc
 *			  functions
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <status.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lock/lock.h>
#include <arch.h>
#include <mmio.h>
#include <plat_mem.h>

extern uint8_t _heap_start, _heap_end, _heap_size;
static istate_t state;
static lock_t mlock;

typedef struct chunk
{
	size_t size;
#if HEAP_ALIGN > 1
	size_t free;
#else
	uint8_t free;
#endif
	struct chunk *next;
} chunk_t;

static chunk_t *freeList;

static void heap_lock(void)
{
	lock_acquire(&mlock);
	arch_di_save_state(&state);
}

static void heap_unlock(void)
{
	lock_release(&mlock);
	arch_ei_restore_state(&state);
}

static void split(chunk_t *fit_slot, size_t size)
{
	chunk_t *new = (chunk_t *)((size_t) fit_slot + size + sizeof(chunk_t));
	new->size = fit_slot->size - size - sizeof(chunk_t);
	new->free = 1;
	new->next = fit_slot->next;
	fit_slot->size = size;
	fit_slot->free = 0;
	fit_slot->next = new;
	return;
}

static void merge()
{
	chunk_t *cur;
	cur = freeList;
	while(cur && cur->next)
	{
		if(cur->free && cur->next->free)
		{
			cur->size += cur->next->size + sizeof(chunk_t);
			cur->next = cur->next->next;
		}
		cur = cur->next;
	}
}

static chunk_t *get_header(void *p)
{
	return (chunk_t *)p - 1;
}

status_t platform_init_heap()
{
	heap_lock();
	size_t sz = (size_t)&_heap_end;
	sz -= (size_t)&_heap_start;
	memset(&_heap_start, 0, sz);
	freeList = (void *)&_heap_start;
	freeList->size = sz - sizeof(chunk_t);
	freeList->free = 1;
	freeList->next = NULL;
	heap_unlock();
	return success;
}

void *malloc(size_t n_bytes)
{
	chunk_t *cur;
	void *p = NULL;

	if(!n_bytes)
		return NULL;

#if HEAP_ALIGN > 1
	ROUNDUP_ALIGN(n_bytes, ALIGN_BOUND);
#endif

	heap_lock();
	if(!freeList->size)
		goto exit;
	cur = freeList;
	while(cur->size < n_bytes || (!cur->free && cur->next))
		cur = cur->next;
	if(cur->size == n_bytes)
		cur->free = 0;
	else if(cur->size > (n_bytes + sizeof(chunk_t)))
		split(cur, n_bytes);
	else
		goto exit;
	p = (void *)(++cur);
exit:
	heap_unlock();
	return p;
}

void free(void *ptr)
{
	if(ptr == NULL)
		return;

	chunk_t *cur = get_header(ptr);
	heap_lock();
	if((void *)&_heap_start <= (void *)cur &&
		(void *)cur <= (void *)&_heap_end)
	{
		cur->free = 1;
		merge();
		merge();
	}
	heap_unlock();
	return;
}

void *calloc(size_t n_blocks, size_t n_bytes)
{
	n_bytes *= n_blocks;
	void *p = malloc(n_bytes);
	if(p)
		memset(p, 0, n_bytes);
	return p;
}

void *realloc(void *p, size_t n_bytes)
{
	if(!p)
		return malloc(n_bytes);
	if(!n_bytes)
	{
		free(p);
		return NULL;
	}

	const chunk_t *header = get_header(p);
	void *new_p = malloc(n_bytes);
	if(!new_p)
		return NULL;
	memcpy(new_p, p, header->size);
	free(p);
	return new_p;
}

size_t heap_usage(void)
{
	unsigned int usage = 0;
	chunk_t *head = (chunk_t *)&_heap_start;
	while(head->next != NULL)
	{
		if(!head->free)
			usage += head->size + sizeof(chunk_t);
		head = head->next;
	}
	return usage;
}

void __heap_status(bool dump)
{
	size_t i;
	unsigned int cntr;
	unsigned int h_used, h_perc;
	h_used = heap_usage();
	h_perc = (h_used * 100)/(unsigned int)&_heap_size;
	printf("Heap Dump: %p - %p\n", &_heap_start, &_heap_end);
	printf("Heap Used: %u/%u - %u%%\n", h_used,
		(unsigned int)&_heap_size, h_perc);

	if(!dump)
		return;

	for(i = (size_t)&_heap_start; i < (size_t)&_heap_end; i+=32)
	{
		printf("[");
		for(cntr = 0; cntr < 32; cntr++)
		{
			printf("%02x", MMIO8(i+cntr));
			if(cntr && !((cntr + 1) % 4) && cntr != 31)
				printf(" ");
		}
		printf("]\n");
	}
}

