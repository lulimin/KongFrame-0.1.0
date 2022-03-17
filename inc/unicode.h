// unicode.h
// Created by lulimin on 2019/2/25.

#ifndef __UNICODE_H
#define __UNICODE_H

#include "common.h"
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_unicode.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_unicode.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_unicode.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_unicode.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_unicode.h"
#endif // K_PLATFORM_MAC

#endif // __UNICODE_H
