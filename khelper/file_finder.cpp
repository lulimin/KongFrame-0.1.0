// file_finder.cpp
// Created by lulimin on 2019/2/21.

#include "file_finder.h"
#include "../inc/utility.h"
#include "../inc/platform.h"
#include "../inc/frame_file.h"
#include "../inc/auto_buffer.h"

/// \file file_finder.cpp
/// \brief Search file or directory.

/// object: FileFinder
/// \brief Search file or directory object.
DEF_OBJECT(FileFinder, IObject);

/// readonly: string Path
/// \brief Current file path.
DEF_READ(const char*, FileFinder, Path);
/// readonly: string Name
/// \brief Current specified file name(or match symbol).
DEF_READ(const char*, FileFinder, Name);

/// function: bool DirectoryEmpty(string path, string name)
/// \brief Test directory empty under specified file path.
/// \param path File path.
/// \param name Match name.
DEF_FUNC_2(bool, FileFinder, DirectoryEmpty, const char*, const char*);
/// function: bool SearchDirectory(string path, string name)
/// \brief Search sub-directories under specified file path.
/// \param path File path.
/// \param name Match name.
DEF_FUNC_2(bool, FileFinder, SearchDirectory, const char*, const char*);
/// function: bool SearchFile(string path, string file_name)
/// \brief Search matched files under specified file path.
/// \param path File path.
/// \param file_name Match name.
DEF_FUNC_2(bool, FileFinder, SearchFile, const char*, const char*);

/// function: int GetDirectoryCount()
/// \brief Get number of found sub-directories.
DEF_FUNC_0(int, FileFinder, GetDirectoryCount);
/// function: table GetDirectoryList()
/// \brief Get found child directory list.
DEF_FUNC_A(FileFinder, GetDirectoryList);

/// function: int GetFileCount()
/// \brief Get number of found files.
DEF_FUNC_0(int, FileFinder, GetFileCount);
/// function: table GetFileList()
/// \brief Get found file name list.
DEF_FUNC_A(FileFinder, GetFileList);

/// function: int ClearComments(string src_name, string dst_name)
/// \brief Remove all comments in file.
/// \param src_name Source file name.
/// \param dst_name Destination file name.
DEF_FUNC_2(int, FileFinder, ClearComments, const char*, const char*);

// FileFinder

FileFinder::FileFinder()
{
}

FileFinder::~FileFinder()
{
	this->ClearResultDirectories();
	this->ClearResultFiles();
}

bool FileFinder::Startup(const IArgList& args)
{
	return true;
}

bool FileFinder::Shutdown()
{
	return true;
}

const char* FileFinder::GetPath() const
{
	return m_sPath.CString();
}

const char* FileFinder::GetName() const
{
	return m_sName.CString();
}

void FileFinder::ClearResultDirectories()
{
	for (size_t i = 0; i < m_Directories.Size(); ++i)
	{
		K_FREE(m_Directories[i]);
	}

	m_Directories.Clear();
}

void FileFinder::ClearResultFiles()
{
	for (size_t i = 0; i < m_Files.Size(); ++i)
	{
		K_FREE(m_Files[i]);
	}

	m_Files.Clear();
}

bool FileFinder::DirectoryEmpty(const char* path, const char* name)
{
	Assert(path != NULL);
	Assert(name != NULL);

	plat_file_search_t fs;

	if (!plat_find_first(&fs, path, name))
	{
		return true;
	}

	do
	{
		if (plat_get_find_is_dir(&fs))
		{
			const char* find_name = plat_get_find_name(&fs);

			if ((strcmp(find_name, ".") != 0) &&
				(strcmp(find_name, "..") != 0))
			{
				plat_find_close(&fs);
				return false;
			}
		}
		else
		{
			plat_find_close(&fs);
			return false;
		}
	} while (plat_find_next(&fs));

	plat_find_close(&fs);
	return true;
}

bool FileFinder::SearchDirectory(const char* path, const char* name)
{
	Assert(path != NULL);

	this->ClearResultDirectories();

	plat_file_search_t fs;

	if (!plat_find_first(&fs, path, name))
	{
		return false;
	}

	do
	{
		if (plat_get_find_is_dir(&fs))
		{
			const char* find_name = plat_get_find_name(&fs);

			if ((strcmp(find_name, ".") != 0) &&
				(strcmp(find_name, "..") != 0))
			{
				size_t name_size = strlen(find_name) + 1;
				char* dir_name = (char*)K_ALLOC(name_size);

				memcpy(dir_name, find_name, name_size);
				m_Directories.PushBack(dir_name);
			}
		}
	} while (plat_find_next(&fs));

	plat_find_close(&fs);
	m_sPath = path;
	return true;
}

bool FileFinder::SearchFile(const char* path, const char* name)
{
	Assert(path != NULL);
	Assert(name != NULL);

	this->ClearResultFiles();

	plat_file_search_t fs;

	if (!plat_find_first(&fs, path, name))
	{
		return false;
	}

	do
	{
		if (plat_get_find_is_file(&fs))
		{
			const char* find_name = plat_get_find_name(&fs);
			size_t name_size = strlen(find_name) + 1;
			char* file_name = (char*)K_ALLOC(name_size);

			memcpy(file_name, find_name, name_size);
			m_Files.PushBack(file_name);
		}
	} while (plat_find_next(&fs));

	plat_find_close(&fs);
	m_sPath = path;
	m_sName = name;
	return true;
}

int FileFinder::GetDirectoryCount()
{
	return (int)m_Directories.Size();
}

void FileFinder::GetDirectoryList(const IArgList& args, IArgList* res)
{
	res->Clear();

	for (size_t i = 0; i < m_Directories.Size(); ++i)
	{
		res->AddString(m_Directories[i]);
	}
}

int FileFinder::GetFileCount()
{
	return (int)m_Files.Size();
}

void FileFinder::GetFileList(const IArgList& args, IArgList* res)
{
	res->Clear();
	
	for (size_t i = 0; i < m_Files.Size(); ++i)
	{
		res->AddString(m_Files[i]);
	}
}

// Remove C/C++ comments.
static int remove_comments(const char* src, size_t src_size, char* dst,
	size_t* pResultSize)
{
	Assert(src != NULL);
	Assert(dst != NULL);
	Assert(pResultSize != NULL);

	int count = 0;
	const char* p = src;
	const char* end = p + src_size;
	char* out = dst;
	int state = 0;

	while (p < end)
	{
		char ch = *p++;
		
		switch (state)
		{
		case 0:
			if (ch == '/')
			{
				state = 1;
			}
			else if (ch == '\'')
			{
				*out++ = ch;
				state = 5;
			}
			else if (ch == '\"')
			{
				*out++ = ch;
				state = 7;
			}
			else
			{
				*out++ = ch;
			}
			break;
		case 1:
			if (ch == '/')
			{
				state = 4;
			}
			else if (ch == '*')
			{
				state = 2;
			}
			else
			{
				*out++ = '/';
				*out++ = ch;
				state = 0;
			}
			break;
		case 2:
			if (ch == '*')
			{
				state = 3;
			}
			break;
		case 3:
			if (ch == '/')
			{
				++count;
				state = 0;
			}
			else
			{
				state = 2;
			}
			break;
		case 4:
			if (ch == '\n')
			{
				*out++ = ch;
				++count;
				state = 0;
			}
			else if (ch == '\\')
			{
				state = 9;
			}
			break;
		case 5:
			if (ch == '\'')
			{
				state = 0;
			}
			else if (ch == '\\')
			{
				state = 6;
			}
			*out++ = ch;
			break;
		case 6:
			state = 5;
			*out++ = ch;
			break;
		case 7:
			if (ch == '\"')
			{
				state = 0;
			}
			else if (ch == '\\')
			{
				state = 8;
			}
			*out++ = ch;
			break;
		case 8:
			state = 7;
			*out++ = ch;
			break;
		case 9:
			if (ch == '\\')
			{
			}
			else if (ch == '\r')
			{
			}
			else
			{
				state = 4;
			}
			break;
		default:
			Assert(0);
			break;
		}
	}

	*pResultSize = out - dst;
	return count;
}

int FileFinder::ClearComments(const char* src_name, const char* dst_name)
{
	Assert(src_name != NULL);
	Assert(dst_name != NULL);
	
	void* fp = frame_fopen(src_name, "rb");

	if (NULL == fp)
	{
		return -1;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	size_t file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	TAutoBuffer<char, 256, FrameAlloc> auto_buf1(file_size + 1);
	char* src = auto_buf1.GetBuffer();

	if (frame_fread(src, 1, file_size, fp) != file_size)
	{
		frame_fclose(fp);
		return -1;
	}

	frame_fclose(fp);
	src[file_size] = 0;

	TAutoBuffer<char, 256, FrameAlloc> auto_buf2(file_size + 1);
	char* dst = auto_buf2.GetBuffer();
	size_t result_size;
	int count = remove_comments(src, file_size, dst, &result_size);

	if ((0 == count) && (strcmp(src_name, dst_name) == 0))
	{
		return 0;
	}

	fp = frame_fopen(dst_name, "wb");

	if (NULL == fp)
	{
		return -1;
	}

	if (frame_fwrite(dst, 1, result_size, fp) != result_size)
	{
		frame_fclose(fp);
		return -1;
	}

	frame_fclose(fp);
	return count;
}
