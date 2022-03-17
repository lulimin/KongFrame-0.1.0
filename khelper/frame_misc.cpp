// frame_misc.cpp
// Created by lulimin on 2019/2/21.

#include "frame_misc.h"
#include "../inc/auto_buffer.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_file.h"

/// \file frame_misc.cpp
/// \brief Frame misc functions.

/// object: FrameMisc
/// \brief Frame misc functions object.
DEF_OBJECT(FrameMisc, IObject);

/// function: string GetDecimal(double value, int fractions)
/// \brief Double value convert to decimal string.
/// \param value Source data.
/// \param fractions Number of decimal fractions.
DEF_FUNC_2(ArgString, FrameMisc, GetDecimal, double, int);
/// function: int GetBoundType(object id)
/// \brief Get object hook type.
/// \param id Object id.
DEF_FUNC_1(int, FrameMisc, GetBoundType, const uniqid_t&);
/// function: string GetBoundName(object id)
/// \brief Get object hook name.
/// \param id Object id.
DEF_FUNC_1(const char*, FrameMisc, GetBoundName, const uniqid_t&);
/// function: string GetPlatformName()
/// \brief Get current platform name.
DEF_FUNC_0(const char*, FrameMisc, GetPlatformName);
/// function: bool GetDebugMode()
/// \brief Whether in debug mode.
DEF_FUNC_0(bool, FrameMisc, GetDebugMode);
/// function: int GetLogLevel()
/// \brief Get current log level.
DEF_FUNC_0(int, FrameMisc, GetLogLevel);
/// function: bool SetLogLevel(int log_level)
/// \brief Set current log level.
/// \param log_level New level of log.
DEF_FUNC_1(bool, FrameMisc, SetLogLevel, int);
/// function: bool DumpInformation(string info_type, string file_name)
/// \brief Dump frame information to file.
/// \param info_type Type of information.
/// \param file_name File name.
DEF_FUNC_2(bool, FrameMisc, DumpInformation, const char*, const char*);
/// function: bool FindFile(string file_name)
/// \brief Find file.
/// \param file_name File name.
DEF_FUNC_1(bool, FrameMisc, FindFile, const char*);
/// function: bool RemoveFile(string file_name)
/// \brief Delete file.
/// \param file_name File name.
DEF_FUNC_1(bool, FrameMisc, RemoveFile, const char*);
/// function: bool FindDir(string dir_name)
/// \brief Find directory.
/// \param dir_name Directory name.
DEF_FUNC_1(bool, FrameMisc, FindDir, const char*);
/// function: bool CreateDir(string dir_name)
/// \brief Create directory.
/// \param dir_name Directory name.
DEF_FUNC_1(bool, FrameMisc, CreateDir, const char*);
/// function: bool RemoveDir(string dir_name)
/// \brief Delete directory.
/// \param dir_name Directory name.
DEF_FUNC_1(bool, FrameMisc, RemoveDir, const char*);

// FrameMisc

FrameMisc::FrameMisc()
{
}

FrameMisc::~FrameMisc()
{
}

bool FrameMisc::Startup(const IArgList& args)
{
	return true;
}

bool FrameMisc::Shutdown()
{
	return true;
}

ArgString FrameMisc::GetDecimal(double value, int fractions)
{
	char buf[64] = { 0 };

	if (0 == fractions)
	{
		plat_int64_to_string(buf, sizeof(buf), (int64_t)value);
	}
	else if ((fractions > 0) && (fractions < 10))
	{
		char fmt[8];

		fmt[0] = '%';
		fmt[1] = '.';
		fmt[2] = '0' + (char)fractions;
		fmt[3] = 'f';
		fmt[4] = 0;
		plat_safe_sprintf(buf, sizeof(buf), fmt, value);
	}

	return ArgString(buf);
}

int FrameMisc::GetBoundType(const uniqid_t& id)
{
	IObject* pObj = frame_get_object(id);

	if (NULL == pObj)
	{
		return 0;
	}

	IObjectHook* pHook = pObj->GetHook();

	if (NULL == pHook)
	{
		return 0;
	}

	return pHook->GetType();
}

const char* FrameMisc::GetBoundName(const uniqid_t& id)
{
	IObject* pObj = frame_get_object(id);

	if (NULL == pObj)
	{
		return "";
	}

	IObjectHook* pHook = pObj->GetHook();

	if (NULL == pHook)
	{
		return "";
	}

	return pHook->GetName();
}

const char* FrameMisc::GetPlatformName()
{
#if defined(K_PLATFORM_WINDOWS)
	return "windows";
#elif defined(K_PLATFORM_LINUX)
	return "linux";
#elif defined(K_PLATFORM_ANDROID)
	return "android";
#elif defined(K_PLATFORM_IOS)
	return "ios";
#elif defined(K_PLATFORM_MAC)
	return "mac";
#endif
}

bool FrameMisc::GetDebugMode()
{
#ifdef K_DEBUG
	return true;
#else
	return false;
#endif // K_DEBUG
}

int FrameMisc::GetLogLevel()
{
	return frame_get_log_level();
}

bool FrameMisc::SetLogLevel(int log_level)
{
	return frame_set_log_level(log_level);
}

bool FrameMisc::DumpInformation(const char* info_type, const char* file_name)
{
	Assert(info_type != NULL);
	Assert(file_name != NULL);

	return frame_dump_information(info_type, file_name);
}

bool FrameMisc::FindFile(const char* file_name)
{
	Assert(file_name != NULL);

	return frame_get_file_access()->FileExists(file_name);
}

bool FrameMisc::RemoveFile(const char* file_name)
{
	Assert(file_name != NULL);

	return frame_get_file_access()->FileDelete(file_name);
}

bool FrameMisc::FindDir(const char* dir_name)
{
	Assert(dir_name != NULL);

	return frame_get_file_access()->DirectoryExists(dir_name);
//	size_t len = strlen(dir_name);
//
//	if (0 == len)
//	{
//		return false;
//	}
//
//	char last_char = dir_name[len - 1];
//
//	if ((last_char == '\\') || (last_char == '/'))
//	{
//		TAutoBuffer<char, 256, FrameAlloc> auto_buf(len);
//		char* buf = auto_buf.GetBuffer();
//
//		memcpy(buf, dir_name, len - 1);
//		buf[len - 1] = 0;
//		return frame_get_file_access()->DirectoryExists(buf);
//	}
//	else
//	{
//		return frame_get_file_access()->DirectoryExists(dir_name);
//	}
}

bool FrameMisc::CreateDir(const char* dir_name)
{
	Assert(dir_name != NULL);

	return frame_get_file_access()->DirectoryCreate(dir_name);
}

bool FrameMisc::RemoveDir(const char* dir_name)
{
	Assert(dir_name != NULL);

	return frame_get_file_access()->DirectoryDelete(dir_name);
}
