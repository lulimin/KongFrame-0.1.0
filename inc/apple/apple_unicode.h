// apple_unicode.h
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_UNICODE_H
#define __APPLE_UNICODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>
#include <locale.h>

// Get convert to wide string length(include end symbol).
inline size_t plat_local_to_unicode_size(const char* s)
{
	setlocale(LC_ALL, "");

	size_t res = mbstowcs(NULL, s, 0);

	if (res == (size_t)(-1))
	{
		return 1; // Cannot convert.
	}

	return res + 1;
}

// String convert to wide string.
inline const wchar_t* plat_local_to_unicode(wchar_t* dst, size_t byte_size,
	const char* src)
{
	setlocale(LC_ALL, "");

	const size_t len = byte_size / sizeof(wchar_t);
	size_t res = mbstowcs(dst, src, len);

	if (res == (size_t)(-1))
	{
		dst[0] = 0; // Cannot convert.
	}
	else if (res == len)
	{
		dst[len - 1] = 0;
	}

	return dst;
}

// Get convert to string length(include end symbol).
inline size_t plat_unicode_to_local_size(const wchar_t* ws)
{
	setlocale(LC_ALL, "");

	size_t res = wcstombs(NULL, ws, 0);

	if (res == (size_t)(-1))
	{
		return 1; // Cannot convert.
	}

	return res + 1;
}

// Wide string convert to string.
inline const char* plat_unicode_to_local(char* dst, size_t byte_size,
	const wchar_t* src)
{
	setlocale(LC_ALL, "");

	size_t res = wcstombs(dst, src, byte_size);

	if (res == (size_t)(-1))
	{
		dst[0] = 0; // Cannot convert.
	}
	else if (res == byte_size)
	{
		dst[byte_size - 1] = 0;
	}

	return dst;
}

// Get utf8 string convert to wide string length(include end symbol).
inline size_t plat_utf8_to_unicode_size(const char* s)
{
	size_t src_len = strlen(s);

	if (0 == src_len)
	{
		return 1;
	}

	size_t count = 0;
	size_t pos = 0;

	while (pos < src_len)
	{
		unsigned char c = s[pos];

		if ((c & 0x80) == 0)
		{
			count += 1;
			pos += 1;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			count += 1;
			pos += 2;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			count += 1;
			pos += 3;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			count += 1;
			pos += 4;
		}
		else
		{
			count += 1;
			pos += 5; 
		}
	}

	return count + 1;
}

// Utf8 string convert to wide string.
inline const wchar_t* plat_utf8_to_unicode(wchar_t* dst, size_t byte_size,
	const char* src)
{
	size_t src_len = strlen(src);

	if (0 == src_len) 
	{
		dst[0] = 0;
		return dst;
	}

	size_t max_len = byte_size / sizeof(wchar_t) - 1;	
	size_t count = 0;
	size_t pos = 0;

	while (pos < src_len) 
	{
		if (count >= max_len)
		{
			break;
		}

		wchar_t wchar;
		unsigned char c = src[pos];

		if ((c & 0x80) == 0) 
		{ 
			// 0xxx xxxx
			wchar = src[pos];
			pos += 1;
		}
		else if ((c & 0xE0) == 0xC0)
		{ 
			// 110x xxxx 10xx xxxx
			wchar = (src[pos + 0] & 0x3F) << 6;
			wchar |= (src[pos + 1] & 0x3F);
			pos += 2;
		}
		else if ((c & 0xF0) == 0xE0) 
		{ 
			// 1110 xxxx 10xx xxxx 10xx xxxx
			wchar = (src[pos + 0] & 0x1F) << 12;
			wchar |= (src[pos + 1] & 0x3F) << 6;
			wchar |= (src[pos + 2] & 0x3F);
			pos += 3;
		}
		else if ((c & 0xF8) == 0xF0)
		{ 
			// UCS4: 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
			wchar = (src[pos + 0] & 0x0F) << 18;
			wchar |= (src[pos + 1] & 0x3F) << 12;
			wchar |= (src[pos + 2] & 0x3F) << 6;
			wchar |= (src[pos + 3] & 0x3F);
			pos += 4;
		}
		else
		{ 
			// UCS4: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx 
			wchar = (src[pos + 0] & 0x07) << 24;
			wchar |= (src[pos + 1] & 0x3F) << 18;
			wchar |= (src[pos + 2] & 0x3F) << 12;
			wchar |= (src[pos + 3] & 0x3F) << 6;
			wchar |= (src[pos + 4] & 0x3F);
			pos += 5;
		}

		dst[count++] = wchar;
	}

	dst[count] = 0;
	return dst;
}

// Get wide string convert to utf8 string length(include end symbol).
inline size_t plat_unicode_to_utf8_size(const wchar_t* ws)
{
	size_t src_len = wcslen(ws);

	if (0 == src_len)
	{
		return 1;
	}

	size_t count = 0;

	for (size_t i = 0; i < src_len; ++i)
	{
		wchar_t wch = ws[i];

		if (wch < 0x80) // 0000 - 007F
		{
			count += 1;
		}
		else if (wch < 0x800) // 0080 - 07FF
		{
			count += 2;
		}
		else if (wch < 0x10000)	// 0800 - FFFF
		{
			count += 3;
		}
		else if (wch < 0x110000) //	0001 0000 - 0010 FFFF
		{
			count += 4;
		}
		else
		{
			count += 5;
		}
	}

	return count + 1;
}

// Wide string convert to utf8 string.
inline const char* plat_unicode_to_utf8(char* dst, size_t byte_size,
	const wchar_t* src)
{
	size_t src_len = wcslen(src);

	if (0 == src_len) 
	{
		dst[0] = 0;
		return dst;
	}

	size_t max_len = byte_size - 1;
	size_t count = 0;

	for (size_t i = 0; i < src_len; ++i) 
	{
		wchar_t wch = src[i];

		if (wch < 0x80)	
		{ 
			// 0xxx xxxx
			if ((count + 1) > max_len)
			{
				break;
			}

			dst[count++] = (char)wch;
		} 
		else if (wch < 0x800) 
		{ 
			// 110x xxxx 10xx xxxx
			if ((count + 2) > max_len)
			{
				break;
			}

			dst[count++] = 0xC0 | (wch >> 6);
			dst[count++] = 0x80 | (wch & 0x3F);
		} 
		else if (wch < 0x10000)
		{	
			// 1110 xxxx 10xx xxxx 10xx xxxx
			if ((count + 3) > max_len)
			{
				break;
			}

			dst[count++] = 0xE0 | (wch >> 12);
			dst[count++] = 0x80 | ((wch >> 6) & 0x3F);
			dst[count++] = 0x80 | (wch & 0x3F);
		} 
		else if (wch < 0x110000)
		{ 
			// UCS4: 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
			if ((count + 4) > max_len)
			{
				break;
			}

			dst[count++] = 0xF0 | (wch >> 18);
			dst[count++] = 0x80 | ((wch >> 12) & 0x3F);
			dst[count++] = 0x80 | ((wch >> 6) & 0x3F);
			dst[count++] = 0x80 | (wch & 0x3F);
		} 
		else
		{ 
			// UCS4: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxxx
			if ((count + 5) > max_len)
			{
				break;
			}

			dst[count++] = 0xF8 | (wch >> 24);
			dst[count++] = 0x80 | ((wch >> 18) & 0x3F);
			dst[count++] = 0x80 | ((wch >> 12) & 0x3F);
			dst[count++] = 0x80 | ((wch >> 6) & 0x3F);
			dst[count++] = 0x80 | (wch & 0x3F);
		}
	}

	dst[count] = 0;
	return dst;
}

// Get utf16 string length.
inline size_t plat_get_utf16_length(const unsigned short* src)
{
	const unsigned short* p = src;

	while (*p)
	{
		++p;
	}

	return p - src;
}

// Utf16 string convert to wide string.
inline const wchar_t* plat_utf16_to_unicode(wchar_t* dst, size_t byte_size,
	const unsigned short* src)
{
	size_t max_len = byte_size / sizeof(wchar_t) - 1;
	const unsigned short* p = src;
	size_t count = 0;

	while (*p)
	{
		if (count >= max_len)
		{
			break;
		}

		dst[count++] = (wchar_t)*p;
		++p;
	}

	dst[count] = 0;
	return dst;
}

// Wide string convert to utf16 string.
inline const unsigned short* plat_unicode_to_utf16(unsigned short* dst,
	size_t byte_size, const wchar_t* src)
{
	size_t max_len = byte_size / sizeof(unsigned short) - 1;
	const wchar_t* p = src;
	size_t count = 0;

	while (*p) 
	{
		if (count >= max_len)
		{
			break;
		}

		dst[count++] = (unsigned short)*p;
		++p;
	}

	dst[count] = 0;
	return dst;
}

// Copy string to clipboard(include end symbol).
inline bool plat_copy_text_to_clipboard(const char* s)
{
	return false;
}

// Get string size in clipboard.
inline size_t plat_get_clipboard_text_size()
{
	return 0;
}

// Paste string from clipboard.
inline size_t plat_paste_text_from_clipboard(char* buffer, size_t buffer_size)
{
	return 0;
}

#endif // __APPLE_UNICODE_H
