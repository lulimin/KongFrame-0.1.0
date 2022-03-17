// pod_hash.h
// Created by lulimin on 2019/2/14.

#ifndef __POD_HASH_H
#define __POD_HASH_H

#include "common.h"
#include "uniqid.h"
#include "utility.h"

// TPodHash

template<typename TYPE>
class TPodHash
{
public:
	static size_t HashCode(const TYPE& value);
	static bool ValueEqual(const TYPE& v1, const TYPE& v2);
};

template<>
class TPodHash<int>
{
public:
	static size_t HashCode(const int& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const int& v1, const int& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<unsigned int>
{
public:
	static size_t HashCode(const unsigned int& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const unsigned int& v1, const unsigned int& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<int64_t>
{
public:
	static size_t HashCode(const int64_t& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const int64_t& v1, const int64_t& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<uint64_t>
{
public:
	static size_t HashCode(const uint64_t& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const uint64_t& v1, const uint64_t& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<short>
{
public:
	static size_t HashCode(const short& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const short& v1, const short& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<unsigned short>
{
public:
	static size_t HashCode(const unsigned short& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const unsigned short& v1, const unsigned short& v2)
	{
		return v1 == v2;
	}
};

template<>
class TPodHash<char>
{
public:
	static size_t HashCode(const char& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const char& s1, const char& s2)
	{
		return s1 == s2;
	}
};

template<>
class TPodHash<unsigned char>
{
public:
	static size_t HashCode(const unsigned char& value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const unsigned char& s1, const unsigned char& s2)
	{
		return s1 == s2;
	}
};

template<>
class TPodHash<void*>
{
public:
	static size_t HashCode(const void* value)
	{
		return (size_t)value;
	}

	static bool ValueEqual(const void* s1, const void* s2)
	{
		return s1 == s2;
	}
};

template<>
class TPodHash<uniqid_t>
{
public:
	static size_t HashCode(const uniqid_t& value)
	{
		return (size_t)(value.nIndex + value.nOrder);
	}

	static bool ValueEqual(const uniqid_t& s1, const uniqid_t& s2)
	{
		return uniqid_equal(s1, s2);
	}
};

template<>
class TPodHash<const char*>
{
public:
	static size_t HashCode(const char* value)
	{
		return util_string_hash(value);
	}

	static bool ValueEqual(const char* s1, const char* s2)
	{
		return strcmp(s1, s2) == 0;
	}
};

template<typename TYPE>
class TPodHashCI : public TPodHash<TYPE>
{
};

template<>
class TPodHashCI<const char*>
{
public:
	static size_t HashCode(const char* value)
	{
		return util_string_hash_ci(value);
	}

	static bool ValueEqual(const char* s1, const char* s2)
	{
		return stricmp(s1, s2) == 0;
	}
};

#endif // __POD_HASH_H
