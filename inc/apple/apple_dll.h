// apple_dll.h
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_DLL_H
#define __APPLE_DLL_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <time.h>
#include <ctype.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>

// DLL handle.
typedef void* plat_dll_handle_t;

// Modify DLL name for load DLL corrently.
inline const char* plat_dll_reform_name(const char* name, char* pBuffer,
	size_t size, const char* dll_extension)
{
	char* p = pBuffer;
	bool has_path = (strchr(name, '/') != NULL) || 
		(strchr(name, '\\') != NULL);
	
	if (!has_path)
	{
		// Add current directory mark.
		*p++ = '.';
		*p++ = '/';
		size -= 2;
	}

	size_t ext_len = strlen(dll_extension);

	size -= ext_len;

	size_t name_size = strlen(name) + 1;

	if (name_size > size)
	{
		memcpy(p, name, size - 1);
		p[size - 1] = 0;
	}
	else
	{
		memcpy(p, name, name_size);
	}

	if (ext_len > 0)
	{
		char* dot = strrchr(p, '.');

		if (dot)
		{
			// Replace file extension.
			memcpy(dot + 1, dll_extension, ext_len + 1);
		}
	}
	
	return pBuffer;
}

// Load DLL.
inline plat_dll_handle_t plat_dll_open(const char* file_name)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	return dlopen(sname, RTLD_NOW);
}

// Release DLL.
inline bool plat_dll_close(plat_dll_handle_t handle)
{
	return dlclose(handle) == 0;
}

// Get process address in DLL.
inline void* plat_dll_symbol(plat_dll_handle_t handle, const char* proc_name)
{
	dlerror();

	void* addr = dlsym(handle, proc_name);
	char* err = dlerror();

	if (err)
	{
		return NULL;
	}

	return addr;
}

// Get load DLL error.
inline const char* plat_dll_error(char* pBuffer, size_t size)
{
	char* err = dlerror();
	
	if (NULL == pBuffer)
	{
		return NULL; // Clear error only.
	}
	
	if (NULL == err)
	{
		return NULL;
	}
	
	size_t err_len = strlen(err);
	
	if (err_len >= size)
	{
		memcpy(pBuffer, err, size - 1);
		pBuffer[size - 1] = 0;
	}
	else
	{
		memcpy(pBuffer, err, err_len + 1);
	}
	
	return pBuffer;
}

#endif // __APPLE_DLL_H
