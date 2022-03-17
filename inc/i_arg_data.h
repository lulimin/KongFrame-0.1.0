// i_arg_data.h
// Created by lulimin on 2019/2/14.

#ifndef __I_ARG_DATA_H
#define __I_ARG_DATA_H

#include "common.h"
#include "uniqid.h"
#include "data_type.h"
#include <string.h>

// Argument data interface.

class IArgData
{
public:
	virtual ~IArgData() {}

	// Get type of data.
	virtual int GetType() const = 0;
	// Get value.
	virtual bool GetBoolean() const = 0;
	virtual int GetInt32() const = 0;
	virtual int64_t GetInt64() const = 0;
	virtual float GetFloat() const = 0;
	virtual double GetDouble() const = 0;
	virtual const char* GetString() const = 0;
	virtual uniqid_t GetUniqid() const = 0;
	virtual void* GetPointer() const = 0;
	// Set value.
	virtual void SetNull() = 0;
	virtual void SetBoolean(bool value) = 0;
	virtual void SetInt32(int value) = 0;
	virtual void SetInt64(int64_t value) = 0;
	virtual void SetFloat(float value) = 0;
	virtual void SetDouble(double value) = 0;
	virtual void SetString(const char* value) = 0;
	virtual void SetUniqid(const uniqid_t& value) = 0;
	virtual void SetPointer(void* value) = 0;
	// Copy data.
	virtual void Assign(const IArgData& src) = 0;
	
private:
	// Avoid directly copy.
	IArgData& operator=(const IArgData&);
};

#endif // __I_ARG_DATA_H
