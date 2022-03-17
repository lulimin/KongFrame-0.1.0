// platform.h
// Created by lulimin on 2019/2/14.

#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "common.h"

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_platform.h"
	#include "windows/windows_file.h"
	#include "windows/windows_dll.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_platform.h"
	#include "linux/linux_file.h"
	#include "linux/linux_dll.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_platform.h"
	#include "linux/linux_file.h"
	#include "linux/linux_dll.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_platform.h"
	#include "apple/apple_file.h"
	#include "apple/apple_dll.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_platform.h"
	#include "apple/apple_file.h"
	#include "apple/apple_dll.h"
#endif // K_PLATFORM_MAC

#endif // __PLATFORM_H
