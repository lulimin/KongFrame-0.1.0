// windows_atomic.h 
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_ATOMIC_H
#define __WINDOWS_ATOMIC_H

#include <windows.h>

// Atomic.
struct plat_atomic_t
{
	volatile LONG nValue;
};

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
	return InterlockedIncrement(&pAtomic->nValue);
}

// Decrease atomic by 1.
inline int plat_atomic_decrement(plat_atomic_t* pAtomic)
{
	return InterlockedDecrement(&pAtomic->nValue);
}

// Set atomic value.
inline int plat_atomic_set(plat_atomic_t* pAtomic, int value)
{
	return InterlockedExchange(&pAtomic->nValue, value);
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
	return InterlockedCompareExchange(&pAtomic->nValue, new_value, old_value);
}

#endif // __WINDOWS_ATOMIC_H
