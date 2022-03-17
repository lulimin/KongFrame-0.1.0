// windows_mmap.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_MMAP_H
#define __WINDOWS_MMAP_H

#include <windows.h>
#include <string.h>

// Share memory between processes.
struct plat_mmap_t
{
	size_t nSize;
	HANDLE hFile;
	void* pMemory;
};

// Test specified name share memory exists.
inline bool plat_mmap_exists(const char* name)
{
	HANDLE hFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);

	if (NULL == hFile)
	{
		return false;
	}

	CloseHandle(hFile);
	return true;
}

// Initialize memory map.
inline bool plat_mmap_initialize(plat_mmap_t* pMap)
{
	pMap->nSize = 0;
	pMap->hFile = NULL;
	pMap->pMemory = NULL;
	return true;
}

// Shutdown memory map.
inline bool plat_mmap_shutdown(plat_mmap_t* pMap)
{
	if (pMap->pMemory != NULL)
	{
		UnmapViewOfFile(pMap->pMemory);
		pMap->pMemory = NULL;
	}

	if (pMap->hFile != NULL)
	{
		if (!CloseHandle(pMap->hFile))
		{
			return false;
		}

		pMap->hFile = NULL;
	}

	return true;
}

// Get or create share memory.
inline bool plat_mmap_create(plat_mmap_t* pMap, const char* name, size_t size,
	bool* exists)
{
	pMap->nSize = size;

	if (exists)
	{
		*exists = false;
	}

	pMap->hFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);

	if (NULL == pMap->hFile)
	{
		pMap->hFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
			PAGE_READWRITE, 0, DWORD(pMap->nSize), name);

		if (NULL == pMap->hFile)
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

	pMap->pMemory = MapViewOfFile(pMap->hFile, FILE_MAP_ALL_ACCESS, 0, 0,
		pMap->nSize);

	if (NULL == pMap->pMemory)
	{
		CloseHandle(pMap->hFile);
		pMap->hFile = NULL;
		return false;
	}

	return true;
}

// Get memory size.
inline size_t plat_mmap_get_size(plat_mmap_t* pMap)
{
	return pMap->nSize;
}

// Get memory address.
inline void* plat_mmap_get_memory(plat_mmap_t* pMap)
{
	return pMap->pMemory;
}

// Test memory validate.
inline bool plat_mmap_is_valid(plat_mmap_t* pMap)
{
	return (pMap->hFile != NULL) && (pMap->pMemory != NULL);
}

#endif // __WINDOWS_MMAP_H
