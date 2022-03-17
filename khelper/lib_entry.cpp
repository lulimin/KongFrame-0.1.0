// lib_entry.cpp
// Created by lulimin on 2019/2/18.

#include "../inc/common.h"
#include "../inc/frame_def.h"
#include <stdio.h>
#include <stdlib.h>

DEF_MODULE;

#ifdef K_DYNAMIC_LIB

#ifdef K_PLATFORM_WINDOWS

#include <windows.h>

// DLL entry.
bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{ 
	switch (fdwReason) 
	{ 
	case DLL_PROCESS_ATTACH: 
		break; 
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break; 
	case DLL_THREAD_DETACH: 
		break; 
	default: 
		break; 
	} 

	return TRUE; 
}

#endif // K_PLATFORM_WINDOWS

#else

// Link creators in static library.
void link_helper_creators(IServiceCreator** svc, IObjectCreator** obj)
{
	LINK_SERVICE(svc, SocketService);
	LINK_OBJECT(obj, FileFinder);
	LINK_OBJECT(obj, FrameMisc);
	LINK_OBJECT(obj, JsonDoc);
	LINK_OBJECT(obj, NodeTree);
	LINK_OBJECT(obj, TcpLink);
}

#endif // K_DYNAMIC_LIB
