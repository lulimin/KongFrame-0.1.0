// apple_string.h
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_STRING_H
#define __APPLE_STRING_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

// Get lower case string.
inline char* plat_string_lower(char* str)
{
	char* p = str;
	
	while (*p)
	{
		*p = tolower(*p);
		p++;
	}

	return str;
}

// Get upper case string.
inline char* plat_string_upper(char* str)
{
	char* p = str;

	while (*p)
	{
		*p = toupper(*p);
		p++;
	}

	return str;
}

// Format string.
inline size_t plat_safe_sprintf(char* dst, size_t dst_size, 
	const char* src, ...)
{
	va_list args;
	
	va_start(args, src);
	
	size_t size_1 = dst_size - 1;
	int res = vsnprintf(dst, size_1, src, args);

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
	int res = vsnprintf(dst, size_1, src, args);
	
	if ((size_t)res >= size_1)
	{
		dst[size_1] = 0;
		return size_1;
	}
	
	return res;
}

// String convert to 64-bit integer.
inline int64_t plat_string_to_int64(const char* str)
{
	return atoll(str);
}

// 64-bit integer convert to string.
inline void plat_int64_to_string(char* dst, size_t dst_size, int64_t val)
{
	snprintf(dst, dst_size - 1, "%lld", val);
	dst[dst_size - 1] = 0;
}

#endif // __APPLE_STRING_H
