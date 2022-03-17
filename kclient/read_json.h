// read_json.h
// Created by lulimin on 2020/2/18.

#ifndef __READ_JSON_H
#define __READ_JSON_H

#define SAJSON_NO_STD_STRING
#include "../ext/sajson/sajson.h"

// Read configuration string.
inline const char* read_json_string(const sajson::value& node, const char* key)
{
	//Assert(key != NULL);

	sajson::string sakey(key, strlen(key));

	if (node.find_object_key(sakey) == node.get_length())
	{
		return "";
	}

	const sajson::value& val = node.get_value_of_key(sakey);

	if (val.get_type() != sajson::TYPE_STRING)
	{
		return "";
	}
	
	return val.as_cstring();
}

// Read configuration boolean.
inline bool read_json_boolean(const sajson::value& node, const char* key)
{
	//Assert(key != NULL);
	
	sajson::string sakey(key, strlen(key));

	if (node.find_object_key(sakey) == node.get_length())
	{
		return false;
	}

	const sajson::value& val = node.get_value_of_key(sakey);

	if ((val.get_type() != sajson::TYPE_FALSE) &&
		(val.get_type() != sajson::TYPE_TRUE))
	{
		return false;
	}

	return val.get_boolean_value();
}

// Read configuration integer.
static inline int read_json_int32(const sajson::value& node, const char* key)
{
	//Assert(key != NULL);

	sajson::string sakey(key, strlen(key));

	if (node.find_object_key(sakey) == node.get_length())
	{
		return 0;
	}

	const sajson::value& val = node.get_value_of_key(sakey);

	if (val.get_type() != sajson::TYPE_INTEGER)
	{
		return 0;
	}

	return val.get_integer_value();
}

#endif // __READ_JSON_H
