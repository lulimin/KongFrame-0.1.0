// frame_file.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_FILE_H
#define __FRAME_FILE_H

#include "i_file_access.h"
#include "frame_api.h"
#include <stdio.h>

// File access.

// Open file.
inline void* frame_fopen(const char* file_name, const char* mode)
{
	return frame_get_file_access()->FileOpen(file_name, mode);
}

// Close file.
inline int frame_fclose(void* stream)
{
	if (frame_get_file_access()->FileClose(stream))
	{
		return 0;
	}
	else
	{
		return -1; // EOF.
	}
}

// Move file access position.
inline int frame_fseek(void* stream, long offset, int origin)
{
	if (frame_get_file_access()->FileSeek(stream, offset, origin))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

// Get file access position.
inline long frame_ftell(void* stream)
{
	return (long)frame_get_file_access()->FileTell(stream);
}

// Read file.
inline size_t frame_fread(void* buffer, size_t size, size_t count, 
	void* stream)
{
	size_t read_size = frame_get_file_access()->FileRead(stream, buffer, 
		size * count);
	
	return read_size / size;
}

// Write file.
inline size_t frame_fwrite(const void* buffer, size_t size, size_t count, 
	void* stream)
{
	size_t write_size = frame_get_file_access()->FileWrite(stream, buffer, 
		size * count);
	
	return write_size / size;
}

// Formatted write file.
inline int __cdecl frame_fprintf(void* stream, const char* format, ...)
{
	va_list vl;
	
	va_start(vl, format);
	
	size_t res = frame_get_file_access()->FilePrintList(stream, format, vl);
	
	va_end(vl);
	return (int)res;
}

// Commit file.
inline int frame_fflush(void* stream)
{
	if (frame_get_file_access()->FileFlush(stream))
	{
		return 0;
	}
	else
	{
		return -1; // EOF.
	}
}

#endif // __FRAME_FILE_H
