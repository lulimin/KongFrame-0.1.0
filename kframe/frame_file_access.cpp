// frame_file_access.cpp
// Created by lulimin on 2019/2/17.

#include "frame_file_access.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "../inc/utility.h"
#include "../inc/platform.h"
#include "../inc/auto_buffer.h"
#include <string.h>
#include <stdio.h>

// FrameFileAccess

FrameFileAccess* FrameFileAccess::CreateInstance()
{
	return K_INNER_NEW(FrameFileAccess);
}

FrameFileAccess::FrameFileAccess()
{
}

FrameFileAccess::~FrameFileAccess()
{
}

void FrameFileAccess::Release()
{
	K_INNER_DELETE(this);
}

bool FrameFileAccess::DirectoryExists(const char* name)
{
	return plat_find_dir(name);
}

bool FrameFileAccess::DirectoryCreate(const char* name)
{
	if (plat_find_dir(name))
	{
		return false;
	}

	if (plat_create_dir(name))
	{
		return true;
	}

	bool succeed = true;
	size_t size = strlen(name) + 1;
	TAutoBuffer<char, 256, FrameInnerAlloc> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();

	util_string_copy(buffer, size, name);

	for (size_t i = 0; i < size; ++i)
	{
		if ((buffer[i] == '\\') || (buffer[i] == '/'))
		{
			buffer[i] = 0;

			if (!plat_find_dir(buffer))
			{
				if (!plat_create_dir(buffer))
				{
					succeed = false;
				}
			}

			buffer[i] = PLAT_PATH_SEPARATOR;
		}
	}

	return succeed;
}

bool FrameFileAccess::DirectoryDelete(const char* name)
{
	return plat_delete_dir(name);
}

bool FrameFileAccess::FileExists(const char* name)
{
	return plat_file_exists(name);
}

bool FrameFileAccess::FileDelete(const char* name)
{
	return plat_file_delete(name);
}

uint64_t FrameFileAccess::FileSize(const char* name)
{
	plat_file_size_t pfs;
	
	if (!plat_get_file_size(name, &pfs))
	{
		return 0;
	}

	return ((uint64_t)pfs.nSizeHigh << 32) + pfs.nSizeLow;
}

bool FrameFileAccess::FileTime(const char* name, file_time_t* file_time)
{
	plat_file_time_t pft;

	if (!plat_get_file_time(name, &pft))
	{
		return false;
	}
	
	file_time->nYear = pft.nYear;
	file_time->nMonth = pft.nMonth;
	file_time->nDay = pft.nDay;
	file_time->nHour = pft.nHour;
	file_time->nMinute = pft.nMinute;
	file_time->nSecond = pft.nSecond;
	return true;
}

void* FrameFileAccess::FileOpen(const char* name, const char* mode)
{
	return plat_file_open(name, mode);
}

bool FrameFileAccess::FileClose(void* handle)
{
	return fclose((FILE*)handle) == 0;
}

bool FrameFileAccess::FileSeek(void* handle, long offset, int origin)
{
	return fseek((FILE*)handle, offset, origin) == 0;
}

size_t FrameFileAccess::FileTell(void* handle)
{
	return ftell((FILE*)handle);
}

size_t FrameFileAccess::FileRead(void* handle, void* pdata, size_t len)
{
	return fread(pdata, sizeof(char), len, (FILE*)handle);
}

size_t FrameFileAccess::FileWrite(void* handle, const void* pdata, size_t len)
{
	return fwrite(pdata, sizeof(char), len, (FILE*)handle);
}

size_t __cdecl FrameFileAccess::FilePrint(void* handle, const char* info, ...)
{
	va_list vlist;

	va_start(vlist, info);

	size_t res = vfprintf((FILE*)handle, info, vlist);

	va_end(vlist);
	return res;
}

size_t FrameFileAccess::FilePrintList(void* handle, const char* info,
	va_list vlist)
{
	return vfprintf((FILE*)handle, info, vlist);
}

bool FrameFileAccess::FileFlush(void* handle)
{
	return fflush((FILE*)handle) == 0;
}
