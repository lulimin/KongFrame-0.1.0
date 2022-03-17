// windows_file.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_FILE_H
#define __WINDOWS_FILE_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// File path separate symbol.
#define PLAT_PATH_SEPARATOR '\\'

// File size.
struct plat_file_size_t
{
	unsigned int nSizeHigh;
	unsigned int nSizeLow;
};

// File time.
struct plat_file_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
};

// File search data.
struct plat_file_search_t
{
	HANDLE handle;
	WIN32_FIND_DATA data;
};

// Convert to standard path string.
inline const char* plat_get_normal_path(char* buffer, size_t size,
	const char* path)
{
    // Mark previus symbol is target separate symbol.
    bool slash = false;
    const char* src = path;
    char* dst = buffer;
    char* dst_end = buffer + size - 1;
    
    while (*src)
	{
		if (dst >= dst_end)
		{
			break;
		}

        if ((*src == '/') || (*src == '\\'))
		{
        	if (slash)
			{
	            ++src;
	            continue;
        	}
			else
			{
				slash = true;
			}
        	
        	*dst = '\\';
    	} 
		else
		{
   			*dst = *src;
    		slash = false;
    	}
    	
   		++dst;
   		++src;
	}
	
	*dst = 0;
	return buffer;
}

// Get full file path.
inline size_t plat_get_full_path(const char* file_name, char* buffer,
	size_t size, char** file_part)
{
	DWORD res = GetFullPathName(file_name, (DWORD)size, buffer, file_part);
	
	if (0 == res)
	{
		buffer[0] = 0;
		return 0;
	}	
	
	if ((size_t)res >= size)
	{
		// Buffer overflow.
		buffer[0] = 0;
		return 0;
	}
	
	return res;
}

// Test file exists.
inline bool plat_file_exists(const char* file_name)
{
	DWORD res = GetFileAttributes(file_name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// Open file.
inline FILE* plat_file_open(const char* file_name, const char* mode)
{
	return fopen(file_name, mode);
}

// Delete file.
inline bool plat_file_delete(const char* file_name)
{
	return DeleteFile(file_name) == TRUE;
}

// Get file size.
inline bool plat_get_file_size(const char* file_name, 
	plat_file_size_t* file_size)
{
	HANDLE fp = CreateFile(file_name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == fp)
	{
		return false;
	}

	DWORD size_high;
	DWORD size_low = GetFileSize(fp, &size_high);

	CloseHandle(fp);
	file_size->nSizeHigh = size_high;
	file_size->nSizeLow = size_low;
	return true;
}

// Get file last modified time.
inline bool plat_get_file_time(const char* file_name, 
	plat_file_time_t* file_time)
{
	HANDLE fp = CreateFile(file_name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == fp)
	{
		return false;
	}

	FILETIME ft;

	if (!GetFileTime(fp, NULL, NULL, &ft))
	{
		CloseHandle(fp);
		return false;
	}

	CloseHandle(fp);

	SYSTEMTIME st;

	// Set to UTC time.
	FileTimeToSystemTime(&ft, &st);
	file_time->nYear = st.wYear;
	file_time->nMonth = st.wMonth;
	file_time->nDay = st.wDay;
	file_time->nHour = st.wHour;
	file_time->nMinute = st.wMinute;
	file_time->nSecond = st.wSecond;
	return true;
}

// Set file time
inline bool plat_set_file_time(const char* file_name, 
	const plat_file_time_t* file_time)
{
	HANDLE handle = CreateFile(file_name, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	if (INVALID_HANDLE_VALUE == handle)
	{
		return false;
	}
	
	SYSTEMTIME st;
	FILETIME ft;
	
	memset(&st, 0, sizeof(st));
	st.wYear = file_time->nYear;
	st.wMonth = file_time->nMonth;
	st.wDay = file_time->nDay;
	st.wHour = file_time->nHour;
	st.wMinute = file_time->nMinute;
	st.wSecond = file_time->nSecond;
	
	SystemTimeToFileTime(&st, &ft);
	
	if (!SetFileTime(handle, &ft, &ft, &ft))
	{
		CloseHandle(handle);
		return false;
	}
	
	CloseHandle(handle);
	return true;
}

// Change file attribute, guarantee file can modify.
inline bool plat_enable_file_modify(const char* file_name)
{
	return SetFileAttributes(file_name, FILE_ATTRIBUTE_ARCHIVE) == TRUE;
}

// Get current work directory.
inline bool plat_get_current_dir(char* buffer, size_t size)
{
	DWORD res = GetCurrentDirectory((DWORD)size, buffer);

	if (0 == res)
	{
		buffer[0] = 0;
		return false;
	}

	if ((size_t)res >= size)
	{
		// Buffer overflow.
		buffer[0] = 0;
		return false;
	}

	return true;
}

// Set current work directory.
inline bool plat_set_current_dir(const char* dir_name)
{
	return SetCurrentDirectory(dir_name) == TRUE;
}

// Test directory exists.
inline bool plat_find_dir(const char* dir_name)
{
	DWORD res = GetFileAttributes(dir_name);

	if (res == DWORD(-1))
	{
		return false;
	}

	return (res & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// Create directory.
inline bool plat_create_dir(const char* dir_name)
{
	return CreateDirectory(dir_name, NULL) == TRUE;
}

// Delete directory.
inline bool plat_delete_dir(const char* dir_name)
{
	return RemoveDirectory(dir_name) == TRUE;
}

// Start search file.
inline bool plat_find_first(plat_file_search_t* file_search, const char* path,
	const char* name)
{
	char fname[512];
	
	if (0 == name[0])
	{
		// Search all.
		_snprintf(fname, sizeof(fname) - 1, "%s*.*", path);
		fname[sizeof(fname) - 1] = 0;
	}
	else
	{
		if (strchr(name, '.') != NULL)
		{
			// Specify match symbol.
			_snprintf(fname, sizeof(fname) - 1, "%s%s", path, name);
			fname[sizeof(fname) - 1] = 0;
		}
		else
		{
			// Specify file extension.
			_snprintf(fname, sizeof(fname) - 1, "%s*.%s", path, name);
			fname[sizeof(fname) - 1] = 0;
		}
	}
	
	file_search->handle = FindFirstFile(fname, &file_search->data);
	
	if (INVALID_HANDLE_VALUE == file_search->handle)
	{
		return false;
	}
	
	return true;
}

// Search next file.
inline bool plat_find_next(plat_file_search_t* file_search)
{
	return FindNextFile(file_search->handle, &file_search->data) == TRUE;
}

// End search file.
inline bool plat_find_close(plat_file_search_t* file_search)
{
	return FindClose(file_search->handle) == TRUE;
}

// Test found object is file.
inline bool plat_get_find_is_file(plat_file_search_t* file_search)
{
	return (file_search->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// Test found object is directory.
inline bool plat_get_find_is_dir(plat_file_search_t* file_search)
{
	return (file_search->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// Get found file name.
inline const char* plat_get_find_name(plat_file_search_t* file_search)
{
	return file_search->data.cFileName;
}

#endif // __WINDOWS_FILE_H
