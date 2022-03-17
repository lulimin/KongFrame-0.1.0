// mutex.h
// Created by lulimin on 2019/2/14.

#ifndef __MUTEX_H
#define __MUTEX_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_mutex.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_mutex.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_mutex.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_mutex.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_mutex.h"
#endif // K_PLATFORM_MAC

// Mutex between processes.

class Mutex
{
public:
	// Test specified name mutex exists.
	static bool Exists(const char* name)
	{
		return plat_mutex_exists(name);
	}

	// Guarantee uniquely.
	static bool Exclusive(const char* name)
	{
		return plat_mutex_exclusive(name);
	}

public:
	Mutex()
	{
		plat_mutex_initialize(&m_Mutex);
	}

	~Mutex()
	{
		plat_mutex_shutdown(&m_Mutex);
	}

	// Test validate.
	bool IsValid()
	{
		return plat_mutex_is_valid(&m_Mutex);
	}

	// Create or get mutex.
	bool Create(const char* name, bool* exists)
	{
		return plat_mutex_create(&m_Mutex, name, exists);
	}

	// Delete mutex.
	bool Destroy()
	{
		return plat_mutex_shutdown(&m_Mutex);
	}

	// Lock mutex.
	void Lock()
	{
		plat_mutex_lock(&m_Mutex);
	}

	// Unlock mutex.
	void Unlock()
	{
		plat_mutex_unlock(&m_Mutex);
	}

private:
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

private:
	plat_mutex_t m_Mutex;
};

// Automatic lock and unlock in scope range.

class ScopedMutex
{
public:
	explicit ScopedMutex(Mutex* pMutex)
	{
		Assert(pMutex != NULL);
		Assert(pMutex->IsValid());

		m_pMutex = pMutex;
		m_pMutex->Lock();
	}

	~ScopedMutex()
	{
		m_pMutex->Unlock();
	}

private:
	ScopedMutex();
	ScopedMutex(const ScopedMutex&);
	ScopedMutex& operator=(const ScopedMutex&);

private:
	Mutex* m_pMutex;
};

#endif // __MUTEX_H
