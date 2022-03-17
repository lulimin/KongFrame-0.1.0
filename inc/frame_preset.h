// frame_preset.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_PRESET_H
#define __FRAME_PRESET_H

#include "common.h"

class IFileAccess;
class IObjectCreator;
class IServiceCreator;

// Frame preset data.
struct frame_preset_t
{
	// Work path.
	const char* szWorkPath;
	// Script file path.
	const char* szScriptPath;
	// Coder file path.
	const char* szCoderPath;
	// Asset file path.
	const char* szAssetPath;
	// Output file path.
	const char* szOutputPath;
	// Class name of main object.
	const char* szMainObject;
	// File access interface.
	IFileAccess* pFileAccess;
	// Service creators link.
	IServiceCreator* pServiceCreator;
	// Object creators link.
	IObjectCreator* pObjectCreator;
	// Automatic reload changed script file.
	bool bScriptReload;
	// Profile script performance.
	int nScriptProfile;
};

#endif // __FRAME_PRESET_H
