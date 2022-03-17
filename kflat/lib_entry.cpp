// lib_entry.cpp
// Created by lulimin on 2020/9/11.

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
void link_flat_creators(IServiceCreator** svc, IObjectCreator** obj)
{
	LINK_SERVICE(svc, FlatInputService);
	LINK_OBJECT(obj, FlatButton);
	LINK_OBJECT(obj, FlatCheckBox);
	LINK_OBJECT(obj, FlatComboBox);
	LINK_OBJECT(obj, FlatCursor);
	LINK_OBJECT(obj, FlatDataGrid);
	LINK_OBJECT(obj, FlatFont);
	LINK_OBJECT(obj, FlatForm);
	LINK_OBJECT(obj, FlatGroupBox);
	LINK_OBJECT(obj, FlatHScrollBar);
	LINK_OBJECT(obj, FlatLabel);
	LINK_OBJECT(obj, FlatListBox);
	LINK_OBJECT(obj, FlatListView);
	LINK_OBJECT(obj, FlatMainMenu);
	LINK_OBJECT(obj, FlatMenu);
	LINK_OBJECT(obj, FlatMeta);
	LINK_OBJECT(obj, FlatNumeric);
	LINK_OBJECT(obj, FlatPanel);
	LINK_OBJECT(obj, FlatPicker);
	LINK_OBJECT(obj, FlatProgressBar);
	LINK_OBJECT(obj, FlatRadioButton);
	LINK_OBJECT(obj, FlatTextBox);
	LINK_OBJECT(obj, FlatTrackBar);
	LINK_OBJECT(obj, FlatTreeView);
	LINK_OBJECT(obj, FlatUI);
	LINK_OBJECT(obj, FlatVScrollBar);
	LINK_OBJECT(obj, FlatWidget);
	LINK_OBJECT(obj, FlatWindow);
}

#endif // K_DYNAMIC_LIB
