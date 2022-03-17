// asset_thread.h
// Created by lulimin on 2020/10/14.

#ifndef __ASSET_THREAD_H
#define __ASSET_THREAD_H

#include "../inc/common.h"
#include "../inc/frame_mem.h"
#include "../inc/thread.h"
#include "../inc/thread_lock.h"
#include "asset_task_buffer.h"

DEFINE_FRAME_ALLOC(AssetThread);

// AssetThread

class IAssetLoader;

class AssetThread
{
private:
	// Input task.
	struct task_in_t
	{
		int nTaskType;
		IAssetLoader* pAssetLoader;
	};

	// Output task.
	struct task_out_t
	{
		int nTaskType;
		IAssetLoader* pAssetLoader;
		bool bSucceed;
	};

	static bool ThreadInit(void* pParam);
	static void ThreadProc(void* pParam);

public:
	AssetThread();
	~AssetThread();

	// Start asset thread.
	bool Start();
	// Stop asset thread.
	bool Stop();
	// Add asset loading task.
	bool AddTask(int task_type, IAssetLoader* pLoader);
	// Clear input tasks.
	void ClearInTasks();
	// Clear output tasks.
	void ClearOutTasks();
	// Get number of tasks.
	int GetTaskCount();
	// Check out task.
	void CheckOutTask();

private:
	AssetThread(const AssetThread&);
	AssetThread& operator=(const AssetThread&);

	// Thread working.
	void CheckInTask();
	// Task completed.
	bool EndTask(int task_type, IAssetLoader* pLoader, bool succeed);
	// Get input task.
	bool GetTaskIn(task_in_t* pTask);
	// Get output task.
	bool GetTaskOut(task_out_t* pTask);

private:
	Thread* m_pThread;
	ThreadYield* m_pThreadYield;
	ThreadLock m_TaskInLock;
	ThreadLock m_TaskOutLock;
	TAssetTaskBuffer<task_in_t, AssetThreadAlloc> m_InTasks;
	TAssetTaskBuffer<task_out_t, AssetThreadAlloc> m_OutTasks;
};

#endif // __ASSET_THREAD_H
