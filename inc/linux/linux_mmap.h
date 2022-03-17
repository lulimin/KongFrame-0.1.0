// linux_mmap.h
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_MMAP_H
#define __LINUX_MMAP_H

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Share memory between processes.
struct plat_mmap_t
{
	size_t nSize;
	void* pMemory;
	int	ShmID;
};

// Confirm file exists.
static bool plat_mmap_confirm_file(const char* name)
{
	int fd = open(name, O_CREAT | O_EXCL | O_WRONLY, 0666);

	if (fd < 0)
	{
		if (errno != EEXIST)
		{
			return false;
		}
	}
	else
	{
		close(fd);
	}

	return true;
}

// Test specified name share memory exists.
inline bool plat_mmap_exists(const char* name)
{
	// Share file.
	if (!plat_mmap_confirm_file(name))
	{
		return false;
	}

	key_t key = ftok(name, 0);

	if (-1 == key)
	{
		return false;
	}

	//if (shmget(key, 1, IPC_CREAT | IPC_EXCL) == -1) {
	//	if (errno == EEXIST)
	//		return true;
	//}
	if (shmget(key, 0, 0) == -1)
	{
		return false;
	}

	return true;
}

// Initialize memory map.
inline bool plat_mmap_initialize(plat_mmap_t* pMap)
{
	pMap->nSize = 0;
	pMap->pMemory = NULL;
	pMap->ShmID = -1;
	return true;
}

// Shutdown memory map.
inline bool plat_mmap_shutdown(plat_mmap_t* pMap)
{
	if (pMap->pMemory)
	{
		shmdt(pMap->pMemory);
		pMap->pMemory = NULL;
	}

	if (pMap->ShmID != -1)
	{
		shmctl(pMap->ShmID, IPC_RMID, NULL);
		pMap->ShmID = -1;
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

	// Share file.
	if (!plat_mmap_confirm_file(name))
	{
		return false;
	}

	key_t key = ftok(name, 0);

	if (key == -1)
	{
		return false;
	}

	pMap->ShmID = shmget(key, size, IPC_CREAT | IPC_EXCL | 00666);

	if (pMap->ShmID == -1)
	{
		if (errno != EEXIST)
		{
			return false;
		}

		if (exists)
		{
			*exists = true;
		}

		pMap->ShmID = shmget(key, size, IPC_CREAT | 00666);

		if (pMap->ShmID == -1)
		{
			return false;
		}
	}

	// Map to process address space.
	pMap->pMemory = shmat(pMap->ShmID, NULL, 0);

	if ((void*)-1 == pMap->pMemory)
	{
		pMap->pMemory = NULL;
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

// Test validate.
inline bool plat_mmap_is_valid(plat_mmap_t* pMap)
{
	return (pMap->ShmID != -1) && (pMap->pMemory != NULL);
}

#endif // __LINUX_MMAP_H
