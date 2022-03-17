// windows_platform.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_PLATFORM_H
#define __WINDOWS_PLATFORM_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>

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
	SYSTEMTIME st;
	
	GetLocalTime(&st);
	dt->nYear = st.wYear;
	dt->nMonth = st.wMonth;
	dt->nDay = st.wDay;
	dt->nHour = st.wHour;
	dt->nMinute = st.wMinute;
	dt->nSecond = st.wSecond;
	dt->nMilliseconds = st.wMilliseconds;
	dt->nDayOfWeek = st.wDayOfWeek;
	return true;
}

// Get system tick number.
inline double plat_get_tick_count()
{
	return (double)GetTickCount64();
}

// Get current high precision time.
inline double plat_get_time_count()
{
	static LARGE_INTEGER s_u64Freq;
	static BOOL s_init = QueryPerformanceFrequency(&s_u64Freq);

	LARGE_INTEGER u64Tick;

	QueryPerformanceCounter(&u64Tick);
	return (double)u64Tick.QuadPart / (double)s_u64Freq.QuadPart;
}

// Start console application.
inline bool plat_launch(const char* cmd_line)
{
	char cmd[256];
	size_t size = strlen(cmd_line) + 1;
	
	if (size > sizeof(cmd))
	{
		return false;
	}
	
	memcpy(cmd, cmd_line, size);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	
	BOOL res = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, 
		NULL, NULL, &si, &pi);

	if (res)
	{
		// Release resources.
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	
	return res == TRUE;
}

// Kill process.
inline bool plat_kill_process(unsigned int proc_id)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, proc_id);

	if (NULL == hProcess)
	{
		return false;
	}

	if (!TerminateProcess(hProcess, 0))
	{
		CloseHandle(hProcess);
		return false;
	}

	CloseHandle(hProcess);
	return true;
}

// Get current process id.
inline unsigned int plat_get_process_id()
{
	return GetCurrentProcessId();
}

// Get current execute file name.
inline bool plat_get_process_file(char* buffer, size_t size)
{
	if (GetModuleFileName(NULL, buffer, (DWORD)size) == 0)
	{
		buffer[0] = 0;
		return false;
	}

	return true;
}

// System sleep.
inline void plat_sleep(unsigned int ms)
{
	Sleep(ms);
}

// Test illegal float value.
inline int plat_is_nan(float value)
{
	return _isnan(value);
}

// Display information.
inline void plat_popup_message(const char* info, const char* title)
{
	MessageBox(NULL, info, title, MB_OK);
}

// Display error information.
inline void plat_popup_error(const char* info, const char* title)
{
	MessageBox(NULL, info, title, MB_OK | MB_ICONERROR);
}

#endif // __WINDOWS_PLATFORM_H
