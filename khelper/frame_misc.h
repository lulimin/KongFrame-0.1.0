// frame_misc.h
// Created by lulimin on 2019/2/21.

#ifndef __FRAME_MISC_H
#define __FRAME_MISC_H

#include "../inc/frame_def.h"

// Frame misc functions.

class FrameMisc : public IObject
{
public:
	FrameMisc();
	virtual ~FrameMisc();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Get decimal string.
	ArgString GetDecimal(double value, int fractions);
	// Get object hook type.
	int GetBoundType(const uniqid_t& id);
	// Get object hook name.
	const char* GetBoundName(const uniqid_t& id);
	// Get current platform name.
	const char* GetPlatformName();
	// Whether in debug mode.
	bool GetDebugMode();
	// Get current log level.
	int GetLogLevel();
	// Set current log level.
	bool SetLogLevel(int log_level);
	// Dump frame information to file.
	bool DumpInformation(const char* info_type, const char* file_name);
	// Find file.
	bool FindFile(const char* file_name);
	// Remove file.
	bool RemoveFile(const char* file_name);
	// Find directory.
	bool FindDir(const char* dir_name);
	// Create directory.
	bool CreateDir(const char* dir_name);
	// Remove directory.
	bool RemoveDir(const char* dir_name);
};

#endif // __FRAME_MISC_H
