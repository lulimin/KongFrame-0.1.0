// apple_platform.h
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_PLATFORM_H
#define __APPLE_PLATFORM_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

// Date and time.
struct plat_date_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
	int nMilliseconds;
	int nDayOfWeek;
};

// Get locale current time.
inline bool plat_get_local_time(plat_date_time_t* dt)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);

	tm* pt = localtime(&tv.tv_sec);
	
	dt->nYear = pt->tm_year + 1900;
	dt->nMonth = pt->tm_mon + 1;
	dt->nDay = pt->tm_mday;
	dt->nHour = pt->tm_hour;
	dt->nMinute = pt->tm_min;
	dt->nSecond = pt->tm_sec;
	dt->nMilliseconds = tv.tv_usec / 1000;
	dt->nDayOfWeek = pt->tm_wday;
	return true;
}

// Get system time tick.
inline double plat_get_tick_count()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0;
}

// Get current high precision time tick.
inline double plat_get_time_count()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

// Start console program.
inline bool plat_launch(const char* cmd_line)
{
	char cmd[256];
	size_t size = strlen(cmd_line) + 1;
	
	if (size > sizeof(cmd))
	{
		return false;
	}
	
	memcpy(cmd, cmd_line, size);
	
	char* argv[16] = { NULL };
	char* envp[1] = { NULL };
	int argc = 0;
	
	argv[argc++] = cmd;
	
	char* p = strchr(cmd, ' ');
	
	while (p)
	{
		*p = 0;
		
		while (*(++p) == ' ')
		{
			*p = 0;
		}

		argv[argc++] = p;
		
		if (argc == (sizeof(argv) / sizeof(argv[0]) - 1))
		{
			break;
		}
	}
	
	int pid = fork();
	
	if (0 == pid)
	{
		// Child process.
		if (execve(cmd, argv, envp) == -1)
		{
			exit(0); // Start process failed, close child process.
		}
	}
	
	return pid > 0;
}

// Kill process.
inline bool plat_kill_process(unsigned int proc_id)
{
	return kill(proc_id, SIGKILL);
}

// Get current process id.
inline unsigned int plat_get_process_id()
{
	return getpid();
}

// Get current execute file path.
inline bool plat_get_process_file(char* buffer, size_t size)
{
	ssize_t res = readlink("/proc/self/exe", buffer, size);

	if ((res < 0) || (res >= size))
	{
		buffer[0] = 0;
		return false;
	}

	buffer[res] = 0;
	return true;
}

// System sleep.
inline void plat_sleep(unsigned int ms)
{
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000000;
	nanosleep(&ts, NULL);
}

// Test illegal float number.
inline int plat_is_nan(float value)
{
	return isnan(value);
}

// Display information.
inline void plat_popup_message(const char* info, const char* title)
{
	printf("(message)%s:%s\n", title, info);
}

// Display error information.
inline void plat_popup_error(const char* info, const char* title)
{
	printf("(error)%s:%s\n", title, info);
}

#endif // __APPLE_PLATFORM_H
