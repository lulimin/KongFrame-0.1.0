// linux_thread.h
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_THREAD_H
#define __LINUX_THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

// Thread.
struct plat_thread_t
{
	pthread_t hThread;
};

// Thread yield.
struct plat_thread_yield_t
{
	pthread_cond_t hCond;
	pthread_mutex_t hMutex;
};

// Thread local storage.
struct plat_thread_key_t
{
	pthread_key_t key;
};

typedef void*(*plat_thread_func_t)(void*);

// Thread sleep.
inline void plat_thread_sleep(int ms)
{
	if (ms > 0)
	{
		struct timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = ms * 1000000;
		nanosleep(&ts, NULL);
	}
	else if (ms == 0)
	{
		sched_yield();
	}
}

// Initialize thread.
inline bool plat_thread_initialize(plat_thread_t* pThread)
{
	pThread->hThread = 0;
	return true;
}

// Shutdown thread.
inline bool plat_thread_shutdown(plat_thread_t* pThread)
{
	return true;
}

// Start thread.
inline bool plat_thread_start(plat_thread_t* pThread, plat_thread_func_t func,
	int stack_size, void* pContext)
{
	int res;
	
	if (stack_size > 0)
	{
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, stack_size);
		res = pthread_create(&pThread->hThread, &attr, func, pContext);
		pthread_attr_destroy(&attr);
	}
	else
	{
		res = pthread_create(&pThread->hThread, NULL, func, pContext);
	}

	return 0 == res;
}

// Stop thread.
inline bool plat_thread_stop(plat_thread_t* pThread)
{
	return pthread_join(pThread->hThread, NULL) == 0;
}

// Initialize thread yield.
inline bool plat_thread_yield_create(plat_thread_yield_t* pYield)
{
	pthread_cond_init(&pYield->hCond, NULL);
	pthread_mutex_init(&pYield->hMutex, NULL);
	return true;
}

// Shutdown thread yield.
inline bool plat_thread_yield_destroy(plat_thread_yield_t* pYield)
{
	pthread_cond_destroy(&pYield->hCond);
	pthread_mutex_destroy(&pYield->hMutex);
	return true;
}

// Wait thread wake up.
inline bool plat_thread_yield_wait(plat_thread_yield_t* pYield, int ms)
{
	bool result = false;

	if (ms < 0)
	{
		// Infinite waiting.
		pthread_mutex_lock(&pYield->hMutex);

		int res = pthread_cond_wait(&pYield->hCond, &pYield->hMutex);

		if (0 == res)
		{
			result = true;
		}

		pthread_mutex_unlock(&pYield->hMutex);
	}
	else
	{
		struct timespec ts;

		clock_gettime(CLOCK_REALTIME, &ts);

		ts.tv_nsec += ms * 1000000;

		if (ts.tv_nsec > 999999999)
		{
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}

		pthread_mutex_lock(&pYield->hMutex);

		int res = pthread_cond_timedwait(&pYield->hCond, &pYield->hMutex, &ts);

		if (0 == res)
		{
			result = true;
		}

		pthread_mutex_unlock(&pYield->hMutex);
	}

	return result;
}

// Wake up thread.
inline bool plat_thread_yield_signal(plat_thread_yield_t* pYield)
{
	return pthread_cond_signal(&pYield->hCond) == 0;
}

// Create thread key.
inline bool plat_thread_key_create(plat_thread_key_t* pKey)
{
	return pthread_key_create(&pKey->key, NULL) == 0;
}

// Destroy thead key.
inline bool plat_thread_key_destroy(plat_thread_key_t* pKey)
{
	return pthread_key_delete(pKey->key) == 0;
}

// Set thread key.
inline bool plat_thread_key_set(plat_thread_key_t* pKey, void* value)
{
	return pthread_setspecific(pKey->key, value) == 0;
}

// Get thread key.
inline void* plat_thread_key_get(plat_thread_key_t* pKey)
{
	return pthread_getspecific(pKey->key);
}

#endif // __LINUX_THREAD_H
