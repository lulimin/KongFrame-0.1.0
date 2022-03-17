// asset_task_buffer.h
// Created by lulimin on 2020/10/14.

#ifndef __ASSET_TASK_BUFFER_H
#define __ASSET_TASK_BUFFER_H

#include "../inc/common.h"

DEFINE_SYSTEM_ALLOC(AssetTaskBuffer);

// Fixed length task buffer.

template<typename TYPE, typename ALLOC = AssetTaskBufferAlloc>
class TAssetTaskBuffer  
{
private:
	typedef TAssetTaskBuffer<TYPE, ALLOC> self_type;
	
public:
	explicit TAssetTaskBuffer(size_t size = 0)
	{
		m_nTaskSize = size;
		m_nTaskIn = 0;
		m_nTaskOut = 0;
		m_nTaskCount = 0;
		m_nTaskLimit = 0;
		
		if (size > 0)
		{
			m_pTaskBuffer = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * size);
		}
		else
		{
			m_pTaskBuffer = NULL;
		}
	}
	
	TAssetTaskBuffer(const self_type& src)
	{
		m_nTaskSize = src.m_nTaskSize;
		m_nTaskIn = src.m_nTaskIn;
		m_nTaskOut = src.m_nTaskOut;
		m_nTaskCount = src.m_nTaskCount;
		m_nTaskLimit = src.m_nTaskLimit;
		
		if (m_nTaskSize > 0)
		{
			m_pTaskBuffer = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * m_nTaskSize);
			memcpy(m_pTaskBuffer, src.m_pTaskBuffer, 
				m_nTaskSize * sizeof(TYPE));
		}
		else
		{
			m_pTaskBuffer = NULL;
		}
	}
	
	~TAssetTaskBuffer()
	{
		if (m_pTaskBuffer)
		{
			m_Alloc.Free(m_pTaskBuffer);
		}
	}
	
	self_type& operator=(const self_type& src)
	{
		self_type temp(src);
		
		this->Swap(temp);
		return *this;
	}

	void Swap(self_type& src)
	{
		TYPE* temp_task_buffer = m_pTaskBuffer;
		size_t temp_task_size = m_nTaskSize;
		size_t temp_task_in = m_nTaskIn;
		size_t temp_task_out = m_nTaskOut;
		size_t temp_task_count = m_nTaskCount;
		size_t temp_task_limit = m_nTaskLimit;
		
		m_pTaskBuffer = src.m_pTaskBuffer;
		m_nTaskSize = src.m_nTaskSize;
		m_nTaskIn = src.m_nTaskIn;
		m_nTaskOut = src.m_nTaskOut;
		m_nTaskCount = src.m_nTaskCount;
		m_nTaskLimit = src.m_nTaskLimit;
		src.m_pTaskBuffer = temp_task_buffer;
		src.m_nTaskSize = temp_task_size;
		src.m_nTaskIn = temp_task_in;
		src.m_nTaskOut = temp_task_out;
		src.m_nTaskCount = temp_task_count;
		src.m_nTaskLimit = temp_task_limit;
	}	

	// Set limit number of tasks.
	void SetTaskLimit(size_t value)
	{
		m_nTaskLimit = value;
	}

	// Get limit number of tasks.
	size_t GetTaskLimit() const 
	{ 
		return m_nTaskLimit; 
	}
	
	// Get number of tasks.
	size_t GetTaskCount() const
	{
		return m_nTaskCount;
	}

	// Test empty.
	bool Empty() const
	{ 
		return m_nTaskIn == m_nTaskOut;
	}
	
	// Clear tasks.
	void Clear()
	{
		m_nTaskIn = 0;
		m_nTaskOut = 0;
		m_nTaskCount = 0;
	}

	// Enqueue one task.
	bool Enqueue(const TYPE& task)
	{
		if (this->CheckOverflow())
		{
			return false;
		}

		if (0 == m_nTaskSize)
		{
			this->ExpandBuffer();
		}
		else if (((m_nTaskIn + 1) % m_nTaskSize) == m_nTaskOut)
		{
			this->ExpandBuffer();
		}
		
		m_pTaskBuffer[m_nTaskIn] = task;
		m_nTaskIn = (m_nTaskIn + 1) % m_nTaskSize;
		m_nTaskCount++;
		return true;
	}
	
	// Dequeue one task and delete it.
	bool Dequeue(TYPE* pTask)
	{
		Assert(pTask != NULL);
		
		if (m_nTaskIn == m_nTaskOut)
		{
			return false;
		}
		
		*pTask = m_pTaskBuffer[m_nTaskOut];
		m_nTaskOut = (m_nTaskOut + 1) % m_nTaskSize;
		m_nTaskCount--;
		return true;
	}
	
private:
	// Check buffer overflow.
	bool CheckOverflow() const
	{
		// Limit number of tasks.
		if (m_nTaskLimit > 0) 
		{
			if (m_nTaskCount >= m_nTaskLimit)
			{
				return true;
			}
		}
		
		return false;
	}

	// Expand task buffer.
	bool ExpandBuffer()
	{
		size_t new_size = m_nTaskSize * 2;
	
		if (0 == new_size)
		{
			new_size = 256;
		}
		
		self_type temp(new_size);
		TYPE task;
		
		while (this->Dequeue(&task))
		{
			temp.Enqueue(task);
		}
		
		this->Swap(temp);
		return true;
	}

private:
	ALLOC m_Alloc;
	TYPE* m_pTaskBuffer;
	size_t m_nTaskSize;
	size_t m_nTaskIn;
	size_t m_nTaskOut;
	size_t m_nTaskCount;
	size_t m_nTaskLimit;
};

#endif // __ASSET_TASK_BUFFER_H
