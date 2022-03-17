// frame_file_access.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_FILE_ACCESS_H
#define __FRAME_FILE_ACCESS_H

#include "../inc/common.h"
#include "../inc/i_file_access.h"

// Default file access.

class FrameFileAccess : public IFileAccess
{
public:
	static FrameFileAccess* CreateInstance();

public:
	FrameFileAccess();
	virtual ~FrameFileAccess();

	// Destroy self.
	virtual void Release();
	
	// Test directory exists.
	virtual bool DirectoryExists(const char* name);
	// Create directory.
	virtual bool DirectoryCreate(const char* name);
	// Delete directory.
	virtual bool DirectoryDelete(const char* name);

	// Test file exists.
	virtual bool FileExists(const char* name);
	// Delete file.
	virtual bool FileDelete(const char* name);
	// Get file byte size.
	virtual uint64_t FileSize(const char* name);
	// Get file date & time.
	virtual bool FileTime(const char* name, file_time_t* file_time);
	// Open file.
	virtual void* FileOpen(const char* name, const char* mode);
	// Close file.
	virtual bool FileClose(void* handle);
	// Move file access pointer.
	virtual bool FileSeek(void* handle, long offset, int origin);
	// Get current position of file access pointer.
	virtual size_t FileTell(void* handle);
	// Read file.
	virtual size_t FileRead(void* handle, void* pdata, size_t len);
	// Write file.
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len);
	// Formatted write file.
	virtual size_t __cdecl FilePrint(void* handle, const char* info, ...);
	// Formatted write file.
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vlist);
	// Flush file data.
	virtual bool FileFlush(void* handle);
};

#endif // __FRAME_FILE_ACCESS_H
