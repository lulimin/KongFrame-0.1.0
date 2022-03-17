// time_counter.h
// Created by lulimin on 2019/2/14.

#ifndef __TIME_COUNTER_H
#define __TIME_COUNTER_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_timer.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_timer.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_timer.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_timer.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_timer.h"
#endif // K_PLATFORM_MAC

// High precision timer.

class TimeCounter
{
public:
	TimeCounter()
	{
		plat_timer_initialize(&m_Timer);
	}

	~TimeCounter()
	{
		plat_timer_shutdown(&m_Timer);
	}

	// Set use high precision.
	void SetHighPrecision(bool flag)
	{
		plat_timer_set_high_precision(&m_Timer, flag);
	}

	// Create timer.
	bool Create()
	{
		return plat_timer_create(&m_Timer);
	}

	// Get current time value.
	double GetCurrentSeconds()
	{
		return plat_timer_get_current(&m_Timer);
	}

	// Get elapse seconds.
	double GetElapseSeconds(double expect = 0.0)
	{
		return plat_timer_get_elapse(&m_Timer, expect);
	}

	// Get elapse milliseconds.
	int GetElapseMilliseconds(int expect = 0)
	{
		return plat_timer_get_elapse_ms(&m_Timer, expect);
	}

private:
	plat_timer_t m_Timer;
};

#endif // __TIME_COUNTER_H
