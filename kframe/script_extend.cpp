// script_extend.cpp
// Created by lulimin on 2019/2/18.

#include "script_extend.h"
#include "script_routine.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "frame_factory.h"
#include "frame_object_desc.h"
#include "../inc/arg_data.h"
#include "../inc/arg_list.h"
#include "../inc/i_object.h"
#include "../inc/i_object_desc.h"
#include "../inc/i_object_hook.h"
#include "../inc/i_data_table.h"
#include "../inc/i_file_access.h"
#include "../inc/utility.h"
#include "../inc/auto_buffer.h"
#include "../inc/platform.h"

/// \file script_extend.cpp
/// \brief Lua script extension functions.

// Get script coroutine.
static inline scp_routine_t* scp_get_routine(lua_State* state)
{
	return (scp_routine_t*)scp_util_get_context(state);
}

// Get object in stack.
static inline IObject* scp_get_object(lua_State* state, int index)
{
	uniqid_t id;

	if (!scp_util_read_uniqid(state, index, &id))
	{
		return NULL;
	}

	return frame_get_object(id);
}

// Throw script error.
static inline int scp_error(lua_State* state, const char* err)
{
	return scp_util_error_handler(state, err);
}

#define VERIFY_ARGUMENTS(state, name, num) if (lua_gettop(state) != num) \
	{ return scp_error(state, "("#name")Arguments not "#num"."); }

typedef TArgData<64, FrameInnerAlloc> InnerArgData;
typedef TArgList<8, 128, FrameInnerAlloc> InnerArgList;

/// bool k_boolean(var value)
/// \brief Convert to boolean value.
/// \param value Source data.
static int k_boolean(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_boolean, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		lua_pushboolean(state, false);
		return 1;
	case LUA_TBOOLEAN:
		lua_pushboolean(state, lua_toboolean(state, 1));
		return 1;
	case LUA_TNUMBER:
		lua_pushboolean(state, lua_tonumber(state, 1) != 0);
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		lua_pushboolean(state, stricmp(s, "true") == 0);
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		bool value;

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
			value = pData->GetBoolean();
			break;
		case DT_INT32:
			value = pData->GetInt32() != 0;
			break;
		case DT_INT64:
			value = pData->GetInt64() != 0;
			break;
		case DT_FLOAT:
			value = pData->GetFloat() != 0;
			break;
		case DT_DOUBLE:
			value = pData->GetDouble() != 0;
			break;
		case DT_STRING:
			value = stricmp(pData->GetString(), "true") == 0;
			break;
		default:
			value = false;
			break;
		}

		lua_pushboolean(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_boolean)Argument type error.");
}

/// int32 k_int32(var value)
/// \brief Convert to 32-bit integer value.
/// \param value Source data.
static int k_int32(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_int32, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		scp_util_push_int32(state, 0);
		return 1;
	case LUA_TBOOLEAN:
		scp_util_push_int32(state, lua_toboolean(state, 1));
		return 1;
	case LUA_TNUMBER:
		scp_util_push_int32(state, (int)lua_tonumber(state, 1));
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		scp_util_push_int32(state, (int)atof(s));
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		int value;

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
			value = pData->GetBoolean();
			break;
		case DT_INT32:
			value = pData->GetInt32();
			break;
		case DT_INT64:
			value = (int)pData->GetInt64();
			break;
		case DT_FLOAT:
			value = (int)pData->GetFloat();
			break;
		case DT_DOUBLE:
			value = (int)pData->GetDouble();
			break;
		case DT_STRING:
			value = atoi(pData->GetString());
			break;
		default:
			value = 0;
			break;
		}

		scp_util_push_int32(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_int32)Argument type error.");
}

/// int64 k_int64(var value)
/// \brief Convert to 64-bit integer value.
/// \param value Source data.
static int k_int64(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_int64, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		scp_util_push_int64(state, 0);
		return 1;
	case LUA_TBOOLEAN:
		scp_util_push_int64(state, lua_toboolean(state, 1));
		return 1;
	case LUA_TNUMBER:
		scp_util_push_int64(state, (int64_t)lua_tonumber(state, 1));
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		scp_util_push_int64(state, plat_string_to_int64(s));
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		int64_t value;

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
			value = pData->GetBoolean();
			break;
		case DT_INT32:
			value = pData->GetInt32();
			break;
		case DT_INT64:
			value = pData->GetInt64();
			break;
		case DT_FLOAT:
			value = (int64_t)pData->GetFloat();
			break;
		case DT_DOUBLE:
			value = (int64_t)pData->GetDouble();
			break;
		case DT_STRING:
			value = plat_string_to_int64(pData->GetString());
			break;
		default:
			value = 0;
			break;
		}

		scp_util_push_int64(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_int64)Argument type error.");
}

/// float k_float(var value)
/// \brief Convert to float value.
/// \param value Source data.
static int k_float(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_float, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		scp_util_push_float(state, 0.0F);
		return 1;
	case LUA_TBOOLEAN:
		scp_util_push_float(state, (float)lua_toboolean(state, 1));
		return 1;
	case LUA_TNUMBER:
		scp_util_push_float(state, (float)lua_tonumber(state, 1));
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		scp_util_push_float(state, (float)atof(s));
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		float value;

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
			value = (float)pData->GetBoolean();
			break;
		case DT_INT32:
			value = (float)pData->GetInt32();
			break;
		case DT_INT64:
			value = (float)pData->GetInt64();
			break;
		case DT_FLOAT:
			value = pData->GetFloat();
			break;
		case DT_DOUBLE:
			value = (float)pData->GetDouble();
			break;
		case DT_STRING:
			value = (float)atof(pData->GetString());
			break;
		default:
			value = 0.0F;
			break;
		}

		scp_util_push_float(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_float)Argument type error.");
}

/// double k_number(var value)
/// \brief Convert to lua number(double type).
/// \param value Source data.
static int k_number(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_number, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		lua_pushnumber(state, 0);
		return 1;
	case LUA_TBOOLEAN:
		lua_pushnumber(state, lua_toboolean(state, 1));
		return 1;
	case LUA_TNUMBER:
		lua_pushnumber(state, lua_tonumber(state, 1));
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		lua_pushnumber(state, atof(s));
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		double value;

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
			value = pData->GetBoolean();
			break;
		case DT_INT32:
			value = pData->GetInt32();
			break;
		case DT_INT64:
			value = (double)pData->GetInt64();
			break;
		case DT_FLOAT:
			value = pData->GetFloat();
			break;
		case DT_DOUBLE:
			value = pData->GetDouble();
			break;
		case DT_STRING:
			value = atof(pData->GetString());
			break;
		default:
			value = 0.0;
			break;
		}

		lua_pushnumber(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_number)Argument type error.");
}

// Convert number to string.
static inline void number_to_string(char* buf, size_t size, double value)
{
	if (fabs(value - (int64_t)value) < DBL_EPSILON)
	{
		plat_int64_to_string(buf, size, (int64_t)value);
	}
	else
	{
		plat_safe_sprintf(buf, size, "%f", value);

		char* p = buf + strlen(buf) - 1;

		while (p > buf)
		{
			if (*p == '0')
			{
				*p = 0;
			}
			else
			{
				break;
			}

			--p;
		}
	}
}

/// string k_string(var value)
/// \brief Convert to string value.
/// \param value Source data.
static int k_string(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_string, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		lua_pushstring(state, "nil");
		return 1;
	case LUA_TBOOLEAN:
	{
		if (lua_toboolean(state, 1))
		{
			lua_pushstring(state, "true");
		}
		else
		{
			lua_pushstring(state, "false");
		}

		return 1;
	}
	case LUA_TNUMBER:
	{
		char buf[64];

		number_to_string(buf, sizeof(buf), lua_tonumber(state, 1));
		lua_pushstring(state, buf);
		return 1;
	}
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);

		lua_pushstring(state, s);
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		char buf[64];

		switch (pData->GetType())
		{
		case DT_BOOLEAN:
		{
			if (pData->GetBoolean())
			{
				lua_pushstring(state, "true");
			}
			else
			{
				lua_pushstring(state, "false");
			}

			break;
		}
		case DT_INT32:
			plat_safe_sprintf(buf, sizeof(buf), "%d", pData->GetInt32());
			lua_pushstring(state, buf);
			break;
		case DT_INT64:
			plat_int64_to_string(buf, sizeof(buf), pData->GetInt64());
			lua_pushstring(state, buf);
			break;
		case DT_FLOAT:
			number_to_string(buf, sizeof(buf), pData->GetFloat());
			lua_pushstring(state, buf);
			break;
		case DT_DOUBLE:
			number_to_string(buf, sizeof(buf), pData->GetDouble());
			lua_pushstring(state, buf);
			break;
		case DT_STRING:
			lua_pushstring(state, pData->GetString());
			break;
		case DT_UNIQID:
			util_uniqid_to_string(buf, sizeof(buf), pData->GetUniqid());
			lua_pushstring(state, buf);
			break;
		default:
			lua_pushstring(state, "");
			break;
		}

		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_string)Argument type error.");
}

/// object k_uniqid(var value)
/// \brief Convert to object id.
/// \param value Source data.
static int k_uniqid(lua_State* state)
{
	//VERIFY_ARGUMENTS(state, k_uniqid, 1);
	int arg_num = lua_gettop(state);

	if (0 == arg_num)
	{
		// Return empty object id while no argument.
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	case LUA_TBOOLEAN:
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	case LUA_TSTRING:
	{
		const char* s = lua_tostring(state, 1);
		
		scp_util_push_uniqid(state, util_string_to_uniqid(s));
		return 1;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);
		uniqid_t value;

		switch (pData->GetType())
		{
		case DT_STRING:
		{
			const char* s = pData->GetString();

			value = util_string_to_uniqid(s);
			break;
		}
		case DT_UNIQID:
			value = pData->GetUniqid();
			break;
		default:
			break;
		}

		scp_util_push_uniqid(state, value);
		return 1;
	}
	default:
		break;
	}

	return scp_error(state, "(k_uniqid)Argument type error.");
}

/// string k_type(var value)
/// \brief Get data type name.
/// \param value Source data.
static int k_type(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_type, 1);

	switch (lua_type(state, 1))
	{
	case LUA_TNIL:
		lua_pushstring(state, "nil");
		break;
	case LUA_TBOOLEAN:
		lua_pushstring(state, "boolean");
		break;
	case LUA_TNUMBER:
		lua_pushstring(state, "number");
		break;
	case LUA_TSTRING:
		lua_pushstring(state, "string");
		break;
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, 1);

		lua_pushstring(state, util_get_type_name(pData->GetType()));
		break;
	}
	default:
		lua_pushstring(state, "unknown");
		break;
	}

	return 1;
}

/// bool k_find_class(string name)
/// \brief Search object class.
/// \param name Object class name.
static int k_find_class(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_class, 1);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_find_class)Need name of class.");
	}

	lua_pushboolean(state, frame_get_object_desc(name) != NULL);
	return 1;
}

/// object k_create(string name, [args])
/// \brief Create object.
/// \param name Object class name.
/// \param args Object initialize arguments[optional].
/// \return Empty id if create failed.
static int k_create(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 1)
	{
		return scp_error(state, "(k_create)Need at least one argument.");
	}

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_create)Need object class name.");
	}

	InnerArgList args;

	if (arg_num >= 2)
	{
		if (!scp_util_read_arg_list(state, 2, arg_num, &args))
		{
			return scp_error(state, "(k_create)Get arguments error.");
		}
	}

	// Object can be deleted in script.
	IObject* pObj = frame_script_create_object(name, args);

	if (NULL == pObj)
	{
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	scp_util_push_uniqid(state, pObj->GetUID());
	return 1;
}

/// bool k_delete(object id)
/// \brief Delete object(only created by k_create).
/// \param id Object id.
static int k_delete(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_delete, 1);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_delete)Need object.");
	}

	if (!frame_factory_create_in_script(pObj->GetUID()))
	{
		return scp_error(state, "(k_delete)Cannot delete by script.");
	}

	pObj->Release();
	lua_pushboolean(state, true);
	return 1;
}

/// bool k_bind(object id, string script, [string init_func, [args]])
/// \brief Object bind script.
/// \param id Object id.
/// \param script Script file name.
/// \param init_func Initialize function[optional].
/// \param args Arguments of initialize function[optional].
static int k_bind(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, "(k_bind)Need at least two arguments.");
	}

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_bind)Need object.");
	}

	const char* script = scp_util_read_string(state, 2);

	if (NULL == script)
	{
		return scp_error(state, "(k_bind)Need script name.");
	}

	// Execute initialize function.
	if (arg_num >= 3)
	{
		const char* func = scp_util_read_string(state, 3);

		if (NULL == func)
		{
			return scp_error(state, "(k_bind)Need initialize function name.");
		}

		uniqid_t obj_id = pObj->GetUID();
		InnerArgList args;

		args.AddUniqid(obj_id);

		if (scp_util_read_arg_list(state, 4, arg_num, &args))
		{
			if (!frame_bind_script(pObj, script))
			{
				lua_pushboolean(state, false);
				return 1;
			}

			frame_run_routine(script, func, obj_id, args, NULL);
		}
	}
	else
	{
		if (!frame_bind_script(pObj, script))
		{
			lua_pushboolean(state, false);
			return 1;
		}
	}

	lua_pushboolean(state, true);
	return 1;
}

/// bool k_event(object id, string event, string func)
/// \brief Set event process function, object must bind script first.
/// \param id Object id.
/// \param event Event name.
/// \param func Callback function.
static int k_event(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if ((arg_num != 3) && (arg_num != 4))
	{
		return scp_error(state, "(k_event)Nubmer of arguments error.");
	}

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_event)Need object.");
	}

	const char* event = scp_util_read_string(state, 2);

	if (NULL == event)
	{
		return scp_error(state, "(k_event)Need event name.");
	}

	const char* file = "";
	const char* func = "";

	if (3 == arg_num)
	{
		func = scp_util_read_string(state, 3);

		if (NULL == func)
		{
			return scp_error(state, "(k_event)Need function name.");
		}
	}
	else
	{
		file = scp_util_read_string(state, 3);

		if (NULL == file)
		{
			return scp_error(state, "(k_event)Need script file name.");
		}

		func = scp_util_read_string(state, 4);

		if (NULL == func)
		{
			return scp_error(state, "(k_event)Need function name.");
		}
	}

	IObjectHook* pHook = pObj->GetHook();

	if (NULL == pHook)
	{
		return scp_error(state, "(k_event)Object not bind script.");
	}

	if (util_string_empty(func))
	{
		lua_pushboolean(state, pHook->RemoveEventProcessor(event));
	}
	else
	{
		lua_pushboolean(state, pHook->AddEventProcessor(event, file, func));
	}

	return 1;
}

/// string k_class_name(object id)
/// \brief Get object class name.
/// \param id Object id.
static int k_class_name(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_class_name, 1);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_class_name)Need object.");
	}

	lua_pushstring(state, pObj->GetDesc()->GetObjectName());
	return 1;
}

/// object k_main_object()
/// \brief Get main object.
static int k_main_object(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_main_object, 0);

	IObject* pObj = frame_get_main_object();

	if (NULL == pObj)
	{
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	scp_util_push_uniqid(state, pObj->GetUID());
	return 1;
}

/// object k_find_object(string name)
/// \brief Search object by class name.
/// \param name Object class name.
static int k_find_object(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_object, 1);

	const char* class_name = scp_util_read_string(state, 1);

	if (NULL == class_name)
	{
		return scp_error(state, "(k_find_object)Need object class name.");
	}

	IObject* pObj = frame_find_object(class_name);

	if (NULL == pObj)
	{
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	scp_util_push_uniqid(state, pObj->GetUID());
	return 1;
}

/// bool k_exists(object id)
/// \brief Test object id validate.
/// \param id Object id.
static int k_exists(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_exists, 1);

	uniqid_t id;

	if (!scp_util_read_uniqid(state, 1, &id))
	{
		lua_pushboolean(state, false);
		return 1;
	}

	lua_pushboolean(state, frame_get_object(id) != NULL);
	return 1;
}

/// bool k_derive(object id, string class_name)
/// \brief Test derive from object class.
/// \param id Object id.
/// \param class_name Object class name.
static int k_derive(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_derive, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_derive)Need object.");
	}

	const char* class_name = scp_util_read_string(state, 2);

	if (NULL == class_name)
	{
		return scp_error(state, "(k_derive)Need class name.");
	}

	lua_pushboolean(state, pObj->GetDesc()->DeriveFrom(class_name));
	return 1;
}

/// bool k_find_property(object id, string property_name)
/// \brief Test object internal property exists.
/// \param id Object id.
/// \param property_name Internal property name.
static int k_find_property(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_property, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_find_property)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_find_property)Need property name.");
	}

	IPropertyDesc* pPropDesc = pObj->GetDesc()->FindPropertyDesc(name);

	lua_pushboolean(state, pPropDesc != NULL);
	return 1;
}

/// bool k_find_extra(object id, string name)
/// \brief Test object extra property name exists.
/// \param id Object id.
/// \param name Extra property name.
static int k_find_extra(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_extra, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_find_extra)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_find_extra)Need extra name.");
	}

	IDataTable* pExtra = pObj->GetExtra();

	if (NULL == pExtra)
	{
		lua_pushboolean(state, false);
		return 1;
	}

	lua_pushboolean(state, pExtra->Exists(name));
	return 1;
}

/// bool k_find_method(object id, string method_name)
/// \brief Test object method exists.
/// \param id Object id.
/// \param method_name Method name.
static int k_find_method(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_method, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_find_method)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_find_method)Need method name.");
	}

	IMethodDesc* pMethodDesc = pObj->GetDesc()->FindMethodDesc(name);

	lua_pushboolean(state, pMethodDesc != NULL);
	return 1;
}

/// table k_property_list(object id)
/// \brief Get object internal property list.
/// \param id Object id.
static int k_property_list(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_property_list, 1);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_property_list)Need object.");
	}

	InnerArgList prop_names;

	pObj->GetDesc()->GetPropertyAll(&prop_names);
	scp_util_push_table(state, prop_names);
	return 1;
}

/// table k_extra_list(object id)
/// \brief Get object extra list.
/// \param id Object id.
static int k_extra_list(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_extra_list, 1);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_extra_list)Need object.");
	}

	InnerArgList extra_names;
	IDataTable* pExtra = pObj->GetExtra();

	if (pExtra)
	{
		pExtra->GetKeyList(&extra_names);
	}

	scp_util_push_table(state, extra_names);
	return 1;
}

/// table k_method_list(object id)
/// \brief Get object method list.
/// \param id Object id.
static int k_method_list(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_method_list, 1);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_method_list)Need object.");
	}

	InnerArgList method_names;

	pObj->GetDesc()->GetMethodAll(&method_names);
	scp_util_push_table(state, method_names);
	return 1;
}

/// var k_property(object id, string property_name)
/// \brief Get value of object internal property.
/// \param id Object id.
/// \param property_name Internal property name.
static int k_property(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_property, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_property)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_property)Need property name.");
	}

	InnerArgData value;

	if (!frame_get_property(pObj, name, &value))
	{
		return 0;
	}

	scp_util_push_arg_data(state, value);
	return 1;
}

/// var k_extra(object id, string name)
/// \brief Get value of object extra property.
/// \param id Object id.
/// \param name Extra property name.
static int k_extra(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_extra, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_extra)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_extra)Need extra name.");
	}

	IDataTable* pExtra = pObj->GetExtra();

	if (NULL == pExtra)
	{
		return 0;
	}

	const IArgData* pValue = pExtra->GetValue(name);

	if (NULL == pValue)
	{
		return 0;
	}

	scp_util_push_arg_data(state, *pValue);
	return 1;
}

/// [res] k_method(object id, string method_name, [args])
/// \brief Run method of object.
/// \param id Object id.
/// \param method_name Method name of object.
/// \param args Arguments of method[optional].
static int k_method(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, "(k_method)Need at least two arguments.");
	}

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_method)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_method)Need method name.");
	}

	InnerArgList args;

	if (arg_num > 2)
	{
		if (!scp_util_read_arg_list(state, 3, arg_num, &args))
		{
			return scp_error(state, "(k_method)Get arguments error.");
		}
	}

	InnerArgList result;

	if (!frame_run_method(pObj, name, args, &result))
	{
		return 0;
	}

	FrameObjectDesc* pObjectDesc = (FrameObjectDesc*)pObj->GetDesc();
	FrameMethodDesc* pMethodDesc = pObjectDesc->GetMethodDesc(name);

	if (pMethodDesc->GetResultArray())
	{
		scp_util_push_table(state, result);
		return 1;
	}
	else
	{
		scp_util_push_arg_list(state, result);
		return (int)result.GetCount();
	}
}

/// bool k_set_property(object id, string property_name, var value)
/// \brief Set value of object internal property.
/// \param id Object id.
/// \param property_name Internal property name.
/// \param value Internal property value.
static int k_set_property(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_set_property, 3);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_set_property)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_set_property)Need property name.");
	}

	InnerArgData value;

	if (!scp_util_read_arg_data(state, 3, &value))
	{
		return scp_error(state, "(k_set_property)Need property value.");
	}

	lua_pushboolean(state, frame_set_property(pObj, name, value));
	return 1;
}

/// bool k_set_extra(object id, string name, var value)
/// \brief Set value of object extra property.
/// \param id Object id.
/// \param name Extra property name.
/// \param value Extra property value.
static int k_set_extra(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_set_extra, 3);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_set_extra)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_set_extra)Need extra name.");
	}

	InnerArgData value;

	if (!scp_util_read_arg_data(state, 3, &value))
	{
		return scp_error(state, "(k_set_extra)Need extra value.");
	}

	// Add extra property if not exists.
	IDataTable* pExtra = pObj->GetExtra();

	if (NULL == pExtra)
	{
		frame_create_extra(pObj);
		pExtra = pObj->GetExtra();
	}

	IArgData* pValue = pExtra->GetValue(name);

	if (NULL == pValue)
	{
		pValue = pExtra->Add(name, value);
	}
	else
	{
		pValue->Assign(value);
	}

	lua_pushboolean(state, true);
	return 1;
}

/// bool k_remove_extra(object id, string name)
/// \brief Remove object extra property.
/// \param id Object id.
/// \param name Extra property name.
static int k_remove_extra(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_remove_extra, 2);

	IObject* pObj = scp_get_object(state, 1);

	if (NULL == pObj)
	{
		return scp_error(state, "(k_remove_extra)Need object.");
	}

	const char* name = scp_util_read_string(state, 2);

	if (NULL == name)
	{
		return scp_error(state, "(k_remove_extra)Need extra name.");
	}

	IDataTable* pExtra = pObj->GetExtra();

	if (NULL == pExtra)
	{
		lua_pushboolean(state, false);
		return 1;
	}

	lua_pushboolean(state, pExtra->Remove(name));
	return 1;
}

/// bool k_find_script(string script, [string func])
/// \brief Test script file or script function exists.
/// \param script Script file name.
/// \param func Script function name.
static int k_find_script(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 1)
	{
		return scp_error(state, "(k_find_script)Need at least one arguments.");
	}
	
	const char* script = scp_util_read_string(state, 1);

	if (NULL == script)
	{
		return scp_error(state, "(k_find_script)Need script name.");
	}

	const char* func = "";

	if (arg_num > 1)
	{
		func = scp_util_read_string(state, 2);

		if (NULL == func)
		{
			return scp_error(state, "(k_find_script)Need function name.");
		}
	}
	
	lua_pushboolean(state, frame_find_script(script, func));
	return 1;
}

/// [res] k_run_script(string script, string func, [args])
/// \brief Call specified script function(cannot pause).
/// \param script Script file name.
/// \param func Script function name.
/// \param args Arguments, first argument is relate object id if type match.
/// \return Script executing result, may multiple values.
//static int k_run_script(lua_State* state)
//{
//	int arg_num = lua_gettop(state);
//
//	if (arg_num < 2)
//	{
//		return scp_error(state, "(k_run_script)Need at least two arguments.");
//	}
//
//	const char* script = scp_util_read_string(state, 1);
//
//	if (NULL == script)
//	{
//		return scp_error(state, "(k_run_script)Need script name.");
//	}
//	
//	const char* func = scp_util_read_string(state, 2);
//
//	if (NULL == func)
//	{
//		return scp_error(state, "(k_run_script)Need function name.");
//	}
//
//	InnerArgList args;
//
//	if (arg_num > 2)
//	{
//		if (!scp_util_read_arg_list(state, 3, arg_num, &args))
//		{
//			return scp_error(state, "(k_run_script)Get arguments error.");
//		}
//	}
//
//	InnerArgList res;
//
//	if (!frame_run_script(script, func, args, &res))
//	{
//		return scp_error(state, "(k_run_script)Run script failed.");
//	}
//	
//	return scp_util_push_arg_list(state, res);
//}

/// bool k_find_routine(string script, string func, [object id])
/// \brief Test script coroutine in running.
/// \param script Script file name.
/// \param func Script function name.
/// \param id Related object id[optional].
static int k_find_routine(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, 
			"(k_find_routine)Need at least three arguments.");
	}

	const char* script = scp_util_read_string(state, 1);

	if (NULL == script)
	{
		return scp_error(state, "(k_find_routine)Need script name.");
	}

	const char* func = scp_util_read_string(state, 2);

	if (NULL == func)
	{
		return scp_error(state, "(k_find_routine)Need function name.");
	}

	uniqid_t id;

	if (arg_num > 2)
	{
		if (!scp_util_read_uniqid(state, 3, &id))
		{
			return scp_error(state, "(k_find_routine)Need object id.");
		}
	}

	lua_pushboolean(state, frame_find_routine(script, func, id));
	return 1;
}

/// [res] k_run_routine(string script, string func, [args])
/// \brief Create script coroutine.
/// \param script Script file name.
/// \param func Script function name.
/// \param args Arguments, first argument is related object id if type match.
/// \return Coroutine executing result, may multiple values.
static int k_run_routine(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, "(k_run_routine)Need at least two arguments.");
	}

	const char* script = scp_util_read_string(state, 1);

	if (NULL == script)
	{
		return scp_error(state, "(k_run_routine)Need script name.");
	}
	
	const char* func = scp_util_read_string(state, 2);

	if (NULL == func)
	{
		return scp_error(state, "(k_run_routine)Need function name.");
	}

	uniqid_t id;
	InnerArgList args;

	if (arg_num > 2)
	{
		if (!scp_util_read_arg_list(state, 3, 3, &args))
		{
			return scp_error(state, "(k_run_routine)Get first argument error.");
		}

		if ((args.GetCount() > 0) && (args.GetType(0) == DT_UNIQID))
		{
			id = args.GetUniqid(0);
			args.Clear();
		}

		if (arg_num > 3)
		{
			if (!scp_util_read_arg_list(state, 4, arg_num, &args))
			{
				return scp_error(state, "(k_run_routine)Get arguments error.");
			}
		}
	}

	InnerArgList res;

	if (!frame_run_routine(script, func, id, args, &res))
	{
		return scp_error(state, "(k_run_routine)Run coroutine failed.");
	}
	
	return scp_util_push_arg_list(state, res);
}

/// bool k_kill_routine(string script, string func, [object id])
/// \brief Kill script coroutine.
/// \param script Script file name.
/// \param func Script function name.
/// \param id Related object id[optional].
static int k_kill_routine(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num < 2)
	{
		return scp_error(state, 
			"(k_kill_routine)Need at least two arguments.");
	}
	
	const char* script = scp_util_read_string(state, 1);
	
	if (NULL == script)
	{
		return scp_error(state, "(k_kill_routine)Need script name.");
	}
	
	const char* func = scp_util_read_string(state, 2);
	
	if (NULL == func)
	{
		return scp_error(state, "(k_kill_routine)Need function name.");
	}
	
	uniqid_t id;
	
	if (arg_num > 2) 
	{
		if (!scp_util_read_uniqid(state, 3, &id))
		{
			return scp_error(state, "(k_kill_routine)Need object id.");
		}
	}
	
	scp_routine_t* pRoutine = scp_get_routine(state);

	if (pRoutine)
	{
		if ((stricmp(scp_routine_get_script(pRoutine), script) == 0) &&
			(strcmp(scp_routine_get_function(pRoutine), func) == 0))
		{
			return scp_error(state, "(k_kill_routine)Cannot kill self.");
		}
	}

	lua_pushboolean(state, frame_kill_routine(script, func, id));
	return 1;
}

/// [res] k_wait_signal(double timeout, object id, string signal, [id, signal])
/// \brief Wait one or several signal(script executing paused).
/// \param timeout Seconds to waiting, negative means waiting forever.
/// \param id Related object id, wait global signal if it is empty id.
/// \param signal Signal name to waiting.
/// \param [id,signal] Optional other object id and signal name.
/// \return Signal arguments send by k_send_signal,
///		first argument is signal name if wait multiple signal.
static int k_wait_signal(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num < 3)
	{
		return scp_error(state, 
			"(k_wait_signal)Need at least three arguments.");
	}
	
	double timeout;

	if (!scp_util_read_number(state, 1, &timeout))
	{
		return scp_error(state, "(k_wait_signal)Need timeout value.");
	}
	
	scp_routine_t* pRoutine = scp_get_routine(state);

	if (NULL == pRoutine)
	{
		return scp_error(state, "(k_wait_signal)Must coroutine.");
	}

	scp_routine_clear_wait_signal(pRoutine);
	
	// Number of wait signals.
	int signal_num = (arg_num - 1) / 2;
	
	for (int i = 0; i < signal_num; ++i)
	{
		// Object id.
		uniqid_t id;

		if (!scp_util_read_uniqid(state, 2 + i * 2, &id))
		{
			scp_routine_clear_wait_signal(pRoutine);
			return scp_error(state, "(k_wait_signal)Need object id or null.");
		}

		// Signal name.
		const char* signal = scp_util_read_string(state, 3 + i * 2);

		if (NULL == signal)
		{
			scp_routine_clear_wait_signal(pRoutine);
			return scp_error(state, "(k_wait_signal)Need signal name.");
		}

		scp_routine_add_wait_signal(pRoutine, id, signal);
	}

	scp_routine_pause(pRoutine, timeout, false);
	return lua_yield(state, 0);
}

/// int k_send_signal(object id, string signal, [args])
/// \brief Send a signal,
///		if any coroutine wait this signal then it will continue execute.
/// \param id Related object id, global signal if it is empty id.
/// \param signal Signal name.
/// \param args Other signal arguments.
/// \return Number of triggered coroutines.
static int k_send_signal(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, "(k_send_signal)Need at least two arguments.");
	}
	
	uniqid_t id;
	
	if (!scp_util_read_uniqid(state, 1, &id))
	{
		return scp_error(state, "(k_send_signal)Need object id or null.");
	}
	
	const char* sig = scp_util_read_string(state, 2);
	
	if (NULL == sig)
	{
		return scp_error(state, "(k_send_signal)Need signal name.");
	}

	InnerArgList args;

	if (arg_num > 2)
	{
		if (!scp_util_read_arg_list(state, 3, arg_num, &args))
		{
			return scp_error(state, "(k_send_signal)Get arguments error.");
		}
	}
	
	IObject* pObj = frame_get_object(id);

	if (NULL == pObj)
	{
		id = uniqid_t();
	}

	size_t triggered = frame_send_routine_signal(id, sig, args);

	lua_pushnumber(state, (lua_Number)triggered);
	return 1;
}

/// double k_wait(double timeout)
/// \brief Coroutine paused(only in coroutine).
/// \param timeout Time out seconds, must positive or zero.
/// \return Actual pause seconds.
static int k_wait(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_wait, 1);

	double timeout;
	
	if (!scp_util_read_number(state, 1, &timeout))
	{
		return scp_error(state, "(k_wait)Need time out value.");
	}

	if (timeout < 0.0)
	{
		return scp_error(state, "(k_wait)Timeout value cannot negative.");
	}
	
	scp_routine_t* pRoutine = scp_get_routine(state);

	if (NULL == pRoutine)
	{
		return scp_error(state, "(k_wait)Must coroutine.");
	}

	scp_routine_pause(pRoutine, timeout, true);
	return lua_yield(state, 0);
}

/// double k_wait_elapsed()
/// \brief Get wait signal wasted seconds(only in coroutine).
static int k_wait_elapsed(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_wait_elapsed, 0);
	
	scp_routine_t* pRoutine = scp_get_routine(state);

	if (NULL == pRoutine)
	{
		return scp_error(state, "(k_wait_elapsed)Must coroutine.");
	}
	
	lua_pushnumber(state, scp_routine_get_elapse(pRoutine));
	return 1;
}

/// bool k_set_global(string name, var value)
/// \brief Set global value.
/// \param name Global name.
/// \param value Global value.
static int k_set_global(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_set_global, 2);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_set_global)Need name.");
	}

	InnerArgData value;

	if (!scp_util_read_arg_data(state, 2, &value))
	{
		return scp_error(state, "(k_set_global)Need value.");
	}

	lua_pushboolean(state, frame_set_global_value(name, value));
	return 1;
}

/// var k_global(string name)
/// \brief Get global value.
/// \param name Global name.
static int k_global(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_global, 1);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_global)Need name.");
	}

	IArgData* pGlobal = frame_get_global(name);

	if (NULL == pGlobal)
	{
		lua_pushnil(state);
	}
	else
	{
		scp_util_push_arg_data(state, *pGlobal);
	}

	return 1;
}

/// table k_global_list()
/// \brief Get global name list.
static int k_global_list(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_global_list, 0);

	InnerArgList global_names;

	frame_get_global_list(&global_names);
	scp_util_push_table(state, global_names);
	return 1;
}

/// bool k_find_global(string name)
/// \brief Find global name.
/// \param name Global name.
static int k_find_global(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_find_global, 1);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_find_global)Need name.");
	}

	lua_pushboolean(state, frame_find_global_value(name));
	return 1;
}

/// bool k_remove_global(string name)
/// \brief Remove global name.
/// \param name Global name.
static int k_remove_global(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_remove_global, 1);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_remove_global)Need name.");
	}

	lua_pushboolean(state, frame_remove_global_value(name));
	return 1;
}

/// bool k_single(string name)
/// \brief Get singleton value of object class.
/// \param name Object class name.
static int k_single(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_single, 1);

	const char* name = scp_util_read_string(state, 1);

	if (NULL == name)
	{
		return scp_error(state, "(k_single)Need name.");
	}

	if (frame_get_object_desc(name) == NULL)
	{
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	IArgData* pGlobal = frame_get_global(name);
	
	if (pGlobal)
	{
		scp_util_push_arg_data(state, *pGlobal);
		return 1;
	}

	// Object can be deleted in script.
	IObject* pObj = frame_script_create_object(name, InnerArgList());

	if (NULL == pObj)
	{
		scp_util_push_uniqid(state, uniqid_t());
		return 1;
	}

	uniqid_t id = pObj->GetUID();

	frame_set_global_value(name, InnerArgData(DT_UNIQID, id));
	scp_util_push_uniqid(state, id);
	return 1;
}

/// void k_exit()
/// \brief Exit program.
static int k_exit(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_exit, 0);

	frame_set_exit(true);
	return 0;
}

/// void k_log(int level, string info)
/// \brief Write log file.
/// \param level Log level.
/// \param info Log information content.
static int k_log(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_log, 2);

	int level = (int)lua_tonumber(state, 1);
	const char* info = scp_util_read_string(state, 2);

	if (NULL == info)
	{
		return scp_error(state, "(k_log)Need information.");
	}

	frame_log_string(level, info);
	return 0;
}

/// void k_message(string info)
/// \brief Display information.
/// \param info Information content.
static int k_message(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_message, 1);

	const char* info = scp_util_read_string(state, 1);

	if (NULL == info)
	{
		return scp_error(state, "(k_message)Need information.");
	}

	plat_popup_message(info, "script");
	return 0;
}

/// string k_path(string type)
/// \brief Get specified type path.
/// \param type Path type("work","script","coder","asset","output").
static int k_path(lua_State* state)
{
	VERIFY_ARGUMENTS(state, k_path, 1);

	const char* type = scp_util_read_string(state, 1);

	if (NULL == type)
	{
		return scp_error(state, "(k_path)Need type.");
	}

	if (stricmp(type, "work") == 0)
	{
		lua_pushstring(state, frame_get_work_path());
	}
	else if (stricmp(type, "script") == 0)
	{
		lua_pushstring(state, frame_get_script_path());
	}
	else if (stricmp(type, "coder") == 0)
	{
		lua_pushstring(state, frame_get_coder_path());
	}
	else if (stricmp(type, "asset") == 0)
	{
		lua_pushstring(state, frame_get_asset_path());
	}
	else if (stricmp(type, "output") == 0)
	{
		lua_pushstring(state, frame_get_output_path());
	}
	else
	{
		return scp_error(state, "(k_path)Error type.");
	}

	return 1;
}

#define SCRIPT_EXTEND(state, p) lua_register(state, #p, p)

void register_extend_functions(lua_State* state)
{
	// Data type related.
	SCRIPT_EXTEND(state, k_boolean);
	SCRIPT_EXTEND(state, k_int32);
	SCRIPT_EXTEND(state, k_int64);
	SCRIPT_EXTEND(state, k_float);
	SCRIPT_EXTEND(state, k_number);
	SCRIPT_EXTEND(state, k_string);
	SCRIPT_EXTEND(state, k_uniqid);
	SCRIPT_EXTEND(state, k_type);
	// Object related.
	SCRIPT_EXTEND(state, k_find_class);
	SCRIPT_EXTEND(state, k_create);
	SCRIPT_EXTEND(state, k_delete);
	SCRIPT_EXTEND(state, k_bind);
	SCRIPT_EXTEND(state, k_event);
	SCRIPT_EXTEND(state, k_class_name);
	SCRIPT_EXTEND(state, k_main_object);
	SCRIPT_EXTEND(state, k_find_object);
	SCRIPT_EXTEND(state, k_exists);
	SCRIPT_EXTEND(state, k_derive);
	// Property and method.
	SCRIPT_EXTEND(state, k_find_property);
	SCRIPT_EXTEND(state, k_find_extra);
	SCRIPT_EXTEND(state, k_find_method);
	SCRIPT_EXTEND(state, k_property_list);
	SCRIPT_EXTEND(state, k_extra_list);
	SCRIPT_EXTEND(state, k_method_list);
	SCRIPT_EXTEND(state, k_property);
	SCRIPT_EXTEND(state, k_extra);
	SCRIPT_EXTEND(state, k_method);
	SCRIPT_EXTEND(state, k_set_property);
	SCRIPT_EXTEND(state, k_set_extra);
	SCRIPT_EXTEND(state, k_remove_extra);
	// Script procedure.
	SCRIPT_EXTEND(state, k_find_script);
	SCRIPT_EXTEND(state, k_find_routine);
	SCRIPT_EXTEND(state, k_run_routine);
	SCRIPT_EXTEND(state, k_kill_routine);
	SCRIPT_EXTEND(state, k_wait_signal);
	SCRIPT_EXTEND(state, k_send_signal);
	SCRIPT_EXTEND(state, k_wait);
	SCRIPT_EXTEND(state, k_wait_elapsed);
	// Global value.
	SCRIPT_EXTEND(state, k_set_global);
	SCRIPT_EXTEND(state, k_global);
	SCRIPT_EXTEND(state, k_global_list);
	SCRIPT_EXTEND(state, k_find_global);
	SCRIPT_EXTEND(state, k_remove_global);
	SCRIPT_EXTEND(state, k_single);
	// Misc functions.
	SCRIPT_EXTEND(state, k_exit);
	SCRIPT_EXTEND(state, k_log);
	SCRIPT_EXTEND(state, k_message);
	SCRIPT_EXTEND(state, k_path);
}
