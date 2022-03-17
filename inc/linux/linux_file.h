// linux_file.h
// Created by lulimin on 2019/2/14.

#ifndef __LINUX_FILE_H
#define __LINUX_FILE_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <time.h>
#include <ctype.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>

// File path separate symbol.
#define PLAT_PATH_SEPARATOR '/'

// File size.
struct plat_file_size_t
{
	unsigned int nSizeHigh;
	unsigned int nSizeLow;
};

// File time.
struct plat_file_time_t 
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
};

// File search data.
struct plat_file_search_t
{
	DIR* pDir;
	dirent* pEnt;
	char szPath[256];
	char szExt[16];
};

// Get system standard file path.
inline const char* plat_get_normal_path(char* buffer, size_t size, 
	const char* path)
{
    // Mark previous symbol is a separator.
    bool slash = false;
    const char* src = path;
    char* dst = buffer;
    char* dst_end = buffer + size - 1;
    
    while (*src) 
	{
		if (dst >= dst_end)
		{
			break;
		}
    	
        if ((*src == '/') || (*src == '\\'))
        {
        	if (slash)
			{
	            // Eliminate repeat slash.
	            ++src;
	            continue;
        	}
			else
			{
				slash = true;
			}

        	*dst = '/';
    	} 
		else
		{
   			// Convert to lower case.
   			//*dst = tolower(*src);
			*dst = *src;
			slash = false;
    	}
    	
   		++dst;
   		++src;
	}
	
	*dst = 0;
	return buffer;
}

// Get full file path.
inline size_t plat_get_full_path(const char* file_name, char* buffer,
	size_t size, char** file_part)
{
	memset(buffer, 0, size);
	
	// Eliminate previous space.
	const char* first = file_name;
	
	while ((*first == ' ') || (*first == '\t'))
	{
		++first;
	}

	// Test already include full path.
	if (*first == '/')
	{
		size_t file_name_len = strlen(first);
	
		if (file_name_len >= size)
		{
			return 0; // Buffer overflow.
		}
		
		memcpy(buffer, first, file_name_len + 1);
	}
	else
	{
		// Get current directory.
		char cur_dir[512] = { 0 };
		
		if (getcwd(cur_dir, sizeof(cur_dir) - 1) == NULL)
		{
			return 0;
		}

		size_t dir_len = strlen(cur_dir);

		if ((dir_len > 0) && (cur_dir[dir_len - 1] == '/'))
		{
			cur_dir[dir_len - 1] = 0;
		}

		// Parse relative file path.
		const char* p = first;
		
		while (*p)
		{
			if (*p == '.')
			{
				// (./)Current directory.
				if ((p[1] == '\\') || (p[1] == '/'))
				{
					p += 2;
					continue;
				}
				
				if (p[1] != '.')
				{
					break;
				}
				
				if ((p[2] != '\\') && (p[2] != '/'))
				{
					break;
				}
				
				// (../)Get parent directory.
				char* prev = strrchr(cur_dir, '/');
				
				if (prev)
				{
					*prev = 0;
					//*(prev + 1) = 0;
				}
				
				p += 3;
			} 
			else
			{
				break;
			}
		}
		
		size_t cur_dir_len = strlen(cur_dir);
		
		if (cur_dir[cur_dir_len - 1] != '/')
		{
			cur_dir[cur_dir_len] = '/';
			cur_dir[cur_dir_len + 1] = 0;
		}
	
		if (strlen(cur_dir) + strlen(p) >= size)
		{
			return 0;
		}
	
		strcpy(buffer, cur_dir);
		strcat(buffer, p);
	}

	if (file_part)
	{
		// Split file path and name.
		char* separator = strrchr(buffer, '/');
		
		if (separator)
		{
			*file_part = separator + 1;
		}
		else
		{
			*file_part = buffer; // No file directory.
		}
	}
	
	return strlen(buffer);
}

// Test file exists.
inline bool plat_file_exists(const char* file_name)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFREG;
}

// Open file.
inline FILE* plat_file_open(const char* file_name, const char* mode)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	return fopen(sname, mode);
}

// Delete file.
inline bool plat_file_delete(const char* file_name)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	return unlink(sname) == 0;
}

// Get file length.
inline bool plat_get_file_size(const char* file_name, 
	plat_file_size_t* file_size)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	file_size->nSizeHigh = st.st_size >> 32;
	file_size->nSizeLow = st.st_size & 0xFFFFFFFF;
	return true;
}

// Get file last modify time.
inline bool plat_get_file_time(const char* file_name, 
	plat_file_time_t* file_time)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	
	struct stat st;
	
	if (stat(sname, &st) == -1)
	{
		return false;
	}
	
	// Set to UTC time.
	tm* pt = gmtime(&st.st_mtime);
	
	file_time->nYear = pt->tm_year + 1900;
	file_time->nMonth = pt->tm_mon + 1;
	file_time->nDay = pt->tm_mday;
	file_time->nHour = pt->tm_hour;
	file_time->nMinute = pt->tm_min;
	file_time->nSecond = pt->tm_sec;
	return true;
}

// Set file time.
inline bool plat_set_file_time(const char* file_name, 
	const plat_file_time_t* file_time)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	
	struct utimbuf ut;
	struct tm tm1;
	
	memset(&tm1, 0, sizeof(tm1));
	
	tm1.tm_year = file_time->nYear;
	tm1.tm_mon = file_time->nMonth;
	tm1.tm_mday = file_time->nDay;
	tm1.tm_hour = file_time->nHour;
	tm1.tm_min = file_time->nMinute;
	tm1.tm_sec = file_time->nSecond;
	
	ut.actime = mktime(&tm1);
	ut.modtime = ut.actime;
	return utime(sname, &ut) == 0;
}

// Change file attribute, guarantee file can modify.
inline bool plat_enable_file_modify(const char* file_name)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), file_name);
	return chmod(sname, 0777) == 0;
}

// Get current work directory.
inline bool plat_get_current_dir(char* buffer, size_t size)
{
	char* res = getcwd(buffer, size);

	if (NULL == res)
	{
		buffer[0] = 0;
		return false;
	}

	return true;
}

// Set current work directory.
inline bool plat_set_current_dir(const char* dir_name)
{
	return chdir(dir_name) == 0;
}

// Test directory exists.
inline bool plat_find_dir(const char* dir_name)
{
	char sname[512];

	plat_get_normal_path(sname, sizeof(sname), dir_name);

	struct stat st;

	if (stat(sname, &st) == -1)
	{
		return false;
	}

	return (st.st_mode & S_IFMT) == S_IFDIR;
}

// Create directory.
inline bool plat_create_dir(const char* dir_name)
{
	char sname[512];

	plat_get_normal_path(sname, sizeof(sname), dir_name);
	return mkdir(sname, 0700) == 0;
}

// Delete directory.
inline bool plat_delete_dir(const char* dir_name)
{
	char sname[512];

	plat_get_normal_path(sname, sizeof(sname), dir_name);
	return rmdir(sname) == 0;
}

// Start search file.
inline bool plat_find_first(plat_file_search_t* file_search, const char* path,
	const char* name)
{
	char sname[512];
	
	plat_get_normal_path(sname, sizeof(sname), path);
	
	size_t path_len = strlen(sname);
	
	if (path_len >= sizeof(file_search->szPath))
	{
		return false;
	}
	
	// File extension, support format (*.*) or (*.xxx).
	const char* ext = name;
	const char* name_dot = strchr(name, '.');
	
	if (name_dot)
	{
		if (*(name_dot + 1) == '*')
		{
			// Set to(*.*).
			ext = "";
		}
		else
		{
			// (*.xxx).
			ext = name_dot + 1;
		}
	}
	
	size_t ext_len = strlen(ext);
	
	if (ext_len >= sizeof(file_search->szExt))
	{
		return false;
	}
	
	DIR* pDir = opendir(sname);
	
	if (NULL == pDir)
	{
		return false;
	}
	
	dirent* pEnt = readdir(pDir);
	
	if (ext[0] != 0)
	{
		while (pEnt)
		{
			const char* dot = strchr(pEnt->d_name, '.');
			
			if (dot)
			{
				if (strcasecmp(ext, dot + 1) == 0)
				{
					break; // Match file extension.
				}
			}
			
			// Read next.
			pEnt = readdir(pDir);
		}
	}
	
	if (NULL == pEnt)
	{
		closedir(pDir);
		return false;
	}
	
	file_search->pDir = pDir;
	file_search->pEnt = pEnt;
	memcpy(file_search->szPath, path, path_len + 1);
	memcpy(file_search->szExt, ext, ext_len + 1);
	return true;
}

// Search next file.
inline bool plat_find_next(plat_file_search_t* file_search)
{
	dirent* pEnt = readdir(file_search->pDir);
	const char* ext = file_search->szExt;

	if (ext[0] != 0)
	{
		while (pEnt)
		{
			const char* dot = strchr(pEnt->d_name, '.');

			if (dot)
			{
				if (strcasecmp(ext, dot + 1) == 0)
				{
					break; // Match file extension.
				}
			}

			// Read next.
			pEnt = readdir(file_search->pDir);
		}
	}

	if (NULL == pEnt)
	{
		return false;
	}

	file_search->pEnt = pEnt;
	return true;
}

// End search file.
inline bool plat_find_close(plat_file_search_t* file_search)
{
	return closedir(file_search->pDir) == 0;
}

// Get result object is file.
inline bool plat_get_find_is_file(plat_file_search_t* file_search)
{
	char fname[512];
	
	snprintf(fname, sizeof(fname) - 1, "%s/%s", file_search->szPath,
		file_search->pEnt->d_name);
	fname[sizeof(fname) - 1] = 0;
	
	struct stat st;
	
	if (stat(fname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFREG;
}

// Get result object is directory.
inline bool plat_get_find_is_dir(plat_file_search_t* file_search)
{
	char fname[512];
	
	snprintf(fname, sizeof(fname) - 1, "%s/%s", file_search->szPath,
		file_search->pEnt->d_name);
	fname[sizeof(fname) - 1] = 0;
	
	struct stat st;
	
	if (stat(fname, &st) == -1)
	{
		return false;
	}
	
	return (st.st_mode & S_IFMT) == S_IFDIR;
}

// Get search result name.
inline const char* plat_get_find_name(plat_file_search_t* file_search)
{
	return file_search->pEnt->d_name;
}

#endif // __LINUX_FILE_H
