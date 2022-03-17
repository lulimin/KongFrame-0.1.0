// memory_map.h
// Created by lulimin on 2019/2/14.

#ifndef __MEMORY_MAP_H
#define __MEMORY_MAP_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_mmap.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_mmap.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	// Not support.
	//#include "linux/linux_mmap.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_mmap.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_mmap.h"
#endif // K_PLATFORM_MAC

	// Global memory map.

class MemoryMap
{
public:
	// Test specified name share memory exists.
	static bool Exists(const char* name)
	{
		return plat_mmap_exists(name);
	}

public:
	MemoryMap()
	{
		plat_mmap_initialize(&m_Map);
	}

	~MemoryMap()
	{
		plat_mmap_shutdown(&m_Map);
	}

	// Get memory size.
	size_t GetSize()
	{
		return plat_mmap_get_size(&m_Map);
	}

	// Get memory address.
	void* GetMemory()
	{
		return plat_mmap_get_memory(&m_Map);
	}

	// Test memory validate.
	bool IsValid()
	{
		return plat_mmap_is_valid(&m_Map);
	}

	// Get or create share memory.
	bool Create(const char* name, size_t size, bool* exists)
	{
		return plat_mmap_create(&m_Map, name, size, exists);
	}

	// Delete share memory.
	bool Destroy()
	{
		return plat_mmap_shutdown(&m_Map);
	}

private:
	MemoryMap(const MemoryMap&);
	MemoryMap& operator=(const MemoryMap&);

private:
	plat_mmap_t m_Map;
};

#endif // __MEMORY_MAP_H
