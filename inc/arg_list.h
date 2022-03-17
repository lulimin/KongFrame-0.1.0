// arg_list.h
// Created by lulimin on 2019/2/14.

#ifndef __ARG_LIST_H
#define __ARG_LIST_H

#include "i_arg_list.h"

DEFINE_SYSTEM_ALLOC(ArgList);

// Argument data list.

template<size_t DATA_SIZE, size_t BUFFER_SIZE, typename ALLOC = ArgListAlloc>
class TArgList : public IArgList
{
private:
	struct arg_data_t
	{
		int nType;
		union
		{
			bool bDataBoolean;
			int nDataInt32;
			int64_t nDataInt64;
			float fDataFloat;
			double dDataDouble;
			size_t nDataString;
			uint64_t nDataUniqid;
			void* pDataPointer;
		};
	};

public:
	TArgList()
	{
		Assert(DATA_SIZE > 0);
		Assert(BUFFER_SIZE > 0);
		
		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
	}

	TArgList(const TArgList& src)
	{
		Assert(DATA_SIZE > 0);
		Assert(BUFFER_SIZE > 0);
		
		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
		this->RawAppend(src, 0, src.GetCount());
	}

	virtual ~TArgList()
	{
		if (m_nDataSize > DATA_SIZE)
		{
			m_Alloc.Free(m_pData);
		}
		
		if (m_nBufferSize > BUFFER_SIZE)
		{
			m_Alloc.Free(m_pBuffer);
		}
	}

	TArgList& operator=(const TArgList& src)
	{
		if (m_nDataSize > DATA_SIZE)
		{
			m_Alloc.Free(m_pData);
		}
		
		if (m_nBufferSize > BUFFER_SIZE)
		{
			m_Alloc.Free(m_pBuffer);
		}
		
		m_pData = m_DataStack;
		m_nDataSize = DATA_SIZE;
		m_nDataUsed = 0;
		m_pBuffer = m_BufferStack;
		m_nBufferSize = BUFFER_SIZE;
		m_nBufferUsed = 0;
		this->RawAppend(src, 0, src.GetCount());
		return *this;
	}

	// Get number of data.
	virtual size_t GetCount() const 
	{
		return m_nDataUsed;
	}
	
	// Get type of data.
	virtual int GetType(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		return m_pData[index].nType;
	}
	
	// Get value.
	virtual bool GetBoolean(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return false;
		}

		switch (m_pData[index].nType)
		{
		case DT_BOOLEAN:
			return m_pData[index].bDataBoolean;
		case DT_INT32:
			return m_pData[index].nDataInt32 != 0;
		case DT_INT64:
			return m_pData[index].nDataInt64 != 0;
		default:
			break;
		}

		return false;
	}

	virtual int GetInt32(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		switch (m_pData[index].nType)
		{
		case DT_BOOLEAN:
			return (int)m_pData[index].bDataBoolean;
		case DT_INT32:
			return m_pData[index].nDataInt32;
		case DT_INT64:
			return (int)m_pData[index].nDataInt64;
		case DT_FLOAT:
			return (int)m_pData[index].fDataFloat;
		case DT_DOUBLE:
			return (int)m_pData[index].dDataDouble;
		default:
			break;
		}

		return 0;
	}

	virtual int64_t GetInt64(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0;
		}

		switch (m_pData[index].nType)
		{
		case DT_BOOLEAN:
			return (int64_t)m_pData[index].bDataBoolean;
		case DT_INT32:
			return (int64_t)m_pData[index].nDataInt32;
		case DT_INT64:
			return m_pData[index].nDataInt64;
		case DT_FLOAT:
			return (int64_t)m_pData[index].fDataFloat;
		case DT_DOUBLE:
			return (int64_t)m_pData[index].dDataDouble;
		default:
			break;
		}

		return 0;
	}

	virtual float GetFloat(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0.0F;
		}

		switch (m_pData[index].nType)
		{
		case DT_BOOLEAN:
			return (float)m_pData[index].bDataBoolean;
		case DT_INT32:
			return (float)m_pData[index].nDataInt32;
		case DT_INT64:
			return (float)m_pData[index].nDataInt64;
		case DT_FLOAT:
			return m_pData[index].fDataFloat;
		case DT_DOUBLE:
			return (float)m_pData[index].dDataDouble;
		default:
			break;
		}

		return 0.0F;
	}

	virtual double GetDouble(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return 0.0;
		}

		switch (m_pData[index].nType)
		{
		case DT_BOOLEAN:
			return (double)m_pData[index].bDataBoolean;
		case DT_INT32:
			return (double)m_pData[index].nDataInt32;
		case DT_INT64:
			return (double)m_pData[index].nDataInt64;
		case DT_FLOAT:
			return (double)m_pData[index].fDataFloat;
		case DT_DOUBLE:
			return m_pData[index].dDataDouble;
		default:
			break;
		}

		return 0.0;
	}

	virtual const char* GetString(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return "";
		}

		switch (m_pData[index].nType)
		{
		case DT_STRING:
			return m_pBuffer + m_pData[index].nDataString;
		default:
			break;
		}

		return "";
	}

	virtual uniqid_t GetUniqid(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return uniqid_t();
		}

		switch (m_pData[index].nType)
		{
		case DT_UNIQID:
			return uniqid_t(m_pData[index].nDataUniqid);
		default:
			break;
		}

		return uniqid_t();
	}

	virtual void* GetPointer(size_t index) const
	{
		if (index >= m_nDataUsed)
		{
			return NULL;
		}

		switch (m_pData[index].nType)
		{
		case DT_POINTER:
			return m_pData[index].pDataPointer;
		default:
			break;
		}

		return NULL;
	}

	// Add data.
	virtual bool AddBoolean(bool value)
	{
		arg_data_t* p = this->AddArgData();

		p->nType = DT_BOOLEAN;
		p->bDataBoolean = value;
		return true;
	}
	
	virtual bool AddInt32(int value)
	{
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_INT32;
		p->nDataInt32 = value;
		return true;
	}
	
	virtual bool AddInt64(int64_t value)
	{
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_INT64;
		p->nDataInt64 = value;
		return true;
	}
	
	virtual bool AddFloat(float value)
	{
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_FLOAT;
		p->fDataFloat = value;
		return true;
	}
	
	virtual bool AddDouble(double value)
	{
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_DOUBLE;
		p->dDataDouble = value;
		return true;
	}
	
	virtual bool AddString(const char* value)
	{
		Assert(value != NULL);
		
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_STRING;
		p->nDataString = m_nBufferUsed;

		const size_t value_size = strlen(value) + 1;
		char* v = this->AddBuffer(value_size);
		
		memcpy(v, value, value_size);
		return true;
	}
	
	virtual bool AddUniqid(const uniqid_t& value)
	{
		arg_data_t* p = this->AddArgData();
		
		p->nType = DT_UNIQID;
		p->nDataUniqid = value.nValue64;
		return true;
	}

	virtual bool AddPointer(void* value)
	{
		arg_data_t* p = this->AddArgData();

		p->nType = DT_POINTER;
		p->pDataPointer = value;
		return true;
	}

	// Clear all values.
	virtual void Clear()
	{
		m_nDataUsed = 0;
		m_nBufferUsed = 0;
	}

	// Merge two lists.
	virtual bool Concat(const IArgList& src)
	{
		this->RawAppend(src, 0, src.GetCount());
		return true;
	}

	// Append to tail of list.
	virtual bool Append(const IArgList& src, size_t start, size_t count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		size_t end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}

		this->RawAppend(src, start, end);
		return true;
	}

private:
	arg_data_t* AddArgData()
	{
		if (m_nDataUsed >= m_nDataSize)
		{
			size_t new_size = m_nDataSize * 2;
			arg_data_t* p = (arg_data_t*)m_Alloc.Alloc(
				new_size * sizeof(arg_data_t));

			memcpy(p, m_pData, m_nDataUsed * sizeof(arg_data_t));

			if (m_nDataSize > DATA_SIZE)
			{
				m_Alloc.Free(m_pData);
			}

			m_pData = p;
			m_nDataSize = new_size;
		}
		
		return m_pData + m_nDataUsed++;
	}
	
	char* AddBuffer(size_t need_size)
	{
		size_t new_used = m_nBufferUsed + need_size;
		
		if (new_used > m_nBufferSize)
		{
			size_t new_size = m_nBufferSize * 2;
			
			if (new_used > new_size)
			{
				new_size = new_used * 2;
			}
			
			char* p = (char*)m_Alloc.Alloc(new_size);
			
			memcpy(p, m_pBuffer, m_nBufferUsed);
			
			if (m_nBufferSize > BUFFER_SIZE)
			{
				m_Alloc.Free(m_pBuffer);
			}
			
			m_pBuffer = p;
			m_nBufferSize = new_size;
		}
		
		char* ret = m_pBuffer + m_nBufferUsed;

		m_nBufferUsed = new_used;
		return ret;
	}

	void RawAppend(const IArgList& src, size_t start, size_t end)
	{
		for (size_t i = start; i < end; ++i)
		{
			switch (src.GetType(i))
			{
			case DT_BOOLEAN:
				this->AddBoolean(src.GetBoolean(i));
				break;
			case DT_INT32:
				this->AddInt32(src.GetInt32(i));
				break;
			case DT_INT64:
				this->AddInt64(src.GetInt64(i));
				break;
			case DT_FLOAT:
				this->AddFloat(src.GetFloat(i));
				break;
			case DT_DOUBLE:
				this->AddDouble(src.GetDouble(i));
				break;
			case DT_STRING:
				this->AddString(src.GetString(i));
				break;
			case DT_UNIQID:
				this->AddUniqid(src.GetUniqid(i));
				break;
			case DT_POINTER:
				this->AddPointer(src.GetPointer(i));
				break;
			default:
				Assert(0);
				break;
			}
		}
	}
	
private:
	ALLOC m_Alloc;
	arg_data_t m_DataStack[DATA_SIZE];
	arg_data_t* m_pData;
	size_t m_nDataSize;
	size_t m_nDataUsed;
	char m_BufferStack[BUFFER_SIZE];
	char* m_pBuffer;
	size_t m_nBufferSize;
	size_t m_nBufferUsed;
};

#endif // __ARG_LIST_H
