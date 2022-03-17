// net_socket.h
// Created by lulimin on 2019/2/25.

#ifndef __NET_SOCKET_H
#define __NET_SOCKET_H

#ifdef K_PLATFORM_WINDOWS
	#include "windows/windows_net.h"
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_LINUX
	#include "linux/linux_net.h"
#endif // K_PLATFORM_LINUX

#ifdef K_PLATFORM_ANDROID
	#include "linux/linux_net.h"
#endif // K_PLATFORM_ANDROID

#ifdef K_PLATFORM_IOS
	#include "apple/apple_net.h"
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	#include "apple/apple_net.h"
#endif // K_PLATFORM_MAC

#endif // __NET_SOCKET_H
