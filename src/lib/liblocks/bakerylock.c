
#include <status.h>
#include <stdint.h>
#include <arch.h>
#include <lock/bakerylock.h>

void bakerylock_acquire(bakerylock_t *key)
{
	unsigned int tid = arch_core_index();
	key->protect[tid] = 1;
	arch_dmb();

	unsigned int i, n_max = 0;
	for(i = 0; i < N_CORES; i++)
	{
		size_t n_cust = key->thread_count[i];
		n_max = n_cust > n_max ? n_cust : n_max;
	}

	key->thread_count[tid] = n_max + 1;

	arch_dmb();
	key->protect[tid] = 0;
	arch_dmb();

	for(i = 0; i < N_CORES; i++)
	{
		while(key->protect[i]);
		arch_dmb();

		while(key->thread_count[i] && (
		      (key->thread_count[i] < key->thread_count[tid]) ||
		       ((key->thread_count[i] == key->thread_count[tid]) &&
			(i < tid))));
	}
}

void bakerylock_release(bakerylock_t *key)
{
	unsigned int tid = arch_core_index();
	arch_dmb();
	key->thread_count[tid] = 0;
}

