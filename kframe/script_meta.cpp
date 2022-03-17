// script_meta.cpp
// Created by lulimin on 2019/2/17.

#include "script_meta.h"
#include "script_utility.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "../inc/arg_data.h"
#include "../inc/arg_list.h"
#include "../inc/i_object.h"
#include "../inc/i_object_desc.h"
#include "../inc/i_data_table.h"
#include <math.h>

typedef TArgList<8, 128, FrameInnerAlloc> InnerArgList;

static int __cdecl scp_error(lua_State* state, const char* err, ...)
{
	va_list	args;
	
	va_start(args, err);

	int res = scp_util_error_handler_list(state, err, args);

	va_end(args);
	return res;
}

// Add(+).
static int meta_add(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num != 2)
	{
		return scp_error(state, "(meta_add)Need two operand.");
	}

	double op1;

	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_add)First operand not number.");
	}
	
	double op2;
	
	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_add)Second operand not number.");
	}
	
	lua_pushnumber(state, op1 + op2);
	return 1;
}

// Subtract(-).
static int meta_sub(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_sub)Need two operand.");
	}
	
	double op1;
	
	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_sub)First operand not number.");
	}
	
	double op2;
	
	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_sub)Second operand not number.");
	}

	lua_pushnumber(state, op1 - op2);
	return 1;
}

// Multiple(*).
static int meta_mul(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_mul)Need two operand.");
	}
	
	double op1;
	
	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_mul)First operand not number.");
	}
	
	double op2;
	
	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_mul)Second operand not number.");
	}
	
	lua_pushnumber(state, op1 * op2);
	return 1;
}

// Divide(/).
static int meta_div(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_div)Need two operand.");
	}
	
	double op1;
	
	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_div)First operand not number.");
	}
	
	double op2;
	
	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_div)Second operand not number.");
	}
	
	lua_pushnumber(state, op1 / op2);
	return 1;
}

// Modulo(%).
static int meta_mod(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num != 2)
	{
		return scp_error(state, "(meta_mod)Need two operand.");
	}

	double op1;

	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_mod)First operand not number.");
	}

	double op2;

	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_mod)Second operand not number.");
	}

	lua_pushnumber(state, op1 - floor(op1 / op2) * op2); // fmod(op1, op2)
	return 1;
}

// Exponentiation(^).
static int meta_pow(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num != 2)
	{
		return scp_error(state, "(meta_pow)Need two operand.");
	}

	double op1;

	if (!scp_util_read_number(state, 1, &op1))
	{
		return scp_error(state, "(meta_pow)First operand not number.");
	}

	double op2;

	if (!scp_util_read_number(state, 2, &op2))
	{
		return scp_error(state, "(meta_pow)Second operand not number.");
	}

	lua_pushnumber(state, pow(op1, op2));
	return 1;
}

// Negative(-).
static int meta_umn(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num != 1)
	{
		return scp_error(state, "(meta_umn)Need one operand");
	}
	
	double op;
	
	if (!scp_util_read_number(state, 1, &op))
	{
		return scp_error(state, "(meta_umn)First operand not number.");
	}
	
	lua_pushnumber(state, -op);
	return 1;
}

// Concat(..).
static int meta_concat(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_concat)Need two operand.");
	}
	
	const char* op1 = scp_util_read_string(state, 1);

	if (NULL == op1)
	{
		return scp_error(state, "(meta_concat)First operand not string.");
	}
	
	const char* op2 = scp_util_read_string(state, 2);
	
	if (NULL == op2)
	{
		return scp_error(state, "(meta_concat)Second operand not string.");
	}

	TAutoString<char, 128, TCharTraits<char>, FrameInnerAlloc> s(op1, op2);
	
	lua_pushstring(state, s.CString());
	return 1;
}

// Length(#).
static int meta_len(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num != 1)
	{
		return scp_error(state, "(meta_len)Need one operand.");
	}

	const char* op = scp_util_read_string(state, 1);

	if (NULL == op)
	{
		return scp_error(state, "(meta_len)First operand not string.");
	}

	lua_pushnumber(state, (lua_Number)strlen(op));
	return 1;
}

// Equal(==).
static int meta_eq(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_eq)Need two operand.");
	}
	
	int res;

	if (!scp_util_compare(state, 1, 2, &res))
	{
		return scp_error(state, "(meta_eq)Cannot compare equalize.");
	}
	
	lua_pushboolean(state, res == 0);
	return 1;
}

// Less than(<).
static int meta_lt(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_lt)Need two operand.");
	}
	
	int res;
	
	if (!scp_util_compare(state, 1, 2, &res))
	{
		return scp_error(state, "(meta_lt)Cannot compare less.");
	}
	
	lua_pushboolean(state, res < 0);
	return 1;
}

// Less or equal(<=).
static int meta_le(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 2)
	{
		return scp_error(state, "(meta_le)Need two operand.");
	}
	
	int res;
	
	if (!scp_util_compare(state, 1, 2, &res))
	{
		return scp_error(state, "(meta_le)Cannot compare less-equalize.");
	}
	
	lua_pushboolean(state, res <= 0);
	return 1;
}

typedef bool (*method_func_t)(IObject*, const IArgList&, IArgList*);

// Invoke object method.
static int obj_method_launcher(lua_State* state)
{
	// Get method description.
	IMethodDesc* pMethodDesc = scp_util_get_context_method(state);

	Assert(pMethodDesc != NULL);

	// Pop method stack.
	scp_util_pop_context_method(state);

	int arg_num = lua_gettop(state);

	if (arg_num < 1)
	{
		return scp_error(state, "(obj_method_launcher)Need object pointer.");
	}

	uniqid_t id;

	if (!scp_util_read_uniqid(state, 1, &id))
	{
		return scp_error(state, "(obj_method_launcher)Get object id failed.");
	}

	IObject* pObj = frame_get_object(id);

	if (NULL == pObj)
	{
		return scp_error(state, "(obj_method_launcher)Object not found.");
	}

	// Object method middle function.
	method_func_t method_func = (method_func_t)pMethodDesc->GetMethodFunc();

	Assert(method_func != NULL);

	// Get arguments.
	InnerArgList args;

	if (!scp_util_read_arg_list(state, 2, arg_num, &args))
	{
		return scp_error(state, 
			"(obj_method_launcher)Method %s:%s arguments error.",
			pObj->GetDesc()->GetObjectName(), pMethodDesc->GetName());
	}

	// Execute method.
	InnerArgList res;

	if (!method_func(pObj, args, &res))
	{
		return scp_error(state, "(obj_method_launcher)Run method %s:%s failed.",
			pObj->GetDesc()->GetObjectName(), pMethodDesc->GetName());
	}

	if (pMethodDesc->GetResultArray())
	{
		// Return array.
		return scp_util_push_table(state, res);
	}
	else
	{
		return scp_util_push_arg_list(state, res);
	}
}

typedef void (*read_func_t)(IObject*, IArgData*);

// Read property or method(. or :).
static int meta_index(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 2)
	{
		return scp_error(state, "(meta_index)Need at least two arguments.");
	}
	
	if (lua_type(state, 2) != LUA_TSTRING)
	{
		return scp_error(state, "(meta_index)Second argument must string.");
	}
	
	// Property or method name.
	const char* key = lua_tostring(state, 2);
	IArgData* pData = (IArgData*)lua_touserdata(state, 1);
	
	if (NULL == pData)
	{
		return scp_error(state, "(meta_index)Cannot index user-data %s.", key);
	}

	if (pData->GetType() != DT_UNIQID)
	{
		return scp_error(state, "(meta_index)User-data type error %s.", key);
	}

	IObject* pObj = frame_get_object(pData->GetUniqid());
		
	if (NULL == pObj)
	{
		return scp_error(state, "(meta_index)Object not exists %s.", key);
	}

	// Try run method first.
	IMethodDesc* pMethodDesc = pObj->GetDesc()->FindMethodDesc(key);

	if (pMethodDesc != NULL)
	{
		// Run object method.
		scp_util_push_context_method(state, pMethodDesc);
		lua_pushcfunction(state, obj_method_launcher);
		return 1;
	}
		
	// Try read object internal property.
	IPropertyDesc* pPropertyDesc = pObj->GetDesc()->FindPropertyDesc(key);

	if (NULL == pPropertyDesc)
	{
		// Read object extra property.
		IDataTable* pExtra = pObj->GetExtra();

		if (NULL == pExtra)
		{
			return scp_error(state, "(meta_index)Object no extra %s.", key);
		}

		const IArgData* pChild = pExtra->GetValue(key);

		if (NULL == pChild)
		{
			return scp_error(state, "(meta_index)Key %s not in extra.", key);
		}

		scp_util_push_arg_data(state, *pChild);
		return 1;
	}

	// Read object internal property.
	read_func_t read_func = (read_func_t)pPropertyDesc->GetReadFunc();

	Assert(read_func != NULL);

	TArgData<64, FrameInnerAlloc> value;

	read_func(pObj, &value);
	scp_util_push_arg_data(state, value);
	return 1;
}

typedef void (*write_func_t)(IObject*, const IArgData&);

// Write property(.).
static int meta_newindex(lua_State* state)
{
	int arg_num = lua_gettop(state);

	if (arg_num < 3)
	{
		return scp_error(state, "(meta_newindex)Need at least three arguments");
	}

	if (lua_type(state, 2) != LUA_TSTRING)
	{
		return scp_error(state, "(meta_newindex)Second argument must string.");
	}

	// Property name.
	const char* key = lua_tostring(state, 2);
	IArgData* pData = (IArgData*)lua_touserdata(state, 1);

	if (NULL == pData)
	{
		return scp_error(state, "(meta_newindex)Cannot index user-data %s.",
			key);
	}
	
	if (pData->GetType() != DT_UNIQID)
	{
		return scp_error(state, "(meta_newindex)User-data type error %s.", key);
	}

	// Set object property.
	IObject* pObj = frame_get_object(pData->GetUniqid());
		
	if (NULL == pObj)
	{
		return scp_error(state, "(meta_newindex)Object not exists %s.", key);
	}

	// Try get internal property description.
	IPropertyDesc* pPropertyDesc = pObj->GetDesc()->FindPropertyDesc(key);

	if (NULL == pPropertyDesc)
	{
		// Set object extra property.
		IDataTable* pExtra = pObj->GetExtra();

		if (NULL == pExtra)
		{
			frame_create_extra(pObj);
			pExtra = pObj->GetExtra();
		}

		IArgData* pChild = pExtra->GetValue(key);
			
		if (NULL == pChild)
		{
			// Add if not exists.
			pChild = pExtra->Add(key, TArgData<4, FrameInnerAlloc>());
		}

		if (!scp_util_read_arg_data(state, 3, pChild))
		{
			return scp_error(state, "(meta_newindex)Extra %s value invalid.",
				key);
		}

		scp_util_push_arg_data(state, *pChild);
		return 1;
	}

	// Set internal property.
	write_func_t write_func = (write_func_t)pPropertyDesc->GetWriteFunc();

	if (NULL == write_func)
	{
		return scp_error(state, "(meta_newindex)Property %s cannot be write.",
			key);
	}

	TArgData<64, FrameInnerAlloc> value;

	if (!scp_util_read_arg_data(state, 3, &value))
	{
		return scp_error(state, "(meta_newindex)Cannot set property %s.", key);
	}

	write_func(pObj, value);
	scp_util_push_arg_data(state, value);
	return 1;
}

// Destruct user-data.
static int meta_gc(lua_State* state)
{
	int arg_num = lua_gettop(state);
	
	if (arg_num != 1)
	{
		return scp_error(state, "(meta_gc)Need one operand.");
	}
	
	IArgData* pData = (IArgData*)lua_touserdata(state, 1);
	
	pData->~IArgData();
	return 0;
}

// Get user-data metatable name.
const char* scp_get_meta_table_name()
{
	return "kframe_ud_mt";
}

// Add metatable to script VM.
void scp_add_meta_table(lua_State* state)
{
	luaL_newmetatable(state, scp_get_meta_table_name());

	lua_pushstring(state, "__add");
	lua_pushcfunction(state, meta_add);
	lua_settable(state, -3);

	lua_pushstring(state, "__sub");
	lua_pushcfunction(state, meta_sub);
	lua_settable(state, -3);

	lua_pushstring(state, "__mul");
	lua_pushcfunction(state, meta_mul);
	lua_settable(state, -3);

	lua_pushstring(state, "__div");
	lua_pushcfunction(state, meta_div);
	lua_settable(state, -3);

	lua_pushstring(state, "__mod");
	lua_pushcfunction(state, meta_mod);
	lua_settable(state, -3);

	lua_pushstring(state, "__pow");
	lua_pushcfunction(state, meta_pow);
	lua_settable(state, -3);

	lua_pushstring(state, "__umn");
	lua_pushcfunction(state, meta_umn);
	lua_settable(state, -3);

	lua_pushstring(state, "__concat");
	lua_pushcfunction(state, meta_concat);
	lua_settable(state, -3);

	lua_pushstring(state, "__len");
	lua_pushcfunction(state, meta_len);
	lua_settable(state, -3);

	lua_pushstring(state, "__eq");
	lua_pushcfunction(state, meta_eq);
	lua_settable(state, -3);

	lua_pushstring(state, "__lt");
	lua_pushcfunction(state, meta_lt);
	lua_settable(state, -3);

	lua_pushstring(state, "__le");
	lua_pushcfunction(state, meta_le);
	lua_settable(state, -3);

	lua_pushstring(state, "__index");
	lua_pushcfunction(state, meta_index);
	lua_settable(state, -3);

	lua_pushstring(state, "__newindex");
	lua_pushcfunction(state, meta_newindex);
	lua_settable(state, -3);

	lua_pushstring(state, "__gc");
	lua_pushcfunction(state, meta_gc);
	lua_settable(state, -3);

	lua_pop(state, 1);
}
