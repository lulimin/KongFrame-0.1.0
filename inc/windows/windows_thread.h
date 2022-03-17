// windows_thread.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_THREAD_H
#define __WINDOWS_THREAD_H

#include <windows.h>
#include <process.h>

// Thread.
struct plat_thread_t
{
	HANDLE hThread;
};

// Thread yield.
struct plat_thread_yield_t
{
	HANDLE hEvent;
};

// Thread local storage.
struct plat_thread_key_t
{
	DWORD key;
};

typedef void (__cdecl* plat_thread_func_t)(void*);

// Thread sleep.
inline void plat_thread_sleep(int ms)
{
	Sleep(ms);
}

// Initialize thread.
inline bool plat_thread_initialize(plat_thread_t* pThread)
{
	pThread->hThread = NULL;
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
	pThread->hThread = (HANDLE)_beginthread(func, stack_size, pContext);
	return true;
}

// Stop thread.
inline bool plat_thread_stop(plat_thread_t* pThread)
{
	if (NULL == pThread->hThread)
	{
		return false;
	}

	for (;;)
	{
		DWORD res = WaitForSingleObject(pThread->hThread, 1000);

		if (WAIT_OBJECT_0 == res)
		{
			return true;
		}
		else if (WAIT_TIMEOUT == res)
		{
			DWORD exit_code;

			if (GetExitCodeThread(pThread->hThread, &exit_code) == FALSE)
			{
				return false;
			}

			if (exit_code != STILL_ACTIVE)
			{
				break;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

// Initialize thread yield.
inline bool plat_thread_yield_create(plat_thread_yield_t* pYield)
{
	pYield->hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	return pYield->hEvent != NULL;
}

// Shutdown thread yield.
inline bool plat_thread_yield_destroy(plat_thread_yield_t* pYield)
{
	return CloseHandle(pYield->hEvent) == TRUE;
}

// Wait thread wake up.
inline bool plat_thread_yield_wait(plat_thread_yield_t* pYield, int ms)
{
	if (ms < 0)
	{
		// Infinite waiting.
		WaitForSingleObject(pYield->hEvent, INFINITE);
	}
	else
	{
		DWORD res = WaitForSingleObject(pYield->hEvent, ms);

		if (WAIT_TIMEOUT == res)
		{
			return false;
		}
	}

	return true;
}

// Wake up thread.
inline bool plat_thread_yield_signal(plat_thread_yield_t* pYield)
{
	return SetEvent(pYield->hEvent) == TRUE;
}

// Create thread key.
inline bool plat_thread_key_create(plat_thread_key_t* pKey)
{
	DWORD tls_index = TlsAlloc();

	if (tls_index == TLS_OUT_OF_INDEXES)
	{
		return false;
	}
	
	pKey->key = tls_index;
	return true;
}

// Destroy thead key.
inline bool plat_thread_key_destroy(plat_thread_key_t* pKey)
{
	return TlsFree(pKey->key) == TRUE;
}

// Set thread key.
inline bool plat_thread_key_set(plat_thread_key_t* pKey, void* value)
{
	return TlsSetValue(pKey->key, value) == TRUE;
}

// Get thread key.
inline void* plat_thread_key_get(plat_thread_key_t* pKey)
{
	return TlsGetValue(pKey->key);
}

#endif // __WINDOWS_THREAD_H
