// i_arg_list.h
// Created by lulimin on 2019/2/14.

#ifndef __I_ARG_LIST_H
#define __I_ARG_LIST_H

#include "common.h"
#include "uniqid.h"
#include "data_type.h"
#include "arg_string.h"

// Argument data list interface.

class IArgList
{
public:
	virtual ~IArgList() {}
	
	// Get number of data.
	virtual size_t GetCount() const = 0; 
	// Get type of data.
	virtual int GetType(size_t index) const = 0;
	// Get value.
	virtual bool GetBoolean(size_t index) const = 0;
	virtual int GetInt32(size_t index) const = 0;
	virtual int64_t GetInt64(size_t index) const = 0;
	virtual float GetFloat(size_t index) const = 0;
	virtual double GetDouble(size_t index) const = 0;
	virtual const char* GetString(size_t index) const = 0;
	virtual uniqid_t GetUniqid(size_t index) const = 0;
	virtual void* GetPointer(size_t index) const = 0;

	// Add value to list.
	virtual bool AddBoolean(bool value) = 0;
	virtual bool AddInt32(int value) = 0;
	virtual bool AddInt64(int64_t value) = 0;
	virtual bool AddFloat(float value) = 0;
	virtual bool AddDouble(double value) = 0;
	virtual bool AddString(const char* value) = 0;
	virtual bool AddUniqid(const uniqid_t& value) = 0;
	virtual bool AddPointer(void* value) = 0;
	// Clear all values.
	virtual void Clear() = 0;
	// Merge tow lists.
	virtual bool Concat(const IArgList& src) = 0;
	// Add to tail of list.
	virtual bool Append(const IArgList& src, size_t start, size_t count) = 0;

	inline IArgList& operator<<(bool value)
	{ 
		this->AddBoolean(value); 
		return *this; 
	}

	inline IArgList& operator<<(char value)
	{ 
		this->AddInt32(value);
		return *this; 
	}

	inline IArgList& operator<<(unsigned char value)
	{ 
		this->AddInt32(value);
		return *this; 
	}

	inline IArgList& operator<<(short value)
	{ 
		this->AddInt32(value);
		return *this; 
	}

	inline IArgList& operator<<(unsigned short value)
	{ 
		this->AddInt32(value);
		return *this; 
	}

	inline IArgList& operator<<(int value)
	{ 
		this->AddInt32(value);
		return *this; 
	}
	
	inline IArgList& operator<<(unsigned int value)
	{ 
		this->AddInt32(value);
		return *this; 
	}

	inline IArgList& operator<<(long value)
	{ 
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
		this->AddInt32(value);
#else
		this->AddInt64(value);
#endif
		return *this; 
	}

	inline IArgList& operator<<(unsigned long value)
	{ 
#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
		this->AddInt32(value);
#else
		this->AddInt64(value);
#endif
		return *this; 
	}

#if defined(K_PLATFORM_WINDOWS) || defined(K_PLATFORM_32BIT)
	inline IArgList& operator<<(int64_t value) 
	{ 
		this->AddInt64(value);
		return *this; 
	}

	inline IArgList& operator<<(uint64_t value) 
	{ 
		this->AddInt64(value);
		return *this; 
	}
#endif

	inline IArgList& operator<<(float value) 
	{ 
		this->AddFloat(value);
		return *this; 
	}

	inline IArgList& operator<<(double value) 
	{ 
		this->AddDouble(value);
		return *this; 
	}

	inline IArgList& operator<<(const char* value) 
	{ 
		this->AddString(value);
		return *this; 
	}

	inline IArgList& operator<<(const ArgString& value)
	{
		this->AddString(value.CString());
		return *this;
	}

	inline IArgList& operator<<(const uniqid_t& value)
	{ 
		this->AddUniqid(value);
		return *this; 
	}

	inline IArgList& operator<<(const IArgList& value) 
	{ 
		this->Concat(value);
		return *this; 
	}
};

#endif // __I_ARG_LIST_H
