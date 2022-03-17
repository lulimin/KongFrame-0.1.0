// i_file_access.h
// Created by lulimin on 2019/2/17.

#ifndef __I_FILE_ACCESS_H
#define __I_FILE_ACCESS_H

#include "common.h"
#include <stdarg.h>

// File access interface.

class IFileAccess
{
public:
	// File time.
	struct file_time_t
	{
		int nYear;
		int nMonth;
		int nDay;
		int nHour;
		int nMinute;
		int nSecond;
	};

	enum
	{
		_SEEK_SET = 0,
		_SEEK_CUR = 1,
		_SEEK_END = 2
	};
	
public:
	virtual ~IFileAccess() {}

	// Destroy self.
	virtual void Release() = 0;

	// Test directory exists.
	virtual bool DirectoryExists(const char* name) = 0;
	// Create directory.
	virtual bool DirectoryCreate(const char* name) = 0;
	// Delete directory.
	virtual bool DirectoryDelete(const char* name) = 0;

	// Test file exists.
	virtual bool FileExists(const char* name) = 0;
	// Delete file.
	virtual bool FileDelete(const char* name) = 0;
	// Get file size.
	virtual uint64_t FileSize(const char* name) = 0;
	// Get file time.
	virtual bool FileTime(const char* name, file_time_t* file_time) = 0;
	// Open file.
	virtual void* FileOpen(const char* name, const char* mode) = 0;
	// Close file.
	virtual bool FileClose(void* handle) = 0;
	// Move file access pointer.
	virtual bool FileSeek(void* handle, long offset, int origin) = 0;
	// Get position of file access pointer.
	virtual size_t FileTell(void* handle) = 0;
	// Read file.
	virtual size_t FileRead(void* handle, void* pdata, size_t len) = 0;
	// Write file.
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len) = 0;
	// Formatted write file.
	virtual size_t __cdecl FilePrint(void* handle, const char* info, ...) = 0;
	// Formatted write file.
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vl) = 0;
	// Flush file data.
	virtual bool FileFlush(void* handle) = 0;
};

#endif // __I_FILE_ACCESS_H
