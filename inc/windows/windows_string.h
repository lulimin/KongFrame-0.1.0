// windows_string.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_STRING_H
#define __WINDOWS_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// String lower case.
inline char* plat_string_lower(char* str)
{
	return _strlwr(str);
}

// String upper case.
inline char* plat_string_upper(char* str)
{
	return _strupr(str);
}

// Format string.
inline size_t plat_safe_sprintf(char* dst, size_t dst_size, 
	const char* src, ...)
{
	va_list args;
	
	va_start(args, src);
	
	size_t size_1 = dst_size - 1;
	int res = _vsnprintf(dst, size_1, src, args);

	va_end(args);
	
	if ((size_t)res >= size_1)
	{
		dst[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// Format string.
inline size_t plat_safe_sprint_list(char* dst, size_t dst_size, 
	const char* src, va_list args)
{
	size_t size_1 = dst_size - 1;
	int res = _vsnprintf(dst, size_1, src, args);
	
	if ((size_t)res >= size_1)
	{
		dst[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// String convert 64-bit integer.
inline __int64 plat_string_to_int64(const char* str)
{
	return _atoi64(str);
}

// 64-bit integer convert to string.
inline void plat_int64_to_string(char* dst, size_t dst_size, __int64 val)
{
	_snprintf(dst, dst_size - 1, "%I64d", val);
	dst[dst_size - 1] = 0;
}

#endif // __WINDOWS_STRING_H
