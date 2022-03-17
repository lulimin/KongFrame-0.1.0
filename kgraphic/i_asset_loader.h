// i_asset_loader.h
// Created by lulimin on 2020/10/14.

#ifndef __I_ASSET_LOADER_H
#define __I_ASSET_LOADER_H

#include "../inc/common.h"
#include "../inc/thread_lock.h"

// IAssetLoader

class IAssetLoader
{
public:
	IAssetLoader()
	{
		// Initialize reference counts with 1.
		m_RefCount.SetValue(1);
	}

	virtual ~IAssetLoader() {}

	// Delete self while reference counts equal zero, like below:
	// void xxxLoader::Release() { if (this->DecRefCount() == 0) delete this; }
	virtual void Release() = 0;
	// Prepare to load.
	virtual bool PreLoad(bool async) = 0;
	// Post loads.
	virtual bool PostLoad(bool async) = 0;
	// Asynchronous load finished.
	virtual bool FinishLoad(int task_type, bool succeed) = 0;

	// Get reference count.
	int GetRefCount()
	{
		return m_RefCount.GetValue();
	}

	// Increase reference count.
	int IncRefCount()
	{
		return m_RefCount.Increment();
	}

	// Decrease reference count.
	int DecRefCount()
	{
		return m_RefCount.Decrement();
	}

private:
	LockInt m_RefCount;
};

#endif // __I_ASSET_LOADER_H
