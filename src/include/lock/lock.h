#pragma once
#define _LOCK_H_


#if USE_SPINLOCK
#include <lock/spinlock.h>
typedef spinlock_t lock_t;

#define lock_acquire(X)		spinlock_acquire(X)
#define lock_release(X)		spinlock_release(X)
#endif

#if USE_BAKERYLOCK
#include <lock/bakerylock.h>
typedef bakerylock_t lock_t;

#define lock_acquire(X)		bakerylock_acquire(X)
#define lock_release(X)		bakerylock_release(X)
#endif
