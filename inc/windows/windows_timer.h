// windows_timer.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_TIMER_H
#define __WINDOWS_TIMER_H

#include <windows.h>
#pragma comment(lib, "winmm.lib")

// High precision timer
struct plat_timer_t
{
	bool bInit;
	bool bUseQPF;
	unsigned int nPeriodMin;
	LARGE_INTEGER llFrequency;
	LARGE_INTEGER llStartCount;
	LARGE_INTEGER llLastCount;
	DWORD nLastMilliseconds;
	DWORD nStartMilliseconds;
	double dBaseTime;
};

// Initialize timer.
inline bool plat_timer_initialize(plat_timer_t* pTimer)
{
	pTimer->bInit = false;
	pTimer->bUseQPF = true;
	pTimer->dBaseTime = 0.0;
	return true;
}

// Shutdown timer.
inline bool plat_timer_shutdown(plat_timer_t* pTimer)
{
	if (pTimer->bInit && (!pTimer->bUseQPF))
	{
		timeEndPeriod(pTimer->nPeriodMin);
	}

	return true;
}

// Create timer.
inline bool plat_timer_create(plat_timer_t* pTimer)
{
	if (pTimer->bUseQPF)
	{
		pTimer->llFrequency.QuadPart = 0;
		pTimer->llLastCount.QuadPart = 0;

		if (QueryPerformanceFrequency(&pTimer->llFrequency))
		{
			QueryPerformanceCounter(&pTimer->llLastCount);
			pTimer->llStartCount = pTimer->llLastCount;
		}
		else
		{
			pTimer->bUseQPF = false;
		}
	}

	if (!pTimer->bUseQPF)
	{
		TIMECAPS tc;

		timeGetDevCaps(&tc, sizeof(tc));
		pTimer->nPeriodMin = tc.wPeriodMin;
		timeBeginPeriod(pTimer->nPeriodMin);
		pTimer->nLastMilliseconds = timeGetTime();
		pTimer->nStartMilliseconds = pTimer->nLastMilliseconds;
	}

	pTimer->dBaseTime = 0.0;
	pTimer->bInit = true;
	return true;
}

// Set use high precision.
inline void plat_timer_set_high_precision(plat_timer_t* pTimer, bool flag)
{
	pTimer->bUseQPF = flag;
}

// Get current time value.
inline double plat_timer_get_current(plat_timer_t* pTimer)
{
	if (pTimer->bUseQPF)
	{
		LARGE_INTEGER ll;

		QueryPerformanceCounter(&ll);
		return (double)(ll.QuadPart - pTimer->llStartCount.QuadPart) /
			(double)pTimer->llFrequency.QuadPart;
	}
	else
	{
		DWORD cur = timeGetTime();

		if (cur < pTimer->nStartMilliseconds)
		{
			// Process time recycle.
			pTimer->dBaseTime = 
				(0xFFFFFFFF - pTimer->nStartMilliseconds) * 0.001;
			pTimer->dBaseTime += 0.001;
			pTimer->nStartMilliseconds = 0;
		}

		return (double)(cur - pTimer->nStartMilliseconds) * 0.001 + 
			pTimer->dBaseTime;
	}
}

// Get elapse seconds.
inline double plat_timer_get_elapse(plat_timer_t* pTimer, double expect)
{
	if (pTimer->bUseQPF)
	{
		LARGE_INTEGER ll;

		QueryPerformanceCounter(&ll);

		double time = (double)(ll.QuadPart - pTimer->llLastCount.QuadPart) /
			(double)pTimer->llFrequency.QuadPart;

		if (time >= expect)
		{
			pTimer->llLastCount.QuadPart = ll.QuadPart;
		}

		return time;
	}
	else
	{
		DWORD cur = timeGetTime();
		DWORD dw = cur - pTimer->nLastMilliseconds;
		double time = double(dw) * 0.001;

		if (time >= expect)
		{
			pTimer->nLastMilliseconds = cur;
		}

		return time;
	}
}

// Get elapse milliseconds.
inline int plat_timer_get_elapse_ms(plat_timer_t* pTimer, int expect)
{
	if (pTimer->bUseQPF)
	{
		LARGE_INTEGER ll;

		QueryPerformanceCounter(&ll);

		DWORD dw = DWORD((ll.QuadPart - pTimer->llLastCount.QuadPart) *
			1000 / pTimer->llFrequency.QuadPart);

		if (dw >= DWORD(expect))
		{
			pTimer->llLastCount.QuadPart = ll.QuadPart;
		}

		return (int)dw;
	}
	else
	{
		DWORD cur = timeGetTime();
		DWORD dw = cur - pTimer->nLastMilliseconds;

		if (dw >= DWORD(expect))
		{
			pTimer->nLastMilliseconds = cur;
		}

		return (int)dw;
	}
}

#endif // __WINDOWS_TIMER_H
