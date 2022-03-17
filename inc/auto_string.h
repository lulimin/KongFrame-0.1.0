// auto_string.h
// Created by lulimin on 2019/2/17.

#ifndef __AUTO_STRING_H
#define __AUTO_STRING_H

#include "common.h"
#include "char_traits.h"

DEFINE_SYSTEM_ALLOC(AutoString);

// Short string optimize.

template<typename TYPE, size_t SIZE, typename TRAITS = TCharTraits<TYPE>, 
	typename ALLOC = AutoStringAlloc>
class TAutoString
{
private:
	typedef TAutoString<TYPE, SIZE, TRAITS, ALLOC> self_type;

public:
	TAutoString()
	{
		m_Stack[0] = 0;
		m_pBuffer = m_Stack;
		m_nBufferSize = SIZE;
		m_nLength = 0;
	}

	TAutoString(const self_type& src)
	{
		this->RawSet(src.CString(), src.Length());
	}

	TAutoString(const TYPE* src)
	{
		this->RawSet(src, TRAITS::Length(src));
	}

	TAutoString(const TYPE* src, size_t len)
	{
		this->RawSet(src, len);
	}

	TAutoString(const TYPE* s1, const TYPE* s2)
	{
		this->RawSet2(s1, TRAITS::Length(s1), s2, TRAITS::Length(s2));
	}

	TAutoString(const TYPE* s1, size_t len1, const TYPE* s2, size_t len2)
	{
		this->RawSet2(s1, len1, s2, len2);
	}

	~TAutoString()
	{
		if (m_nBufferSize > SIZE)
		{
			m_Alloc.Free(m_pBuffer);
		}
	}

	void Wipe()
	{
		if (m_nBufferSize > SIZE)
		{
			m_Alloc.Free(m_pBuffer);
			m_pBuffer = m_Stack;
			m_nBufferSize = SIZE;
		}
		
		m_pBuffer[0] = 0;
		m_nLength = 0;
	}

	self_type& operator=(const self_type& src)
	{
		return this->RawAssign(src.CString(), src.Length());
	}

	self_type& operator=(const TYPE* src)
	{
		return this->RawAssign(src, TRAITS::Length(src));
	}

	self_type& operator=(TYPE ch)
	{
		return this->RawAssign(&ch, 1);
	}

	self_type& operator+=(const self_type& src)
	{
		return this->RawAppend(src.CString(), src.Length());
	}

	self_type& operator+=(const TYPE* src)
	{
		return this->RawAppend(src, TRAITS::Length(src));
	}

	self_type& operator+=(TYPE ch)
	{
		return this->RawAppend(&ch, 1);
	}

	void Swap(self_type& src)
	{
		size_t temp_length = src.m_nLength;
		size_t temp_buffer_size = src.m_nBufferSize;
		TYPE* temp_buffer = src.m_pBuffer;
		TYPE temp_stack[SIZE];

		if (temp_buffer_size <= SIZE)
		{
			TRAITS::Copy(temp_stack, src.m_Stack, temp_length + 1);
		}

		src.m_nLength = m_nLength;
		src.m_nBufferSize = m_nBufferSize;

		if (m_nBufferSize <= SIZE)
		{
			TRAITS::Copy(src.m_Stack, m_Stack, m_nLength + 1);
			src.m_pBuffer = src.m_Stack;
		} 
		else
		{
			src.m_pBuffer = m_pBuffer;
		}

		m_nLength = temp_length;
		m_nBufferSize = temp_buffer_size;

		if (temp_buffer_size <= SIZE)
		{
			TRAITS::Copy(m_Stack, temp_stack, temp_length + 1);
			m_pBuffer = m_Stack;
		} 
		else
		{
			m_pBuffer = temp_buffer;
		}
	}

	bool Empty() const
	{
		return 0 == m_nLength;
	}

	size_t Length() const
	{
		return m_nLength;
	}

	size_t Capacity() const
	{
		return m_nBufferSize;
	}

	void Clear()
	{
		m_pBuffer[0] = 0;
		m_nLength = 0;
	}

	const TYPE* CString() const
	{
		return m_pBuffer;
	}

	int Compare(const self_type& src) const
	{
		return TRAITS::Compare(m_pBuffer, src.CString());
	}

	int Compare(const TYPE* src) const
	{
		return TRAITS::Compare(m_pBuffer, src);
	}

	self_type& Append(const self_type& src)
	{
		return this->RawAppend(src.CString(), src.Length());
	}

	self_type& Append(const TYPE* src)
	{
		return this->RawAppend(src, TRAITS::Length(src));
	}

	self_type& Append(TYPE ch)
	{
		return this->RawAppend(&ch, 1);
	}

	self_type& Append(const TYPE* src, size_t len)
	{
		return this->RawAppend(src, len);
	}

	self_type SubStr(size_t begin, size_t len)
	{
		return self_type(&m_pBuffer[begin], len);
	}

	size_t NPos()
	{
		return (size_t)(-1);
	}

	size_t Find(const TYPE* src, const size_t begin = 0)
	{
		return TRAITS::Find(m_pBuffer, src, begin);
	}

	size_t FindLastOf(TYPE value)
	{
		for (int i = (int)(m_nLength -  1); i >= 0; --i)
		{
			if (m_pBuffer[i] == value)
			{
				return i;
			}
		}

		return (size_t)(-1);
	}

private:
	void RawSet(const TYPE* src, size_t len)
	{
		m_nLength = len;

		if (m_nLength < SIZE)
		{
			m_nBufferSize = SIZE;
			m_pBuffer = m_Stack;
		}
		else
		{
			m_nBufferSize = (m_nLength + 1) * 2;
			m_pBuffer = (TYPE*)m_Alloc.Alloc(m_nBufferSize * sizeof(TYPE));
		}

		TRAITS::Copy(m_pBuffer, src, len);
		m_pBuffer[m_nLength] = 0;
	}

	void RawSet2(const TYPE* s1, size_t len1, const TYPE* s2, size_t len2)
	{
		m_nLength = len1 + len2;

		if (m_nLength < SIZE)
		{
			m_nBufferSize = SIZE;
			m_pBuffer = m_Stack;
		}
		else
		{
			m_nBufferSize = (m_nLength + 1) * 2;
			m_pBuffer = (TYPE*)m_Alloc.Alloc(m_nBufferSize * sizeof(TYPE));
		}

		TRAITS::Copy(m_pBuffer, s1, len1);
		TRAITS::Copy(m_pBuffer + len1, s2, len2);
		m_pBuffer[m_nLength] = 0;
	}

	self_type& RawAssign(const TYPE* s, size_t len)
	{
		if (len < m_nBufferSize)
		{
			TRAITS::Copy(m_pBuffer, s, len);
			m_pBuffer[len] = 0;
			m_nLength = len;
		}
		else
		{
			self_type temp(s, len);
			
			this->Swap(temp);
		}

		return *this;
	}

	self_type& RawAppend(const TYPE* s, size_t len)
	{
		size_t new_len = m_nLength + len;

		if (new_len < m_nBufferSize)
		{
			TRAITS::Copy(m_pBuffer + m_nLength, s, len);
			m_pBuffer[new_len] = 0;
			m_nLength = new_len;
		}
		else
		{
			self_type temp(m_pBuffer, m_nLength, s, len);
			
			this->Swap(temp);
		}

		return *this;
	}

private:
	ALLOC m_Alloc;
	TYPE m_Stack[SIZE];
	TYPE* m_pBuffer;
	size_t m_nBufferSize;
	size_t m_nLength;
};

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return s1.Compare(s2) == 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TYPE* s2)
{
	return s1.Compare(s2) == 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TYPE* s1,
	const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return s2.Compare(s1) == 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return s1.Compare(s2) != 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TYPE* s2)
{
	return s1.Compare(s2) != 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TYPE* s1,
	const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return s2.Compare(s1) != 0;
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TAutoString<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1,
	const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return TAutoString<TYPE, SIZE, TRAITS, ALLOC>(s1).Append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TAutoString<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2)
{
	return TAutoString<TYPE, SIZE, TRAITS, ALLOC>(s1).Append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TAutoString<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s1, TYPE ch)
{
	return TAutoString<TYPE, SIZE, TRAITS, ALLOC>(s1).Append(ch);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline TAutoString<TYPE, SIZE, TRAITS, ALLOC> 
operator+(const TYPE* s1, const TAutoString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
	return TAutoString<TYPE, SIZE, TRAITS, ALLOC>(s1).Append(s2);
}

#endif // __AUTO_STRING_H
