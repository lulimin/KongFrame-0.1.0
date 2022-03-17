// linux_mutex.h
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// Mutex between processes.
struct plat_mutex_t
{
	int SemID;
};

// Confirm file exists.
inline bool plat_mutex_confirm_file(const char* name)
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

// Test specified name mutex exists.
inline bool plat_mutex_exists(const char* name)
{
	// Share file.
	if (!plat_mutex_confirm_file(name))
	{
		return false;
	}

	key_t key = ftok(name, 0);

	if (-1 == key)
	{
		return false;
	}

	if (semget(key, 0, 0) == -1)
	{
		return false;
	}

	return true;
}

// Guarantee mutex unique.
inline bool plat_mutex_exclusive(const char* name)
{
	// Avoid repeat create file.
	if (!plat_mutex_confirm_file(name))
	{
		return false;
	}

	// Open mutex file.
	int fd = open(name, O_RDWR, 0666);

	if (fd < 0)
	{
		return false;
	}

	// Get file lock status.
	struct flock fl;

	memset(&fl, 0, sizeof(fl));
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	int res = fcntl(fd, F_GETLK, &fl);

	if (res < 0)
	{
		close(fd);
		return false;
	}

	if (fl.l_type != F_UNLCK)
	{
		// Already locked by other process.
		close(fd);
		return false;
	}

	// Lock file.
	memset(&fl, 0, sizeof(fl));
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	res = fcntl(fd, F_SETLK, &fl);

	if (res < 0)
	{
		close(fd);
		return false;
	}

	// Cannot close file for lock safe.
	return true;
}

// Initialize mutex.
inline bool plat_mutex_initialize(plat_mutex_t* pMutex)
{
	pMutex->SemID = -1;
	return true;
}

// Shutdown mutex.
inline bool plat_mutex_shutdown(plat_mutex_t* pMutex)
{
	if (pMutex->SemID != -1)
	{
		semctl(pMutex->SemID, 0, IPC_RMID, NULL);
		pMutex->SemID = -1;
	}

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

	// Share file.
	if (!plat_mutex_confirm_file(name))
	{
		return false;
	}

	key_t key = ftok(name, 0);

	if (-1 == key)
	{
		return false;
	}

	pMutex->SemID = semget(key, 1, IPC_CREAT | IPC_EXCL | 00666);

	if (-1 == pMutex->SemID)
	{
		if (errno == EEXIST)
		{
			// Already exists.
			if (exists)
			{
				*exists = true;
			}

			pMutex->SemID = semget(key, 1, IPC_CREAT | 00666);

			if (-1 == pMutex->SemID)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		// Create new, initialize value.
		union semu { int val; };
		semu arg;

		arg.val = 1;

		if (semctl(pMutex->SemID, 0, SETVAL, arg) == -1)
		{
			// Failed.
			semctl(pMutex->SemID, 0, IPC_RMID, NULL);
			pMutex->SemID = -1;
			return false;
		}
	}

	return true;
}

// Test mutex validate.
inline bool plat_mutex_is_valid(plat_mutex_t* pMutex)
{
	return pMutex->SemID != -1;
}

// Mutex get lock.
inline void plat_mutex_lock(plat_mutex_t* pMutex)
{
	struct sembuf lock;

	lock.sem_num = 0;
	lock.sem_op = -1;
	lock.sem_flg = SEM_UNDO;
	semop(pMutex->SemID, &lock, 1);
}

// Mutex release lock.
inline void plat_mutex_unlock(plat_mutex_t* pMutex)
{
	struct sembuf unlock;

	unlock.sem_num = 0;
	unlock.sem_op = 1;
	unlock.sem_flg = SEM_UNDO;
	semop(pMutex->SemID, &unlock, 1);
}

#endif // __LINUX_MUTEX_H
