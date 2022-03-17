// windows_lock.h 
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_LOCK_H
#define __WINDOWS_LOCK_H

#include <windows.h>

// Lock for mutual-exclusion synchronization.

struct plat_lock_t
{
	CRITICAL_SECTION hLock;
};

// Create lock.
inline void plat_lock_create(plat_lock_t* pLock)
{
	InitializeCriticalSection(&pLock->hLock);
}

// Destroy lock.
inline void plat_lock_destroy(plat_lock_t* pLock)
{
	DeleteCriticalSection(&pLock->hLock);
}

// Get lock.
inline void plat_lock_enter(plat_lock_t* pLock)
{
	EnterCriticalSection(&pLock->hLock);
}

// Release lock.
inline void plat_lock_leave(plat_lock_t* pLock)
{
	LeaveCriticalSection(&pLock->hLock);
}

#endif // __WINDOWS_LOCK_H
