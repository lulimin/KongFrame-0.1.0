// arg_writer.h 
// Created by lulimin on 2019/2/17.

#ifndef __ARG_WRITER_H
#define __ARG_WRITER_H

#include "uniqid.h"
#include "i_arg_data.h"
#include "i_arg_list.h"
#include "arg_string.h"

// TArgWriter

template<typename TYPE>
struct TArgWriter
{
	static void Set(IArgData* v, TYPE value)
	{ 
		// Cannot use this default method.
		Assert(0);
	}

	static void Set(IArgList* vl, TYPE value)
	{
		// Cannot use this default method.
		Assert(0);
	}
};

template<>
struct TArgWriter<bool>
{
	static void Set(IArgData* v, bool value)
	{ 
		v->SetBoolean(value);
	}
	
	static void Set(IArgList* vl, bool value)
	{
		vl->AddBoolean(value);
	}
};

template<>
struct TArgWriter<char>
{
	static void Set(IArgData* v, char value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, char value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<unsigned char>
{
	static void Set(IArgData* v, unsigned char value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, unsigned char value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<short> 
{
	static void Set(IArgData* v, short value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, short value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<unsigned short> 
{
	static void Set(IArgData* v, unsigned short value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, unsigned short value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<int>
{
	static void Set(IArgData* v, int value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, int value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<unsigned int>
{
	static void Set(IArgData* v, unsigned int value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, unsigned int value)
	{
		vl->AddInt32(value);
	}
};

template<>
struct TArgWriter<long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	static void Set(IArgData* v, long value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, long value)
	{
		vl->AddInt32(value);
	}
#else
	static void Set(IArgData* v, long value)
	{ 
		v->SetInt64(value);
	}

	static void Set(IArgList* vl, long value)
	{
		vl->AddInt64(value);
	}
#endif 
};

template<>
struct TArgWriter<unsigned long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	static void Set(IArgData* v, unsigned long value)
	{ 
		v->SetInt32(value);
	}
	
	static void Set(IArgList* vl, unsigned long value)
	{
		vl->AddInt32(value);
	}
#else
	static void Set(IArgData* v, unsigned long value)
	{ 
		v->SetInt64(value);
	}

	static void Set(IArgList* vl, unsigned long value)
	{
		vl->AddInt64(value);
	}
#endif 
};

#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
template<>
struct TArgWriter<int64_t>
{
	static void Set(IArgData* v, int64_t value)
	{ 
		v->SetInt64(value);
	}
	
	static void Set(IArgList* vl, int64_t value)
	{
		vl->AddInt64(value);
	}
};

template<>
struct TArgWriter<uint64_t>
{
	static void Set(IArgData* v, uint64_t value)
	{ 
		v->SetInt64(value);
	}
	
	static void Set(IArgList* vl, uint64_t value)
	{
		vl->AddInt64(value);
	}
};
#endif 

template<>
struct TArgWriter<float>
{
	static void Set(IArgData* v, float value)
	{ 
		v->SetFloat(value);
	}
	
	static void Set(IArgList* vl, float value)
	{
		vl->AddFloat(value);
	}
};

template<>
struct TArgWriter<double>
{
	static void Set(IArgData* v, double value)
	{ 
		v->SetDouble(value);
	}
	
	static void Set(IArgList* vl, double value)
	{
		vl->AddDouble(value);
	}
};

template<>
struct TArgWriter<char*>
{
	static void Set(IArgData* v, char* value)
	{ 
		v->SetString(value);
	}
	
	static void Set(IArgList* vl, char* value)
	{
		vl->AddString(value);
	}
};

template<>
struct TArgWriter<const char*>
{
	static void Set(IArgData* v, const char* value)
	{ 
		v->SetString(value);
	}
	
	static void Set(IArgList* vl, const char* value)
	{
		vl->AddString(value);
	}
};

template<>
struct TArgWriter<ArgString>
{
	static void Set(IArgData* v, ArgString value)
	{
		v->SetString(value.CString());
	}

	static void Set(IArgList* vl, ArgString value)
	{
		vl->AddString(value.CString());
	}
};

template<>
struct TArgWriter<const ArgString&>
{
	static void Set(IArgData* v, const ArgString& value)
	{
		v->SetString(value.CString());
	}

	static void Set(IArgList* vl, const ArgString& value)
	{
		vl->AddString(value.CString());
	}
};

template<>
struct TArgWriter<const uniqid_t&>
{
	static void Set(IArgData* v, const uniqid_t& value)
	{ 
		v->SetUniqid(value);
	}
	
	static void Set(IArgList* vl, const uniqid_t& value)
	{
		vl->AddUniqid(value);
	}
};

template<>
struct TArgWriter<uniqid_t>
{
	static void Set(IArgData* v, uniqid_t value)
	{ 
		v->SetUniqid(value);
	}
	
	static void Set(IArgList* vl, uniqid_t value)
	{
		vl->AddUniqid(value);
	}
};

#endif // __ARG_WRITER_H
