// char_traits.h
// Created by lulimin on 2019/2/14.

#ifndef __CHAR_TRAITS_H
#define __CHAR_TRAITS_H

#include "common.h"
#include "utility.h"
#include <string.h>

// TCharTraits

template<typename TYPE>
struct TCharTraits
{
};

template<>
struct TCharTraits<char>
{
	// Hash value of string.
	static size_t HashCode(const char* s)
	{
		return util_string_hash(s);
	}

	// Compare string.
	static int Compare(const char* s1, const char* s2)
	{
		return strcmp(s1, s2);
	}
	
	// Test string equalize.
	static bool Equal(const char* s1, const char* s2)
	{
		return strcmp(s1, s2) == 0;
	}

	// Length of string.
	static size_t Length(const char* s)
	{
		return strlen(s);
	}

	// Copy string.
	static void Copy(char* dst, const char* src, size_t len)
	{
		memcpy(dst, src, len);
	}
	
	// Find position of string.
	static size_t Find(const char* dst, const char* find, size_t begin = 0)
	{
		const char* pos = strstr(&dst[begin], find);

		if (NULL == pos)
		{
			return (size_t)(-1);
		}

		return (size_t)(pos - dst);
	}
};

template<typename TYPE>
class TCharTraitsCI : public TCharTraits<TYPE>
{
};

template<>
class TCharTraitsCI<char> : public TCharTraits<char>
{
public:
	// Hash value of string.
	static size_t HashCode(const char* s)
	{
		return util_string_hash_ci(s);
	}

	// Compare string.
	static int Compare(const char* s1, const char* s2)
	{
		return stricmp(s1, s2);
	}

	// Test string equalize.
	static bool Equal(const char* s1, const char* s2)
	{
		return stricmp(s1, s2) == 0;
	}
};

#endif // __CHAR_TRAITS_H
