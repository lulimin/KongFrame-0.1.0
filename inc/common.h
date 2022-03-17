// common.h
// Created by lulimin on 2019/2/14.

#ifndef __COMMON_H
#define __COMMON_H

#ifdef _WIN32
	#define K_PLATFORM_WINDOWS 1
	#ifdef _WIN64
		#define K_PLATFORM_64BIT 1
	#else
		#define K_PLATFORM_32BIT 1
	#endif // _WIN64
#elif __APPLE__
	#include <TargetConditionals.h>
	#if defined(TARGET_OS_IPHONE)
		#define K_PLATFORM_IOS 1
		#define K_PLATFORM_64BIT 1
	#elif defined(TARGET_OS_MAC)
		#define K_PLATFORM_MAC 1
		#define K_PLATFORM_64BIT 1
	#endif // TARGET_OS_IPHONE
#elif __ANDROID__
	#define K_PLATFORM_ANDROID 1
	#define K_PLATFORM_64BIT 1
#elif __linux__
	#define K_PLATFORM_LINUX 1
	#define K_PLATFORM_64BIT 1
#endif

#ifdef K_PLATFORM_WINDOWS
	#ifdef _WINDLL
		#define K_DYNAMIC_LIB 1
	#endif // _WINDLL
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_WINDOWS
	#include <stddef.h>
	#include <stdint.h>
	#pragma warning(disable:4786)
	#pragma warning(disable:4996)
	#pragma warning(disable:4819)
	#define K_LIB_EXPORT __declspec(dllexport)
	#define K_LIB_IMPORT __declspec(dllimport)
#endif // K_PLATFORM_WINDOWS

#if defined(K_PLATFORM_LINUX) || defined(K_PLATFORM_ANDROID)
	#include <stddef.h>
	#include <stdint.h>
	#define stricmp strcasecmp
	#define K_LIB_EXPORT __attribute__((visibility("default")))
	#define K_LIB_IMPORT 
	#define __cdecl	
#endif // K_PLATFORM_LINUX || K_PLATFORM_ANDROID

#if defined(K_PLATFORM_IOS) || defined(K_PLATFORM_MAC)
	#include <stddef.h>
	#include <stdint.h>
	#define stricmp strcasecmp
	#define K_LIB_EXPORT
	#define K_LIB_IMPORT
	#define __cdecl	
#endif // K_PLATFORM_IOS || K_PLATFORM_MAC

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if (p) { (p)->Release(); (p) = NULL; } }
#endif // !SAFE_RELEASE

#ifdef K_PLATFORM_WINDOWS
	#ifdef K_DEBUG
		#include <crtdbg.h>
		#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#else
		#define NEW ::new
	#endif // K_DEBUG
#else
	#define NEW ::new
#endif // K_PLATFORM_WINDOWS

#ifdef K_DEBUG
	void _assertion_fail(const char* msg, const char* file, int line);
	#define Assert(p) if (!(p)) { _assertion_fail(#p, __FILE__, __LINE__); }
#else
	#define Assert(p) (void(0))
#endif // K_DEBUG

#define DEFINE_ASSERTION_FAIL \
void _assertion_fail(const char* msg, const char* file, int line) \
{ \
	FILE* fp = fopen("assert.log", "ab"); \
	if (fp) \
	{ \
		fprintf(fp, \
			"Assert(%s), file:%s line:%d.\r\n", msg, file, line); \
		fclose(fp); \
	} \
	fflush(stdout); \
	fprintf(stderr, \
		"Assert(%s), file:%s line:%d.", msg, file, line); \
	fflush(stderr); \
	abort(); \
}

// Define specified class memory allocator.
#define DEFINE_SYSTEM_ALLOC(cls) \
class cls##Alloc \
{ \
public: \
	cls##Alloc() {} \
	~cls##Alloc() {} \
	void* Alloc(size_t size) { return NEW char[size]; } \
	void Free(void* ptr) { delete[] (char*)ptr; } \
};

#endif // __COMMON_H
