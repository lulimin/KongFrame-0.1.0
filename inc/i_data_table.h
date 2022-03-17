// i_data_table.h
// Created by lulimin on 2019/2/14.

#ifndef __I_DATA_TABLE_H
#define __I_DATA_TABLE_H

#include "common.h"

// Named parameter table interface.

class IArgData;
class IArgList;

class IDataTable
{
public:
	virtual ~IDataTable() {}

	// Add key and value.
	virtual IArgData* Add(const char* key, const IArgData& value) = 0;
	// Remove data.
	virtual bool Remove(const char* key) = 0;
	// Clear all.
	virtual void Clear() = 0;
	// Set value(add if not exists).
	virtual void Assign(const char* key, const IArgData& value) = 0;
	// Test key exists.
	virtual bool Exists(const char* key) = 0;
	// Get value.
	virtual IArgData* GetValue(const char* key) = 0;
	// Get number of keys.
	virtual size_t GetCount() = 0;
	// Get key list.
	virtual void GetKeyList(IArgList* result) = 0;
};

#endif // __I_DATA_TABLE_H
