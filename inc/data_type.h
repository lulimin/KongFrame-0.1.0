// data_type.h
// Created by lulimin on 2019/2/14.

#ifndef __DATA_TYPE_H
#define __DATA_TYPE_H

#include "common.h"
#include "uniqid.h"

// Data types.

#define DT_NULL 0 // Unknown.
#define DT_BOOLEAN 1 // Boolean.
#define DT_INT32 2 // 32-bit integer.
#define DT_INT64 3 // 64-bit integer.
#define DT_FLOAT 4 // Single precision decimal.
#define DT_DOUBLE 5 // Double precision decimal.
#define DT_STRING 6 // String ended by zero.
#define DT_UNIQID 7 // Object id.
#define DT_POINTER 8 // Pointer.

// Data type traits.

template<typename TYPE>
struct TDataType
{
	enum { Type = DT_NULL };
};

template<>
struct TDataType<bool>
{
	enum { Type = DT_BOOLEAN };
};

template<>
struct TDataType<char>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<unsigned char>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<short>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<unsigned short>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<int>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<unsigned int>
{
	enum { Type = DT_INT32 };
};

template<>
struct TDataType<long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	enum { Type = DT_INT32 };
#else
	enum { Type = DT_INT64 };
#endif
};

template<>
struct TDataType<unsigned long>
{
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	enum { Type = DT_INT32 };
#else
	enum { Type = DT_INT64 };
#endif
};

#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
template<>
struct TDataType<int64_t>
{
	enum { Type = DT_INT64 };
};

template<>
struct TDataType<uint64_t>
{
	enum { Type = DT_INT64 };
};
#endif

template<>
struct TDataType<float>
{
	enum { Type = DT_FLOAT };
};

template<>
struct TDataType<double>
{
	enum { Type = DT_DOUBLE };
};

template<>
struct TDataType<char*>
{
	enum { Type = DT_STRING };
};

template<>
struct TDataType<const char*>
{
	enum { Type = DT_STRING };
};

template<>
struct TDataType<uniqid_t>
{
	enum { Type = DT_UNIQID };
};

template<>
struct TDataType<const uniqid_t&>
{
	enum { Type = DT_UNIQID };
};

template<>
struct TDataType<void*>
{
	enum { Type = DT_POINTER };
};

template<>
struct TDataType<const void*>
{
	enum { Type = DT_POINTER };
};

#endif // __DATA_TYPE_H
