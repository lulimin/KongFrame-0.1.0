// linux_timer.h
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_TIMER_H
#define __LINUX_TIMER_H

#include <sys/time.h>

// High precision timer.
struct plat_timer_t
{
	bool bInit;
	double dLastTime;
};

// Initialize timer.
inline bool plat_timer_initialize(plat_timer_t* pTimer)
{
	pTimer->bInit = false;
	pTimer->dLastTime = 0.0;
	return true;
}

// Shutdown timer.
inline bool plat_timer_shutdown(plat_timer_t* pTimer)
{
	return true;
}

// Convert to seconds.
inline double plat_timer_to_seconds(struct timeval* tv)
{
	return (double)tv->tv_sec + (double)tv->tv_usec * 0.000001;
}

// Create timer.
inline bool plat_timer_create(plat_timer_t* pTimer)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	pTimer->dLastTime = plat_timer_to_seconds(&tv);
	pTimer->bInit = true;
	return true;
}

// Set use high precision timer.
inline void plat_timer_set_high_precision(plat_timer_t* pTimer, bool flag)
{
}

// Get current time.
inline double plat_timer_get_current(plat_timer_t* pTimer)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return plat_timer_to_seconds(&tv);
}

// Get elapse seconds.
inline double plat_timer_get_elapse(plat_timer_t* pTimer, double expect)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	double now = plat_timer_to_seconds(&tv);
	double elapse = now - pTimer->dLastTime;

	if (elapse >= expect)
	{
		pTimer->dLastTime = now;
	}

	return elapse;
}

// Get elapse milliseconds.
inline int plat_timer_get_elapse_ms(plat_timer_t* pTimer, int expect)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	double now = plat_timer_to_seconds(&tv);
	double elapse = now - pTimer->dLastTime;

	if (elapse >= expect)
	{
		pTimer->dLastTime = now;
	}

	return (int)(elapse * 1000.0);
}

#endif // __LINUX_TIMER_H
