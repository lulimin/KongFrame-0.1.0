// file_finder.h
// Created by lulimin on 2019/2/21.

#ifndef __FILE_FINDER_H
#define __FILE_FINDER_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

// Search file or directory.

class FileFinder : public IObject
{
public:
	FileFinder();
	virtual ~FileFinder();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Current specified file path.
	const char* GetPath() const;
	// Current specified file name(or match symbol).
	const char* GetName() const;
	// Test directory is empty.
	bool DirectoryEmpty(const char* path, const char* name);
	// Search sub-directories under specified file path.
	bool SearchDirectory(const char* path, const char* name);
	// Search matched files under specified file path.
	bool SearchFile(const char* path, const char* name);
	// Get number of sub-directories.
	int GetDirectoryCount();
	// Get sub-directory list.
	void GetDirectoryList(const IArgList& args, IArgList* res);
	// Get number of files.
	int GetFileCount();
	// Get file name list.
	void GetFileList(const IArgList& args, IArgList* res);
	// Remove all comments in file.
	int ClearComments(const char* src_name, const char* dst_name);

private:
	void ClearResultDirectories();
	void ClearResultFiles();

private:
	FrameString m_sPath;
	FrameString m_sName;
	TPodArray<char*, 1, FrameAlloc> m_Files;
	TPodArray<char*, 1, FrameAlloc> m_Directories;
};

#endif // __FILE_FINDER_H
