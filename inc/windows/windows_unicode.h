// windows_unicode.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_UNICODE_H
#define __WINDOWS_UNICODE_H

#include <windows.h>
#include <string.h>

// Get convert to wide string length(include end symbol).
inline size_t plat_local_to_unicode_size(const char* s)
{
	return MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
}

// String convert to wide string.
inline const wchar_t* plat_local_to_unicode(wchar_t* dst, size_t byte_size,
	const char* src)
{
	const size_t len = byte_size / sizeof(wchar_t);
	int res = MultiByteToWideChar(CP_ACP, 0, src, -1, dst, (int)len);

	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			dst[len - 1] = 0;
		}
		else
		{
			dst[0] = 0;
		}
	}

	return dst;
}

// Get convert to string length(include end symbol).
inline size_t plat_unicode_to_local_size(const wchar_t* ws)
{
	return WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL);
}

// Wide string convert to string.
inline const char* plat_unicode_to_local(char* dst, size_t byte_size,
	const wchar_t* src)
{
	int res = WideCharToMultiByte(CP_ACP, 0, src, -1, dst, (int)byte_size, 
		NULL, NULL);
	
	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			dst[byte_size - 1] = 0;
		}
		else
		{
			dst[0] = 0;
		}
	}

	return dst;
}

// Get utf8 string convert to wide string length(include end symbol).
inline size_t plat_utf8_to_unicode_size(const char* s)
{
	return MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
}

// Utf8 string convert to wide string.
inline const wchar_t* plat_utf8_to_unicode(wchar_t* dst, size_t byte_size,
	const char* src)
{
	const size_t len = byte_size / sizeof(wchar_t);
	int res = MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, (int)len);

	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			dst[len - 1] = 0;
		}
		else
		{
			dst[0] = 0;
		}
	}

	return dst;
}

// Get wide string convert to utf8 string length(include end symbol).
inline size_t plat_unicode_to_utf8_size(const wchar_t* ws)
{
	return WideCharToMultiByte(CP_UTF8, 0, ws, -1, NULL, 0, NULL, NULL);
}

// Wide string convert to utf8 string.
inline const char* plat_unicode_to_utf8(char* dst, size_t byte_size,
	const wchar_t* src)
{
	int res = WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, (int)byte_size, 
		NULL, NULL);

	if (0 == res)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			dst[byte_size - 1] = 0;
		}
		else
		{
			dst[0] = 0;
		}
	}

	return dst;
}

// Get utf16 string length.
inline size_t plat_get_utf16_length(const unsigned short* src)
{
	return wcslen((const wchar_t*)src);
}

// Utf16 string convert to wide string.
inline const wchar_t* plat_utf16_to_unicode(wchar_t* dst, size_t byte_size,
	const unsigned short* src)
{
	// Wide string is two bytes in windows system, not need convert.
	size_t buf_len = byte_size / sizeof(wchar_t);
	size_t info_len = wcslen((const wchar_t*)src);

	if (info_len >= buf_len)
	{
		memcpy(dst, src, (buf_len - 1) * sizeof(wchar_t));
		dst[buf_len - 1] = 0;
	}
	else
	{
		memcpy(dst, src, (info_len + 1) * sizeof(wchar_t));
	}

	return dst;
}

// Wide string convert to utf16 string.
inline const unsigned short* plat_unicode_to_utf16(unsigned short* dst,
	size_t byte_size, const wchar_t* src)
{
	// Wide string is two bytes in windows system, not need convert.
	size_t buf_len = byte_size / sizeof(unsigned short);
	size_t info_len = wcslen(src);

	if (info_len >= buf_len)
	{
		memcpy(dst, src, (buf_len - 1) * sizeof(unsigned short));
		dst[buf_len - 1] = 0;
	}
	else
	{
		memcpy(dst, src, (info_len + 1) * sizeof(unsigned short));
	}

	return dst;
}

// Copy string to clipboard.
inline bool plat_copy_text_to_clipboard(const char* s)
{
	if (OpenClipboard(NULL) == FALSE)
	{
		return false;
	}

	int ws_size = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
	HANDLE hMem = GlobalAlloc(GHND, sizeof(wchar_t)* ws_size);

	if (NULL == hMem)
	{
		return false;
	}

	wchar_t* pText = (wchar_t*)GlobalLock(hMem);

	MultiByteToWideChar(CP_UTF8, 0, s, -1, pText, ws_size);
	GlobalUnlock(hMem);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	GlobalFree(hMem);
	return true;
}

// Get string size in clipboard(include end symbol).
inline size_t plat_get_clipboard_text_size()
{
	if (OpenClipboard(NULL) == FALSE)
	{
		return 0;
	}

	HANDLE hMem = GetClipboardData(CF_UNICODETEXT);

	if (NULL == hMem)
	{
		CloseClipboard();
		return 0;
	}

	size_t mem_size = GlobalSize(hMem);

	if (0 == mem_size)
	{
		CloseClipboard();
		return 0;
	}

	wchar_t* pText = (wchar_t*)GlobalLock(hMem);

	if (NULL == pText)
	{
		CloseClipboard();
		return 0;
	}

	size_t utf8_size = WideCharToMultiByte(CP_UTF8, 0, pText,
		(int)(mem_size / sizeof(wchar_t)), NULL, 0, NULL, NULL);
	
	GlobalUnlock(hMem);
	CloseClipboard();
	return utf8_size;
}

// Paste string from clipboard.
inline size_t plat_paste_text_from_clipboard(char* buffer, size_t buffer_size)
{
	if (OpenClipboard(NULL) == FALSE)
	{
		return 0;
	}

	HANDLE hMem = GetClipboardData(CF_UNICODETEXT);

	if (NULL == hMem)
	{
		CloseClipboard();
		return 0;
	}

	size_t mem_size = GlobalSize(hMem);

	if (0 == mem_size)
	{
		CloseClipboard();
		return 0;
	}

	wchar_t* pText = (wchar_t*)GlobalLock(hMem);

	if (NULL == pText)
	{
		CloseClipboard();
		return 0;
	}

	int ws_size = (int)(mem_size / sizeof(wchar_t));
	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, pText, ws_size, buffer,
		(int)buffer_size, NULL, NULL);

	GlobalUnlock(hMem);
	CloseClipboard();
	return utf8_size;
}

#endif // __WINDOWS_UNICODE_H
