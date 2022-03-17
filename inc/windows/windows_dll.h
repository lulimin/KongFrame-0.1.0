// windows_dll.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_DLL_H
#define __WINDOWS_DLL_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// DLL handle.
typedef HMODULE plat_dll_handle_t;

// Modify DLL name guarantee load it correctly.
inline const char* plat_dll_reform_name(const char* name, char* pBuffer,
	size_t size, const char* dll_extension)
{
	// Not need any change in windows system.
	size_t name_size = strlen(name) + 1;

	if (name_size > size)
	{
		memcpy(pBuffer, name, size - 1);
		pBuffer[size - 1] = 0;
	}
	else
	{
		memcpy(pBuffer, name, name_size);
	}

	return pBuffer;
}

// Load DLL.
inline plat_dll_handle_t plat_dll_open(const char* file_name)
{
	return LoadLibraryA(file_name);
}

// Release DLL.
inline bool plat_dll_close(plat_dll_handle_t handle)
{
	return FreeLibrary(handle) == TRUE;
}

// Get procedure address in DLL.
inline void* plat_dll_symbol(plat_dll_handle_t handle, const char* proc_name)
{
	return GetProcAddress(handle, proc_name);
}

// Get load DLL error information.
inline const char* plat_dll_error(char* pBuffer, size_t size)
{
	if (NULL == pBuffer)
	{
		return NULL;
	}
	
	int err = GetLastError();
	
	if (0 == err)
	{
		return NULL;
	}
	
	if (FormatMessageA(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		0, err, 0, pBuffer, (DWORD)size, NULL) == 0)
	{
		return NULL;
	}
		
	return pBuffer;
}

#endif // __WINDOWS_DLL_H
