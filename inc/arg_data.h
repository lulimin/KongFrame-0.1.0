// arg_data.h
// Created by lulimin on 2019/2/14.

#ifndef __ARG_DATA_H
#define __ARG_DATA_H

#include "i_arg_data.h"

DEFINE_SYSTEM_ALLOC(ArgData);

// Argument data.

template<size_t BUFFER_SIZE, typename ALLOC = ArgDataAlloc>
class TArgData : public IArgData 
{
public:
	typedef TArgData<BUFFER_SIZE, ALLOC> self_type;
	
public:
	TArgData()
	{
		m_nType = 0;
		m_nDataInt64 = 0;
	}

	TArgData(const self_type& src)
	{
		m_nType = src.m_nType;

		switch (m_nType)
		{
		case DT_BOOLEAN:
			m_bDataBoolean = src.m_bDataBoolean;
			break;
		case DT_INT32:
			m_nDataInt32 = src.m_nDataInt32;
			break;
		case DT_INT64:
			m_nDataInt64 = src.m_nDataInt64;
			break;
		case DT_FLOAT:
			m_fDataFloat = src.m_fDataFloat;
			break;
		case DT_DOUBLE:
			m_dDataDouble = src.m_dDataDouble;
			break;
		case DT_STRING:
			this->InnerSetString(src.GetString());
			break;
		case DT_UNIQID:
			m_nDataUniqid = src.m_nDataUniqid;
			break;
		case DT_POINTER:
			m_pDataPointer = src.m_pDataPointer;
			break;
		default:
			break;
		}
	}

	explicit TArgData(const IArgData& src)
	{
		m_nType = src.GetType();
		
		switch (m_nType)
		{
		case DT_BOOLEAN:
			m_bDataBoolean = src.GetBoolean();
			break;
		case DT_INT32:
			m_nDataInt32 = src.GetInt32();
			break;
		case DT_INT64:
			m_nDataInt64 = src.GetInt64();
			break;
		case DT_FLOAT:
			m_fDataFloat = src.GetFloat();
			break;
		case DT_DOUBLE:
			m_dDataDouble = src.GetDouble();
			break;
		case DT_STRING:
			this->InnerSetString(src.GetString());
			break;
		case DT_UNIQID:
			m_nDataUniqid = src.GetUniqid().nValue64;
			break;
		case DT_POINTER:
			m_pDataPointer = src.GetPointer();
			break;
		default:
			break;
		}
	}
	
	TArgData(int type, bool value)
	{
		Assert(type == DT_BOOLEAN);

		m_nType = DT_BOOLEAN;
		m_bDataBoolean = value;
	}

	TArgData(int type, int value)
	{
		Assert(type == DT_INT32);
		
		m_nType = DT_INT32;
		m_nDataInt32 = value;
	}

	TArgData(int type, int64_t value)
	{
		Assert(type == DT_INT64);

		m_nType = DT_INT64;
		m_nDataInt64 = value;
	}
	
	TArgData(int type, float value)
	{
		Assert(type == DT_FLOAT);

		m_nType = DT_FLOAT;
		m_fDataFloat = value;
	}

	TArgData(int type, double value)
	{
		Assert(type == DT_DOUBLE);

		m_nType = DT_DOUBLE;
		m_dDataDouble = value;
	}

	TArgData(int type, const char* value)
	{
		Assert(type == DT_STRING);

		m_nType = DT_STRING;
		this->InnerSetString(value);
	}

	TArgData(int type, const uniqid_t& value)
	{
		Assert(type == DT_UNIQID);

		m_nType = DT_UNIQID;
		m_nDataUniqid = value.nValue64;
	}

	TArgData(int type, void* value)
	{
		Assert(type == DT_POINTER);

		m_nType = DT_POINTER;
		m_pDataPointer = value;
	}

	virtual ~TArgData()
	{
		this->ReleaseAll();
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);

		this->Swap(temp);
		return *this;
	}
	
	self_type& operator=(const IArgData& src)
	{
		self_type temp(src);

		this->Swap(temp);
		return *this;
	}
	
	void Swap(self_type& src)
	{
		int temp_type = m_nType;
		int64_t temp_value = m_nDataInt64;
		unsigned int temp_alloc_length = m_nAllocLength;
		char temp_buffer[BUFFER_SIZE];
		bool dst_use_buffer = (m_nType == DT_STRING) &&
			(m_pDataString == m_Buffer);

		if (dst_use_buffer)
		{
			memcpy(temp_buffer, m_Buffer, BUFFER_SIZE);
		}
		
		bool src_use_buffer = (src.m_nType == DT_STRING) &&
			(src.m_pDataString == src.m_Buffer);
		
		if (src_use_buffer)
		{
			memcpy(m_Buffer, src.m_Buffer, BUFFER_SIZE);
			m_pDataString = m_Buffer;
		} 
		else
		{
			
			m_nDataInt64 = src.m_nDataInt64;
			m_nAllocLength = src.m_nAllocLength;
		}

		m_nType = src.m_nType;

		if (dst_use_buffer)
		{
			memcpy(src.m_Buffer, temp_buffer, BUFFER_SIZE);
			src.m_pDataString = src.m_Buffer;
		}
		else
		{
			src.m_nDataInt64 = temp_value;
			src.m_nAllocLength = temp_alloc_length;
		}

		src.m_nType = temp_type;
	}

	// Get type of data.
	virtual int GetType() const
	{
		return m_nType;
	}

	// Get value.
	virtual bool GetBoolean() const
	{
		switch (m_nType)
		{
		case DT_BOOLEAN:
			return m_bDataBoolean;
		case DT_INT32:
			return m_nDataInt32 != 0;
		case DT_INT64:
			return m_nDataInt64 != 0;
		default:
			break;
		}
		
		return false;
	}

	virtual int GetInt32() const
	{
		switch (m_nType)
		{
		case DT_BOOLEAN:
			return (int)m_bDataBoolean;
		case DT_INT32:
			return m_nDataInt32;
		case DT_INT64:
			return (int)m_nDataInt64;
		case DT_FLOAT:
			return (int)m_fDataFloat;
		case DT_DOUBLE:
			return (int)m_dDataDouble;
		default:
			break;
		}

		return 0;
	}

	virtual int64_t GetInt64() const
	{
		switch (m_nType)
		{
		case DT_BOOLEAN:
			return (int64_t)m_bDataBoolean;
		case DT_INT32:
			return (int64_t)m_nDataInt32;
		case DT_INT64:
			return m_nDataInt64;
		case DT_FLOAT:
			return (int64_t)m_fDataFloat;
		case DT_DOUBLE:
			return (int64_t)m_dDataDouble;
		default:
			break;
		}
		
		return 0;
	}

	virtual float GetFloat() const
	{
		switch (m_nType)
		{
		case DT_BOOLEAN:
			return (float)m_bDataBoolean;
		case DT_INT32:
			return (float)m_nDataInt32;
		case DT_INT64:
			return (float)m_nDataInt64;
		case DT_FLOAT:
			return m_fDataFloat;
		case DT_DOUBLE:
			return (float)m_dDataDouble;
		default:
			break;
		}
		
		return 0.0F;
	}

	virtual double GetDouble() const
	{
		switch (m_nType)
		{
		case DT_BOOLEAN:
			return (double)m_bDataBoolean;
		case DT_INT32:
			return (double)m_nDataInt32;
		case DT_INT64:
			return (double)m_nDataInt64;
		case DT_FLOAT:
			return (double)m_fDataFloat;
		case DT_DOUBLE:
			return m_dDataDouble;
		default:
			break;
		}
		
		return 0.0;
	}

	virtual const char* GetString() const
	{
		if (m_nType != DT_STRING)
		{
			return "";
		}
		
		return m_pDataString;
	}

	virtual uniqid_t GetUniqid() const
	{
		if (m_nType != DT_UNIQID)
		{
			return uniqid_t();
		}
		
		return uniqid_t(m_nDataUniqid);
	}

	virtual void* GetPointer() const
	{
		if (m_nType != DT_POINTER)
		{
			return NULL;
		}

		return m_pDataPointer;
	}

	// Set value.
	virtual void SetNull()
	{
		this->ReleaseAll();
		m_nType = DT_NULL;
		m_nDataInt64 = 0;
	}
	
	virtual void SetBoolean(bool value)
	{
		this->ReleaseAll();
		m_nType = DT_BOOLEAN;
		m_bDataBoolean = value;
	}

	virtual void SetInt32(int value)
	{
		this->ReleaseAll();
		m_nType = DT_INT32;
		m_nDataInt32 = value;
	}
	
	virtual void SetInt64(int64_t value)
	{
		ReleaseAll();
		m_nType = DT_INT64;
		m_nDataInt64 = value;
	}
	
	virtual void SetFloat(float value)
	{
		this->ReleaseAll();
		m_nType = DT_FLOAT;
		m_fDataFloat = value;
	}
	
	virtual void SetDouble(double value)
	{
		this->ReleaseAll();
		m_nType = DT_DOUBLE;
		m_dDataDouble = value;
	}
	
	virtual void SetString(const char* value)
	{
		this->ReleaseAll();
		m_nType = DT_STRING;
		this->InnerSetString(value);
	}
	
	virtual void SetUniqid(const uniqid_t& value)
	{
		this->ReleaseAll();
		m_nType = DT_UNIQID;
		m_nDataUniqid = value.nValue64;
	}

	virtual void SetPointer(void* value)
	{
		this->ReleaseAll();
		m_nType = DT_POINTER;
		m_pDataPointer = value;
	}

	virtual void Assign(const IArgData& src)
	{
		self_type temp(src);

		this->Swap(temp);
	}

private:
	void ReleaseAll()
	{
		switch (m_nType)
		{
		case DT_STRING:
			if (m_pDataString != m_Buffer)
			{
				m_Alloc.Free(m_pDataString);
				m_pDataString = NULL;
			}
			break;
		default:
			break;
		}
	}

	void InnerSetString(const char* value)
	{
		const size_t value_size = strlen(value) + 1;
		char* p;
		
		if (value_size > BUFFER_SIZE) 
		{
			p = (char*)m_Alloc.Alloc(value_size);
			m_nAllocLength = (unsigned int)value_size;
		}
		else
		{
			p = m_Buffer;
		}
		
		memcpy(p, value, value_size);
		m_pDataString = p;
	}

private:
	ALLOC m_Alloc;
	union
	{
		bool m_bDataBoolean;
		int m_nDataInt32;
		int64_t m_nDataInt64;
		float m_fDataFloat;
		double m_dDataDouble;
		char* m_pDataString;
		uint64_t m_nDataUniqid;
		void* m_pDataPointer;
	};
	int m_nType;
	union 
	{
		unsigned int m_nAllocLength;
		char m_Buffer[BUFFER_SIZE];
	};
};

#endif // __ARG_DATA_H
