// linux_lock.h 
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_LOCK_H
#define __LINUX_LOCK_H

#include <pthread.h>

// Lock for mutual-exclusion synchronization.
struct plat_lock_t
{
	pthread_mutex_t hMutex;
};

// Create lock.
inline void plat_lock_create(plat_lock_t* pLock)
{
	pthread_mutex_init(&pLock->hMutex, NULL);
}

// Destroy lock.
inline void plat_lock_destroy(plat_lock_t* pLock)
{
	pthread_mutex_destroy(&pLock->hMutex);
}

// Get lock.
inline void plat_lock_enter(plat_lock_t* pLock)
{
	pthread_mutex_lock(&pLock->hMutex);
}

// Release lock.
inline void plat_lock_leave(plat_lock_t* pLock)
{
	pthread_mutex_unlock(&pLock->hMutex);
}

#endif // __LINUX_LOCK_H
