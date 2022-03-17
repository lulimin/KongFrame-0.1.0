// android_file_access.cpp
// Created by lulimin on 2021/11/9.

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "android_file_access.h"
#include "../../inc/utility.h"
#include "../../inc/platform.h"
#include "../../inc/auto_buffer.h"

// AndroidFileAccess

struct file_handle_t
{
    union
    {
        FILE* pFile;
        AAsset* pAsset;
    };

    bool bOutput;
};

// Convert to validate asset name.
static const char* get_asset_path(const char* src, char* dst, size_t size)
{
	util_string_copy(dst, size, src);

	for (char* s = dst; *s; ++s)
	{
		if (*s == '\\')
		{
			*s = '/';
		}	
	}

	if (dst[0] == '/')
    {
	    // Ignore first character '/'.
	    return dst + 1;
    }

	return dst;
}

AndroidFileAccess* AndroidFileAccess::CreateInstance(AAssetManager* pManager,
    const char* internal_data_path)
{
	return NEW AndroidFileAccess(pManager, internal_data_path);
}

AndroidFileAccess::AndroidFileAccess(AAssetManager* pManager,
    const char* internal_data_path)
{
	Assert(pManager != NULL);
	Assert(internal_data_path != NULL);

	m_pAssetManager = pManager;

	if (!util_string_empty(internal_data_path))
	{
        plat_safe_sprintf(m_szOutputPath, sizeof(m_szOutputPath),
            "%s/", internal_data_path);
    }
	else
    {
	    m_szOutputPath[0] = 0;
    }
}

AndroidFileAccess::~AndroidFileAccess()
{
}

void AndroidFileAccess::Release()
{
	delete this;
}

bool AndroidFileAccess::DirectoryExists(const char* name)
{
	char buf[512];
	const char* asset_name = get_asset_path(name, buf, sizeof(buf));
	// Always return not null.
	AAssetDir* pAssetDir = AAssetManager_openDir(m_pAssetManager, asset_name);
	// Check exists any file.
	bool has_file = AAssetDir_getNextFileName(pAssetDir) != NULL;

	AAssetDir_close(pAssetDir);

	if (has_file)
    {
	    return true;
    }

    return plat_find_dir(name);
	//if (!have_file)
	//{
		//if (util_string_empty(m_szOutputPath))
        //{
		//    return false;
        //}

        //char dir_name[512];

        //plat_safe_sprintf(dir_name, sizeof(dir_name), "%s%s", m_szOutputPath, fname);
        //return plat_find_dir(dir_name);
        //return plat_find_dir(name);
	//}

	//AAssetDir_close(pAssetDir);
	//return true;
}

bool AndroidFileAccess::DirectoryCreate(const char* name)
{
    //if (util_string_empty(m_szOutputPath))
    //{
    //    return false;
    //}

    //char buf[512];
    //const char* fname = get_normal_path(name, buf, sizeof(buf));
    //char dir_name[512];

    //plat_safe_sprintf(dir_name, sizeof(dir_name), "%s%s", m_szOutputPath, fname);

    //if (plat_find_dir(dir_name))
    if (plat_find_dir(name))
    {
        return false;
    }

    //if (plat_create_dir(dir_name))
    if (plat_create_dir(name))
    {
        return true;
    }

    bool succeed = true;
    //size_t size = strlen(dir_name) + 1;
    size_t size = strlen(name) + 1;
    TAutoBuffer<char, 256> auto_buf(size);
    char* buffer = auto_buf.GetBuffer();

    //util_string_copy(buffer, size, dir_name);
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

bool AndroidFileAccess::DirectoryDelete(const char* name)
{
    //if (util_string_empty(m_szOutputPath))
    //{
    //    return false;
    //}

    //char buf[512];
    //const char* fname = get_normal_path(name, buf, sizeof(buf));
    //char dir_name[512];

    //plat_safe_sprintf(dir_name, sizeof(dir_name), "%s%s", m_szOutputPath, fname);
    //return plat_delete_dir(dir_name);
    return plat_delete_dir(name);
}

bool AndroidFileAccess::FileExists(const char* name)
{
	char buf[512];
	const char* asset_name = get_asset_path(name, buf, sizeof(buf));
	AAsset* pAsset = AAssetManager_open(m_pAssetManager, asset_name,
		AASSET_MODE_UNKNOWN);

	if (NULL == pAsset)
	{
		//if (util_string_empty(m_szOutputPath))
        //{
		//    return false;
        //}

        //char dir_name[512];

        //plat_safe_sprintf(dir_name, sizeof(dir_name), "%s%s", m_szOutputPath, fname);

        struct stat st;

        //if (stat(dir_name, &st) == -1)
        if (stat(name, &st) == -1)
        {
            return false;
        }

        return (st.st_mode & S_IFMT) == S_IFREG;
	}

	AAsset_close(pAsset);
	return true;
}

bool AndroidFileAccess::FileDelete(const char* name)
{
    char buf[512];
    const char* asset_name = get_asset_path(name, buf, sizeof(buf));
    AAsset* pAsset = AAssetManager_open(m_pAssetManager, asset_name,
        AASSET_MODE_UNKNOWN);

    if (pAsset)
    {
        // Can't delete asset file.
        AAsset_close(pAsset);
        return false;
    }

    //if (util_string_empty(m_szOutputPath))
    //{
    //    return false;
    //}

    //char buf[512];
    //const char* fname = get_normal_path(name, buf, sizeof(buf));
    //char full_name[512];

    //plat_safe_sprintf(full_name, sizeof(full_name), "%s%s", m_szOutputPath, fname);
	//return unlink(full_name) == 0;
    return unlink(name) == 0;
}

uint64_t AndroidFileAccess::FileSize(const char* name)
{
	char buf[512];
	const char* asset_name = get_asset_path(name, buf, sizeof(buf));
	AAsset* pAsset = AAssetManager_open(m_pAssetManager, asset_name,
		AASSET_MODE_UNKNOWN);

	if (NULL == pAsset)
	{
		//if (util_string_empty(m_szOutputPath))
        //{
		//    return 0;
        //}

        //char full_name[512];

        //plat_safe_sprintf(full_name, sizeof(full_name), "%s%s", m_szOutputPath, fname);

        struct stat st;

        //if (stat(full_name, &st) == -1)
        if (stat(name, &st) == -1)
        {
            return false;
        }

        return st.st_size;
	}

	size_t asset_size = AAsset_getLength(pAsset);

	AAsset_close(pAsset);
	return asset_size;
}

bool AndroidFileAccess::FileTime(const char* name, file_time_t* file_time)
{
    char buf[512];
    const char* asset_name = get_asset_path(name, buf, sizeof(buf));
    AAsset* pAsset = AAssetManager_open(m_pAssetManager, asset_name,
        AASSET_MODE_UNKNOWN);

    if (NULL == pAsset)
    {
        //if (util_string_empty(m_szOutputPath))
        //{
        //    return false;
        //}

        //char full_name[512];

        //plat_safe_sprintf(full_name, sizeof(full_name), "%s%s", m_szOutputPath, fname);

        struct stat st;

        //if (stat(full_name, &st) == -1)
        if (stat(name, &st) == -1)
        {
            return false;
        }

        // Use UTC time.
        tm* pt = gmtime(&st.st_mtime);

        file_time->nYear = pt->tm_year + 1900;
        file_time->nMonth = pt->tm_mon + 1;
        file_time->nDay = pt->tm_mday;
        file_time->nHour = pt->tm_hour;
        file_time->nMinute = pt->tm_min;
        file_time->nSecond = pt->tm_sec;
        return true;
    }

    // Asset no file time.
    memset(file_time, 0, sizeof(file_time_t));
    AAsset_close(pAsset);
    return true;
}

void* AndroidFileAccess::FileOpen(const char* name, const char* mode)
{
	char buf[512];
	const char* asset_name = get_asset_path(name, buf, sizeof(buf));

	if ((mode[0] == 'r') || (mode[0] == 'R'))
    {
        AAsset* pAsset = AAssetManager_open(m_pAssetManager, asset_name,
            AASSET_MODE_UNKNOWN);

        if (pAsset)
        {
            file_handle_t* fh = (file_handle_t*)malloc(sizeof(file_handle_t));

            fh->bOutput = false;
            fh->pAsset = pAsset;
            return fh;
        }
    }

	//if (util_string_empty(m_szOutputPath))
    //{
	//    return NULL;
    //}

    //char full_name[512];

    //plat_safe_sprintf(full_name, sizeof(full_name), "%s%s", m_szOutputPath, fname);

    //FILE* fp = fopen(full_name, mode);
    FILE* fp = fopen(name, mode);

    if (NULL == fp)
    {
        return NULL;
    }

    file_handle_t* fh = (file_handle_t*)malloc(sizeof(file_handle_t));

    fh->bOutput = true;
    fh->pFile = fp;
    return fh;
}

bool AndroidFileAccess::FileClose(void* handle)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (fh->bOutput)
    {
        FILE* pFile = fh->pFile;

        free(handle);
        return fclose(pFile) == 0;
    }

    AAsset* pAsset = fh->pAsset;

    free(handle);
	AAsset_close(pAsset);
	return true;
}

bool AndroidFileAccess::FileSeek(void* handle, long offset, int origin)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (fh->bOutput)
    {
        return fseek(fh->pFile, offset, origin) == 0;
    }

    return AAsset_seek(fh->pAsset, offset, origin) != (off_t)(-1);
}

size_t AndroidFileAccess::FileTell(void* handle)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (fh->bOutput)
    {
        return ftell(fh->pFile);
    }

	return AAsset_getLength(fh->pAsset) - AAsset_getRemainingLength(fh->pAsset);
}

size_t AndroidFileAccess::FileRead(void* handle, void* pdata, size_t len)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (fh->bOutput)
    {
        return fread(pdata, 1, len, fh->pFile);
    }

	return AAsset_read(fh->pAsset, pdata, len);
}

size_t AndroidFileAccess::FileWrite(void* handle, const void* pdata, size_t len)
{
	file_handle_t* fh = (file_handle_t*)handle;

	if (!fh->bOutput)
    {
	    return 0;
    }

	return fwrite(pdata, 1, len, fh->pFile);
}

size_t __cdecl AndroidFileAccess::FilePrint(void* handle, const char* info, ...)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (!fh->bOutput)
    {
        return 0;
    }

    va_list vlist;

    va_start(vlist, info);

    size_t res = vfprintf(fh->pFile, info, vlist);

    va_end(vlist);
    return res;
}

size_t AndroidFileAccess::FilePrintList(void* handle, const char* info,
	va_list vl)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (!fh->bOutput)
    {
        return 0;
    }

    return vfprintf(fh->pFile, info, vl);
}

bool AndroidFileAccess::FileFlush(void* handle)
{
    file_handle_t* fh = (file_handle_t*)handle;

    if (!fh->bOutput)
    {
        return false;
    }

    return fflush(fh->pFile) == 0;
}
