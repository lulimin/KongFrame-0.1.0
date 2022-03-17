// tick_counter.h
// Created by lulimin on 2019/2/14.

#ifndef __TICK_COUNTER_H
#define __TICK_COUNTER_H

#include "platform.h"

// Low precision timer.

class TickCounter
{
public:
	TickCounter()
	{
		m_dLastMilliseconds = plat_get_tick_count();
	}
	
	// Reset.
	void Restart()
	{
		m_dLastMilliseconds = plat_get_tick_count();
	}
	
	// Get elapse milliseconds from start point.
	double GetStartedMilliseconds() const
	{
		return plat_get_tick_count() - m_dLastMilliseconds;
	}
	
	// Get elapse milliseconds.
	double GetElapseMilliseconds(double expect)
	{
		double cur = plat_get_tick_count();
		double elapse = cur - m_dLastMilliseconds;
			
		if (elapse >= expect)
		{
			m_dLastMilliseconds = cur;
		}
			
		return elapse;
	}

private:
	double m_dLastMilliseconds;
};

#endif // __TICK_COUNTER_H
