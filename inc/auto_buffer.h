// auto_buffer.h
// Created by lulimin on 2019/2/17.

#ifndef __AUTO_BUFFER_H
#define __AUTO_BUFFER_H

#include "common.h"

DEFINE_SYSTEM_ALLOC(AutoBuffer);

// Memory buffer use stack or heap automatic.

template<typename TYPE, size_t SIZE, typename ALLOC = AutoBufferAlloc>
class TAutoBuffer
{
private:
	typedef TAutoBuffer<TYPE, SIZE, ALLOC> self_type;

public:
	explicit TAutoBuffer(size_t size)
	{
		if (size > SIZE)
		{
			m_pBuffer = (TYPE*)m_Alloc.Alloc(sizeof(TYPE) * size);
		}
		else
		{
			m_pBuffer = m_Stack;
		}
	}

	~TAutoBuffer()
	{
		if (m_pBuffer != m_Stack)
		{
			m_Alloc.Free(m_pBuffer);
		}
	}

	TYPE* GetBuffer()
	{
		return m_pBuffer;
	}

private:
	TAutoBuffer();
	TAutoBuffer(const self_type&);
	self_type& operator=(const self_type&);

private:
	ALLOC m_Alloc;
	TYPE m_Stack[SIZE];
	TYPE* m_pBuffer;
};

#endif // __AUTO_BUFFER_H
