// android_file_access.h
// Created by lulimin on 2021/11/9.

#ifndef __ANDROID_FILE_ACCESS_H
#define __ANDROID_FILE_ACCESS_H

#include "../../inc/i_file_access.h"

// AndroidFileAccess

struct AAssetManager;

class AndroidFileAccess : public IFileAccess
{
public:
	static AndroidFileAccess* CreateInstance(AAssetManager* pManager,
        const char* internal_data_path);

public:
	AndroidFileAccess(AAssetManager* pManager, const char* internal_data_path);
	virtual ~AndroidFileAccess();

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
	// Get file size.
	virtual uint64_t FileSize(const char* name);
	// Get file time.
	virtual bool FileTime(const char* name, file_time_t* file_time);
	// Open file.
	virtual void* FileOpen(const char* name, const char* mode);
	// Close file.
	virtual bool FileClose(void* handle);
	// Move file access pointer.
	virtual bool FileSeek(void* handle, long offset, int origin);
	// Get file access pointer position.
	virtual size_t FileTell(void* handle);
	// Read data from file.
	virtual size_t FileRead(void* handle, void* pdata, size_t len);
	// Write data to file.
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len);
	// Formatted write file.
	virtual size_t __cdecl FilePrint(void* handle, const char* info, ...);
	// Formatted write file.
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vl);
	// Flush file data.
	virtual bool FileFlush(void* handle);

private:
	AAssetManager* m_pAssetManager;
	char m_szOutputPath[256];
};

#endif // __ANDROID_FILE_ACCESS_H
