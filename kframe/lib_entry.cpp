// lib_entry.cpp
// Created by lulimin on 2019/2/18.

#include "../inc/common.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(K_PLATFORM_WINDOWS) && defined(K_DYNAMIC_LIB)

DEFINE_ASSERTION_FAIL;

#include <windows.h>

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

#endif // K_PLATFORM_WINDOWS && K_DYNAMIC_LIB
