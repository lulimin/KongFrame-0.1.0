// arg_reader.h 
// Created by lulimin on 2019/2/14.

#ifndef __ARG_READER_H
#define __ARG_READER_H

#include "uniqid.h"
#include "i_arg_data.h"
#include "i_arg_list.h"
#include "arg_string.h"

// TArgReader

template<typename TYPE>
struct TArgReader
{
	static TYPE Get(const IArgData& vl)
	{ 
		// Cannot use this default method.
		Assert(0);
	}
	
	static TYPE Get(const IArgList& vl, size_t index)
	{ 
		// Cannot use this default method.
		Assert(0);
	}
};

template<>
struct TArgReader<bool>
{
	static bool Get(const IArgData& v)
	{
		return v.GetBoolean();
	}
	
	static bool Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetBoolean(index); 
	}
};

template<>
struct TArgReader<char>
{
	static char Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static char Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<unsigned char>
{
	static unsigned char Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static unsigned char Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<short>
{
	static short Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static short Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<unsigned short>
{
	static unsigned short Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static unsigned short Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<int>
{
	static int Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static int Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<unsigned int>
{
	static unsigned int Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static unsigned int Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
};

template<>
struct TArgReader<long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	static long Get(const IArgData& v)
	{
		return v.GetInt32();
	}
	
	static long Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
#else
	static long Get(const IArgData& v)
	{
		return (long)v.GetInt64();
	}

	static long Get(const IArgList& vl, size_t index)
	{ 
		return (long)vl.GetInt64(index); 
	}
#endif 
};

template<>
struct TArgReader<unsigned long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	static unsigned long Get(const IArgData& v)
	{
		return v.GetInt32();
	}

	static unsigned long Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt32(index); 
	}
#else
	static unsigned long Get(const IArgData& v)
	{
		return (unsigned long)v.GetInt64();
	}

	static unsigned long Get(const IArgList& vl, size_t index)
	{ 
		return (unsigned long)vl.GetInt64(index); 
	}
#endif
};

#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
template<>
struct TArgReader<int64_t>
{
	static int64_t Get(const IArgData& v)
	{
		return v.GetInt64();
	}
	
	static int64_t Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt64(index); 
	}
};

template<>
struct TArgReader<uint64_t>
{
	static uint64_t Get(const IArgData& v)
	{
		return v.GetInt64();
	}
	
	static uint64_t Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetInt64(index); 
	}
};
#endif

template<>
struct TArgReader<float>
{
	static float Get(const IArgData& v)
	{
		return v.GetFloat();
	}
	
	static float Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetFloat(index); 
	}
};

template<>
struct TArgReader<double>
{
	static double Get(const IArgData& v)
	{
		return v.GetDouble();
	}
	
	static double Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetDouble(index); 
	}
};

template<>
struct TArgReader<const char*>
{
	static const char* Get(const IArgData& v)
	{
		return v.GetString();
	}
	
	static const char* Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetString(index); 
	}
};

template<>
struct TArgReader<ArgString>
{
	static const char* Get(const IArgData& v)
	{
		return v.GetString();
	}

	static const char* Get(const IArgList& vl, size_t index)
	{
		return vl.GetString(index);
	}
};

template<>
struct TArgReader<const ArgString&>
{
	static const char* Get(const IArgData& v)
	{
		return v.GetString();
	}

	static const char* Get(const IArgList& vl, size_t index)
	{
		return vl.GetString(index);
	}
};

template<>
struct TArgReader<uniqid_t>
{
	static uniqid_t Get(const IArgData& v)
	{
		return v.GetUniqid();
	}
	
	static uniqid_t Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetUniqid(index); 
	}
};

template<>
struct TArgReader<const uniqid_t& >
{
	static uniqid_t Get(const IArgData& v)
	{
		return v.GetUniqid();
	}
	
	static uniqid_t Get(const IArgList& vl, size_t index)
	{ 
		return vl.GetUniqid(index); 
	}
};

#endif // __ARG_READER_H
