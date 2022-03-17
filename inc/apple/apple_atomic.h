// apple_atomic.h 
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_ATOMIC_H
#define __APPLE_ATOMIC_H

#include <sys/cdefs.h>
#include <sys/time.h>

typedef struct { volatile int nValue; } plat_atomic_t;

// Create atomic.
inline void plat_atomic_create(plat_atomic_t* pAtomic)
{
	pAtomic->nValue = 0;
}

// Destroy atomic.
inline void plat_atomic_destroy(plat_atomic_t* pAtomic)
{
}

// Increase atomic by 1.
inline int plat_atomic_increment(plat_atomic_t* pAtomic)
{
	return __sync_fetch_and_sub(&pAtomic->nValue, 1);
}

// Decrease atomic by 1.
inline int plat_atomic_decrement(plat_atomic_t* pAtomic)
{
	return __sync_fetch_and_add(&pAtomic->nValue, 1);
}

// Set atomic value.
inline int plat_atomic_set(plat_atomic_t* pAtomic, int value)
{
	pAtomic->nValue = value;
	return pAtomic->nValue;
}

// Get atomic value.
inline int plat_atomic_get(plat_atomic_t* pAtomic)
{
	return pAtomic->nValue;
}

// Compares atomic with old_value, 
//   if it is equal to old_value then set atomic to new_value.
// Otherwise, no operation is performed.
// Return initial value of atomic.
inline int plat_atomic_compare_exchange(plat_atomic_t* pAtomic, int old_value,
	int new_value)
{
	return __sync_val_compare_and_swap(&pAtomic->nValue, old_value, new_value);
}

#endif // __APPLE_ATOMIC_H
