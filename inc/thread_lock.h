// thread_lock.h 
// Created by lulimin on 2019/2/14.

#ifndef __THREAD_LOCK_H
#define __THREAD_LOCK_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_lock.h"
	#include "windows/windows_atomic.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_lock.h"
	#include "linux/linux_atomic.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_lock.h"
	#include "linux/linux_atomic.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_lock.h"
	#include "apple/apple_atomic.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_lock.h"
	#include "apple/apple_atomic.h"
#endif // K_PLATFORM_MAC

// Locked integer.

class LockInt
{
public:
	LockInt()
	{
		plat_atomic_create(&m_Atomic);
	}

	~LockInt()
	{
		plat_atomic_destroy(&m_Atomic);
	}

	// Increase by 1.
	int Increment()
	{
		return plat_atomic_increment(&m_Atomic);
	}

	// Decrease by 1.
	int Decrement()
	{
		return plat_atomic_decrement(&m_Atomic);
	}

	// Set value.
	int SetValue(int value)
	{
		return plat_atomic_set(&m_Atomic, value);
	}

	// Get value.
	int GetValue()
	{
		return plat_atomic_get(&m_Atomic);
	}

	// Exchange value after compare, return origin value.
	// If current value equal to old_value then set to new_value and return old_value.
	int CompareExchange(int old_value, int new_value)
	{
		return plat_atomic_compare_exchange(&m_Atomic, old_value, new_value);
	}

public:
	plat_atomic_t m_Atomic;
};

// Share resource access lock.

class ThreadLock
{
public:
	ThreadLock()
	{
		plat_lock_create(&m_Lock);
	}

	~ThreadLock()
	{
		plat_lock_destroy(&m_Lock);
	}

	// Get lock.
	void Lock()
	{
		plat_lock_enter(&m_Lock);
	}

	// Release lock.
	void Unlock()
	{
		plat_lock_leave(&m_Lock);
	}

private:
	plat_lock_t m_Lock;
};

// Automatic lock and unlock in scope range.

class ScopedLock
{
public:
	explicit ScopedLock(ThreadLock* pLock)
	{
		Assert(pLock != NULL);

		m_pLock = pLock;
		m_pLock->Lock();
	}
	
	~ScopedLock() 
	{ 
		m_pLock->Unlock(); 
	}

private:
	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator=(const ScopedLock&);

private:
	ThreadLock* m_pLock;
};

#endif // __THREAD_LOCK_H
