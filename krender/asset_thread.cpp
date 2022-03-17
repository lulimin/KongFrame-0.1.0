// asset_thread.cpp
// Created by lulimin on 2020/10/14.

#include "asset_thread.h"
#include "../kgraphic/i_asset_loader.h"

#define TASK_FAILED 0
#define TASK_SUCCEED 1

// AssetThread

AssetThread::AssetThread()
{
	m_pThread = (Thread*)K_ALLOC(sizeof(Thread));
	new (m_pThread) Thread(ThreadProc, ThreadInit, NULL, this, -1, 0);
	m_pThreadYield = K_NEW(ThreadYield);
}

AssetThread::~AssetThread()
{
	K_DELETE(m_pThread);
	K_DELETE(m_pThreadYield);
}

bool AssetThread::ThreadInit(void* pParam)
{
	AssetThread* pthis = (AssetThread*)pParam;

	return true;
}

void AssetThread::ThreadProc(void* pParam)
{
	AssetThread* pthis = (AssetThread*)pParam;

	pthis->CheckInTask();
}

bool AssetThread::Start()
{
	m_pThread->Start();
	return true;
}

bool AssetThread::Stop()
{
	m_pThread->Stop();
	return true;
}

bool AssetThread::AddTask(int task_type, IAssetLoader* pLoader)
{
	Assert(pLoader != NULL);

	task_in_t task;

	task.nTaskType = task_type;
	task.pAssetLoader = pLoader;

	// Increase reference count of loader.
	pLoader->IncRefCount();

	ScopedLock scoped_lock(&m_TaskInLock);

	if (!m_InTasks.Enqueue(task))
	{
		K_LOG("(AssetThread::AddTask)Add task error.");
		pLoader->Release();
		return false;
	}

	// Thread wake up.
	m_pThreadYield->Signal();
	return true;
}

void AssetThread::ClearInTasks()
{
	ScopedLock scoped_lock(&m_TaskInLock);
	task_in_t task_in;

	while (m_InTasks.Dequeue(&task_in))
	{
		task_in.pAssetLoader->Release();
	}
}

void AssetThread::ClearOutTasks()
{
	ScopedLock scoped_lock(&m_TaskOutLock);
	task_out_t task_out;

	while (m_OutTasks.Dequeue(&task_out))
	{
		task_out.pAssetLoader->Release();
	}
}

int AssetThread::GetTaskCount()
{
	return (int)(m_InTasks.GetTaskCount() + m_OutTasks.GetTaskCount());
}

void AssetThread::CheckOutTask()
{
	if (m_OutTasks.Empty())
	{
		return;
	}

	task_out_t task;

	while (this->GetTaskOut(&task))
	{
		IAssetLoader* pLoader = task.pAssetLoader;

		if (pLoader->GetRefCount() > 1)
		{
			if (task.bSucceed)
			{
				if (pLoader->PostLoad(true))
				{
					pLoader->FinishLoad(task.nTaskType, true);
				}
				else
				{
					pLoader->FinishLoad(task.nTaskType, false);
				}
			}
			else
			{
				pLoader->FinishLoad(task.nTaskType, false);
			}
		}

		pLoader->Release();
	}
}

void AssetThread::CheckInTask()
{
	if (m_InTasks.Empty())
	{
		// Yield asset thread.
		m_pThreadYield->Wait(100);
	}

	if (m_InTasks.Empty())
	{
		return;
	}

	task_in_t task;

	while (this->GetTaskIn(&task))
	{
		IAssetLoader* pLoader = task.pAssetLoader;

		if (pLoader->GetRefCount() == 1)
		{
			// Asset loader already released, not need loading.
			this->EndTask(task.nTaskType, pLoader, false);
			continue;
		}

		bool succeed = pLoader->PreLoad(true);

		this->EndTask(task.nTaskType, pLoader, succeed);
	}
}

bool AssetThread::EndTask(int task_type, IAssetLoader* pLoader, bool succeed)
{
	Assert(pLoader != NULL);

	task_out_t task;

	task.nTaskType = task_type;
	task.pAssetLoader = pLoader;
	task.bSucceed = succeed;

	ScopedLock scoped_lock(&m_TaskOutLock);

	if (!m_OutTasks.Enqueue(task))
	{
		K_LOG("(AssetThread::EndTask)Add task error.");
		pLoader->Release();
		return false;
	}
	
	return true;
}

bool AssetThread::GetTaskIn(task_in_t* pTask)
{
	Assert(pTask != NULL);

	if (m_InTasks.Empty())
	{
		return false;
	}

	ScopedLock scoped_lock(&m_TaskInLock);

	return m_InTasks.Dequeue(pTask);
}

bool AssetThread::GetTaskOut(task_out_t* pTask)
{
	Assert(pTask != NULL);

	if (m_OutTasks.Empty())
	{
		return false;
	}

	ScopedLock scoped_lock(&m_TaskOutLock);

	return m_OutTasks.Dequeue(pTask);
}
