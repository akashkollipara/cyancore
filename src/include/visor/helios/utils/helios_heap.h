/*
 * CYANCORE LICENSE
 * Copyrights (C) 2022, Cyancore Team
 *
 * File Name		: helios_heap.h
 * Description		: CC OS Heap declaration (Placeholder)
 * Primary Author	: Pranjal Chanda [pranjalchanda08@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <visor/helios/helios.h>

void * helios_malloc(size_t size);
void helios_free(void *addr);
