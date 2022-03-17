// thread.h
// Created by lulimin on 2019/2/14.

#ifndef __THREAD_H
#define __THREAD_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_thread.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_thread.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_thread.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_thread.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_thread.h"
#endif // K_PLATFORM_MAC

// Thread

class Thread
{
private:
	typedef bool(__cdecl *entry_func_t)(void*);
	typedef void(__cdecl *loop_func_t)(void*);
	typedef bool(__cdecl *exit_func_t)(void*);

	// Windows thread work procedure.
	static void __cdecl WinWorkerProc(void* pParameter)
	{
		Thread* pthis = (Thread*)pParameter;
		loop_func_t loop_func = pthis->m_LoopFunc;
		void* context = pthis->m_pContext;
		int sleep_ms = pthis->m_nSleep;

		if (pthis->m_EntryFunc)
		{
			pthis->m_EntryFunc(context);
		}

		while (!pthis->m_bExit)
		{
			loop_func(context);

			if (sleep_ms >= 0)
			{
				plat_thread_sleep(sleep_ms);
			}
		}

		if (pthis->m_ExitFunc)
		{
			pthis->m_ExitFunc(context);
		}
	}

	// Thread work procedure.
	static void* __cdecl WorkerProc(void* pParameter)
	{
		Thread* pthis = (Thread*)pParameter;
		loop_func_t loop_func = pthis->m_LoopFunc;
		void* context = pthis->m_pContext;
		int sleep_ms = pthis->m_nSleep;

		if (pthis->m_EntryFunc)
		{
			pthis->m_EntryFunc(context);
		}

		while (!pthis->m_bExit)
		{
			loop_func(context);

			if (sleep_ms >= 0)
			{
				plat_thread_sleep(sleep_ms);
			}
		}

		if (pthis->m_ExitFunc)
		{
			pthis->m_ExitFunc(context);
		}

		return NULL;
	}

public:
	Thread(loop_func_t loop_func, entry_func_t entry_func,
		exit_func_t exit_func, void* context, int sleep_ms, int stack_size)
	{
		m_LoopFunc = loop_func;
		m_EntryFunc = entry_func;
		m_ExitFunc = exit_func;
		m_pContext = context;
		m_nSleep = sleep_ms;
		m_nStackSize = stack_size;
		m_bExit = false;
		plat_thread_initialize(&m_Thread);
	}

	~Thread()
	{
		plat_thread_shutdown(&m_Thread);
	}

	// Set thread quit status.
	void SetExit(bool value)
	{ 
		m_bExit = value;
	}

	// Get thread quit status.
	bool GetExit() const
	{ 
		return m_bExit;
	}

	// Start thread.
	bool Start()
	{
		m_bExit = false;
#ifdef K_PLATFORM_WINDOWS
		return plat_thread_start(&m_Thread, WinWorkerProc, m_nStackSize, this);
#else
		return plat_thread_start(&m_Thread, WorkerProc, m_nStackSize, this);
#endif // K_PLATFORM_WINDOWS
	}

	// Stop thread.
	bool Stop()
	{
		m_bExit = true;
		return plat_thread_stop(&m_Thread);
	}

private:
	Thread();
	Thread(const Thread&);
	Thread& operator=(const Thread&);

private:
	entry_func_t m_EntryFunc;
	loop_func_t m_LoopFunc;
	exit_func_t m_ExitFunc;
	void* m_pContext;
	int m_nSleep;
	int m_nStackSize;
	bool m_bExit;
	plat_thread_t m_Thread;
};

// ThreadYield

class ThreadYield
{
public:
	ThreadYield()
	{
		plat_thread_yield_create(&m_Yield);
	}

	~ThreadYield()
	{
		plat_thread_yield_destroy(&m_Yield);
	}

	// Waiting thread wake up.
	bool Wait(int ms)
	{
		return plat_thread_yield_wait(&m_Yield, ms);
	}

	// Send signal for waking.
	bool Signal()
	{
		return plat_thread_yield_signal(&m_Yield);
	}

private:
	plat_thread_yield_t m_Yield;
};

#endif // __THREAD_H
