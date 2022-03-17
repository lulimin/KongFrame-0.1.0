// windows_mutex.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_MUTEX_H
#define __WINDOWS_MUTEX_H

#include <windows.h>
#include <string.h>

// Mutex between processes.
struct plat_mutex_t
{
	HANDLE hMutex;
};

// Test specified name mutex exists.
inline bool plat_mutex_exists(const char* name)
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

	if (NULL == mutex)
	{
		return false;
	}

	CloseHandle(mutex);
	return true;
}

// Guarantee unique.
inline bool plat_mutex_exclusive(const char* name)
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, name);

	if (NULL == hMutex)
	{
		return false;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return false; // Already exists.
	}

	// Cannot close mutex.
	return true;
}

// Initialize mutex.
inline bool plat_mutex_initialize(plat_mutex_t* pMutex)
{
	pMutex->hMutex = NULL;
	return true;
}

// Shutdown mutex.
inline bool plat_mutex_shutdown(plat_mutex_t* pMutex)
{
	if (!CloseHandle(pMutex->hMutex))
	{
		return false;
	}

	pMutex->hMutex = NULL;
	return true;
}

// Create or get mutex.
inline bool plat_mutex_create(plat_mutex_t* pMutex, const char* name,
	bool* exists)
{
	if (exists)
	{
		*exists = false;
	}

	pMutex->hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

	if (NULL == pMutex->hMutex)
	{
		pMutex->hMutex = CreateMutex(NULL, FALSE, name);

		if (NULL == pMutex->hMutex)
		{
			return false;
		}

		if (exists)
		{
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				*exists = true;
			}
		}
	}
	else
	{
		if (exists)
		{
			*exists = true;
		}
	}

	return true;
}

// Test mutex validate.
inline bool plat_mutex_is_valid(plat_mutex_t* pMutex)
{
	return pMutex->hMutex != NULL;
}

// Lock mutex.
inline void plat_mutex_lock(plat_mutex_t* pMutex)
{
	WaitForSingleObject(pMutex->hMutex, INFINITE);
}

// Unlock mutex.
inline void plat_mutex_unlock(plat_mutex_t* pMutex)
{
	ReleaseMutex(pMutex->hMutex);
}

#endif // __WINDOWS_MUTEX_H
