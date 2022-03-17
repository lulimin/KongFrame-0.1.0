// pod_array.h
// Created by lulimin on 2019/2/14.

#ifndef __POD_ARRAY_H
#define __POD_ARRAY_H

#include "common.h"
#include <string.h>

DEFINE_SYSTEM_ALLOC(PodArray);

// Array of Plain-Old-Data.

template<typename TYPE, size_t SIZE, typename ALLOC = PodArrayAlloc>
class TPodArray
{
private:
	typedef TPodArray<TYPE, SIZE, ALLOC> self_type;

public:
	TPodArray()
	{
		m_pArray = m_Buffer;
		m_nCapacity = SIZE;
		m_nCount = 0;
	}

	TPodArray(const self_type& src)
	{
		m_nCount = src.m_nCount;

		if (m_nCount <= SIZE)
		{
			m_pArray = m_Buffer;
			m_nCapacity = SIZE;
		}
		else
		{
			m_nCapacity = src.m_nCapacity;
			m_pArray = m_Alloc.Alloc(m_nCapacity * sizeof(TYPE));
		}

		memcpy(m_pArray, src.m_pArray, m_nCount * sizeof(TYPE));
	}

	~TPodArray()
	{
		if (m_nCapacity > SIZE)
		{
			m_Alloc.Free(m_pArray);
		}
	}

	void Wipe()
	{
		if (m_nCapacity > SIZE)
		{
			m_Alloc.Free(m_pArray);
			m_pArray = m_Buffer;
			m_nCapacity = SIZE;
		}
		
		m_nCount = 0;
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);

		this->Swap(temp);
		return *this;
	}

	void Swap(self_type& src)
	{
		size_t temp_count = src.m_nCount;
		size_t temp_capacity = src.m_nCapacity;
		TYPE* temp_parray = src.m_pArray;
		TYPE temp_buffer[SIZE];

		if (temp_capacity <= SIZE)
		{
			memcpy(temp_buffer, src.m_Buffer, temp_count * sizeof(TYPE));
		}

		src.m_nCount = m_nCount;
		src.m_nCapacity = m_nCapacity;

		if (m_nCapacity <= SIZE)
		{
			memcpy(src.m_Buffer, m_Buffer, m_nCount * sizeof(TYPE));
			src.m_pArray = src.m_Buffer;
		}
		else
		{
			src.m_pArray = m_pArray;
		}

		m_nCount = temp_count;
		m_nCapacity = temp_capacity;

		if (temp_capacity <= SIZE)
		{
			memcpy(m_Buffer, temp_buffer, temp_count * sizeof(TYPE));
			m_pArray = m_Buffer;
		}
		else
		{
			m_pArray = temp_parray;
		}
	}

	bool Empty() const
	{
		return 0 == m_nCount;
	}
	
	size_t Size() const
	{
		return m_nCount;
	}
	
	const TYPE* Data() const
	{
		return m_pArray;
	}
	
	TYPE* Data()
	{
		return m_pArray;
	}
	
	void PushBack(const TYPE& data)
	{
		if (m_nCount == m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pArray, m_nCount * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pArray);
			}
			
			m_pArray = p;
			m_nCapacity = new_size;
		}
		
		m_pArray[m_nCount++] = data;
	}
	
	TYPE& PushNew()
	{
		if (m_nCount == m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pArray, m_nCount * sizeof(TYPE));
			
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pArray);
			}
			
			m_pArray = p;
			m_nCapacity = new_size;
		}
		
		return m_pArray[m_nCount++];
	}
	
	void PopBack()
	{
		Assert(m_nCount > 0);
		--m_nCount;
	}
	
	TYPE& Back()
	{
		Assert(m_nCount > 0);
		return m_pArray[m_nCount - 1];
	}
	
	const TYPE& Back() const
	{
		Assert(m_nCount > 0);
		return m_pArray[m_nCount - 1];
	}
	
	TYPE& operator[](size_t index)
	{
		Assert(index < m_nCount);
		return m_pArray[index];
	}
	
	const TYPE& operator[](size_t index) const
	{
		Assert(index < m_nCount);
		return m_pArray[index];
	}

	TYPE& Get(size_t index)
	{
		Assert(index < m_nCount);
		return m_pArray[index];
	}

	void Set(size_t index, const TYPE& value)
	{
		Assert(index < m_nCount);
		m_pArray[index] = value;
	}

	void Reserve(size_t size)
	{
		if (size > m_nCapacity)
		{
			TYPE* p = (TYPE*)m_Alloc.Alloc(size * sizeof(TYPE));
			
			memcpy(p, m_pArray, m_nCount * sizeof(TYPE));
			
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pArray);
			}
			
			m_pArray = p;
			m_nCapacity = size;
		}
	}
	
	void Resize(size_t size)
	{
		if (size > m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			
			if (new_size < size)
			{
				new_size = size;
			}

			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pArray, m_nCount * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pArray);
			}
			
			m_pArray = p;
			m_nCapacity = new_size;
		}
		
		m_nCount = size;
	}
	
	void Resize(size_t size, const TYPE& value)
	{
		if (size > m_nCapacity)
		{
			size_t new_size = m_nCapacity * 2;
			
			if (new_size < size)
			{
				new_size = size;
			}
			
			TYPE* p = (TYPE*)m_Alloc.Alloc(new_size * sizeof(TYPE));
			
			memcpy(p, m_pArray, m_nCount * sizeof(TYPE));
				
			if (m_nCapacity > SIZE)
			{
				m_Alloc.Free(m_pArray);
			}
			
			m_pArray = p;
			m_nCapacity = new_size;
		}
		
		if (size > m_nCount)
		{
			for (size_t i = m_nCount; i < size; ++i)
			{
				m_pArray[i] = value;
			}
		}
		
		m_nCount = size;
	}
	
	void Insert(size_t index, const TYPE& data)
	{
		Assert(index <= m_nCount);
		
		Resize(m_nCount + 1);

		TYPE* p = m_pArray + index;

		memmove(p + 1, p, (m_nCount - index - 1) * sizeof(TYPE));
		*p = data;
	}
	
	void Remove(size_t index)
	{
		Assert(index < m_nCount);
		
		TYPE* p = m_pArray + index;

		memmove(p, p + 1, (m_nCount - index - 1) * sizeof(TYPE));
		--m_nCount;
	}
	
	void RemoveFrom(size_t start, size_t count)
	{
		Assert((start <= m_nCount) && ((start + count) <= m_nCount));
		
		TYPE* p = m_pArray + start;

		memmove(p, p + count, (m_nCount - start - count) * sizeof(TYPE));
		m_nCount -= count;
	}
	
	void Clear()
	{
		m_nCount = 0;
	}

private:
	ALLOC m_Alloc;
	TYPE m_Buffer[SIZE];
	TYPE* m_pArray;
	size_t m_nCapacity;
	size_t m_nCount;
};

#endif // __POD_ARRAY_H
