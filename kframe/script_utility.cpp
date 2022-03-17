// script_utility.cpp
// Created by lulimin on 2019/2/17.

#include <new>
#include "script_utility.h"
#include "script_meta.h"
#include "script_routine.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "../inc/arg_data.h"
#include "../inc/arg_list.h"
#include "../inc/platform.h"
#include "../inc/auto_string.h"
#include "../inc/i_file_access.h"
#include "../ext/lua-5.3.6/lstate.h"
#include <float.h>

DEFINE_INNER_ALLOC(ScriptData);

typedef TArgData<4, ScriptDataAlloc> ScriptArgData;

// Custom lua script memory allocator.
static void* custom_realloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
	if (NULL == ptr)
	{
		if (0 == nsize)
		{
			return NULL;
		}

		return K_INNER_ALLOC(nsize);
	}

	if (0 == nsize) 
	{
		K_INNER_FREE(ptr);
		return NULL;
	}

	void* new_ptr = K_INNER_ALLOC(nsize);

	if (nsize >= osize)
	{
		memcpy(new_ptr, ptr, osize);
	}
	else
	{
		memcpy(new_ptr, ptr, nsize);
	}

	K_INNER_FREE(ptr);
	return new_ptr;
}

// Write log.
static bool __cdecl debug_write_log(lua_State* state, const char* info, ...)
{
	va_list	args;
	char buf[256];
	
	va_start(args, info);
	plat_safe_sprint_list(buf, sizeof(buf), info, args);
	va_end(args);
	return frame_log_string(FRAME_DEFAULT_LOG, buf);
}

int __cdecl scp_util_error_handler(lua_State* state, const char* err, ...)
{
	va_list	args;
	
	va_start(args, err);
	
	int res = scp_util_error_handler_list(state, err, args);

	va_end(args);
	return res;
}

int scp_util_error_handler_list(lua_State* state, const char* err, 
	va_list args)
{
	char buf[256] = { 0 };
	
	plat_safe_sprint_list(buf, sizeof(buf), err, args);
	debug_write_log(state, buf);
	//fprintf(stderr, buf);
	fputs(buf, stderr);
	
	lua_Debug ar;
	int level = 0;
	char code_file[256] = { 0 };
	int code_line = 0;
	
	while (lua_getstack(state, level++, &ar))
	{
		if (lua_getinfo(state, "Sl", &ar))
		{
			debug_write_log(state, "(file:%s line:%d)", ar.source,
				ar.currentline);
			fprintf(stderr, "(file:%s line:%d)", ar.source, 
				ar.currentline);

			if ((code_line == 0) && (ar.currentline > 0))
			{
				// Write first validate code position information.
				util_string_copy(code_file, sizeof(code_file), ar.source);
				code_line = ar.currentline;
			}
		}
	}

	fflush(stderr);
	
	char info[256];

	plat_safe_sprintf(info, sizeof(info), "%s(file:%s line:%d)", buf, 
		code_file, code_line);
	plat_popup_error(info, "lua error");
	lua_pushstring(state, buf);
	lua_error(state);
	return 1;
}

bool scp_util_run_error(lua_State* state, const char* script, 
	const char* func, int res)
{
	char buf[256];
	
	plat_safe_sprintf(buf, sizeof(buf), 
		"(script:%s function:%s result:%d)", script, func, res);

	const char* err = "";

	if (LUA_ERRRUN == res)
	{
		err = lua_tostring(state, -1);

		if (NULL == err)
		{
			err = "";
		}
		
		debug_write_log(state, err);
	}
	
	debug_write_log(state, buf);

	char info[256];
	
	plat_safe_sprintf(info, sizeof(info), "%s %s", err, buf);
	plat_popup_error(info, "run error");
	return true;
}

lua_State* scp_util_open_state()
{
	lua_State* state = lua_newstate(custom_realloc, NULL);

	// lua 5.1, lua 5.3
	luaL_openlibs(state);
	return state;
}

bool scp_util_close_state(lua_State* state)
{
	Assert(state != NULL);
	
	lua_close(state);
	return true;
}

struct load_lua_buffer_t 
{
	void* fp;
	char buffer[8192];
};

// Custom lua script read function.
static const char* read_lua_file(lua_State* state, void* ud, size_t* size) 
{
	load_lua_buffer_t* lf = (load_lua_buffer_t*)ud;
	
	*size = frame_file_access()->FileRead(lf->fp, lf->buffer, 
		sizeof(lf->buffer));

	if (*size == 0)
	{
		return NULL;
	}
	
	return lf->buffer;
}

// Custom lua script module loader.
static int custom_lua_module_loader(lua_State* state) 
{
	const char* name = luaL_checkstring(state, 1);
	char fname[256];

	plat_safe_sprintf(fname, sizeof(fname), "%s%s.lua", 
		frame_get_script_path(), name);
	
	load_lua_buffer_t lf;
	
	lf.fp = frame_file_access()->FileOpen(fname, "rb");

	if (NULL == lf.fp)
	{
		return 1;  // Library not found in this path.
	}
	
	// lua 5.1
	//int status = lua_load(state, read_lua_file, &lf, name);
	// lua 5.3
	int status = lua_load(state, read_lua_file, &lf, name, NULL);

	frame_file_access()->FileClose(lf.fp);
	
	if (status != 0)
	{
		char info[256];
		
		plat_safe_sprintf(info, sizeof(info), "(load module:%s result:%d)", 
			name, status);
		lua_pushstring(state, info);
		lua_error(state);
	}
	
	return 1;  // Library load successfully.
}

// lua 5.3
bool scp_util_set_searcher(lua_State* state)
{
	Assert(state != NULL);

	// Get package.searchers table.
	lua_getglobal(state, "package");
	lua_getfield(state, -1, "searchers");
	// Insert custom loader into index 2.
	lua_pushcfunction(state, custom_lua_module_loader);

	for (int i = (int)lua_rawlen(state, -2) + 1; i > 2; --i)
	{
		lua_rawgeti(state, -2, i - 1);
		// The searchers table now is at -3.
		lua_rawseti(state, -3, i);
	}

	lua_rawseti(state, -2, 2);
	// Set searchers into package.
	lua_setfield(state, -2, "searchers");
	lua_pop(state, 1);
	return true;
}

// lua 5.1
//bool scp_util_set_search_path(lua_State* state, const char* path)
//{
//	Assert(state != NULL);
//	Assert(path != NULL);
//	
//	// lua 5.1
//	lua_pushstring(state, "package");
//	lua_rawget(state, LUA_GLOBALSINDEX);
//	lua_pushstring(state, "loaders");
//	lua_rawget(state, -2);
//	lua_pushcfunction(state, custom_lua_module_loader);
//	lua_rawseti(state, -2, 2);
//	lua_pop(state, 2);
//	return true;
//}

//bool scp_util_clear_loaded_module(lua_State* state)
//{
//	Assert(state != NULL);
//
//	// lua 5.1
//	lua_pushstring(state, "package");
//	lua_rawget(state, LUA_GLOBALSINDEX);
//	
//	int table_index = lua_gettop(state);
//	
//	lua_pushstring(state, "loaded");
//	lua_newtable(state);
//	lua_rawset(state, table_index);
//	lua_pop(state, 1);
//	return true;
//}

bool scp_util_load_file(lua_State* state, const char* name)
{
	Assert(state != NULL);
	Assert(name != NULL);
	
	load_lua_buffer_t lf;
	
	lf.fp = frame_file_access()->FileOpen(name, "rb");
	
	if (NULL == lf.fp)
	{
		char info[256];

		plat_safe_sprintf(info, sizeof(info), 
			"(scp_util_load_file)Open file %s failed.", name);
		frame_log_string(FRAME_DEFAULT_LOG, info);
		return false;
	}
	
	// lua 5.1
	//int status = lua_load(state, read_lua_file, &lf, name);
	// lua 5.3
	int status = lua_load(state, read_lua_file, &lf, name, NULL);

	frame_file_access()->FileClose(lf.fp);
	
	if (status == 0)
	{
		int res = lua_pcall(state, 0, LUA_MULTRET, 0);
		
		if (res != 0)
		{
			const char* err = lua_tostring(state, -1);
			
			if (NULL == err)
			{
				err = "";
			}
			
			char info[256];
			
			plat_safe_sprintf(info, sizeof(info), 
				"(load script:%s result:%d)%s", name, res, err);
			frame_log_string(FRAME_DEFAULT_LOG, info);
#ifdef K_DEBUG
			plat_popup_error(info, "load error");
#endif // K_DEBUG
			return false;
		}
	} 
	else
	{
		const char* err = lua_tostring(state, -1);
			
		if (NULL == err)
		{
			err = "";
		}
		
		char info[256];
		
		plat_safe_sprintf(info, sizeof(info), 
			"(compile script:%s result:%d)%s", name, status, err);
		frame_log_string(FRAME_DEFAULT_LOG, info);
		plat_popup_error(info, "compile error");
		return false;
	}
	
	return status == 0;
}

static inline const char* get_thread_table_name() 
{
	return "kframe_thread_table";
}

static int get_thread_table(lua_State* state)
{
	const char* table_name = get_thread_table_name();
	
	lua_pushstring(state, table_name);
	// lua 5.1
	//lua_rawget(state, LUA_GLOBALSINDEX);
	// lua 5.3
	lua_rawget(state, LUA_REGISTRYINDEX);

	int table_index = lua_gettop(state);
	
	if (lua_type(state, table_index) == LUA_TNIL)
	{
		lua_pop(state, 1);
		lua_pushstring(state, table_name);
		lua_newtable(state);
		// lua 5.1
		//lua_rawset(state, LUA_GLOBALSINDEX);
		// lua 5.3
		lua_rawset(state, LUA_REGISTRYINDEX);
		lua_pushstring(state, table_name);
		// lua 5.1
		//lua_rawget(state, LUA_GLOBALSINDEX);
		// lua 5.3
		lua_rawget(state, LUA_REGISTRYINDEX);
		table_index = lua_gettop(state);
		
		if (lua_type(state, table_index) == LUA_TNIL)
		{
			lua_pop(state, 1);
			return 0;
		}
	}
	
	return table_index;
}

lua_State* scp_util_new_thread(lua_State* state, size_t routine_id)
{
	int table = get_thread_table(state);
	
	// Table index start from 1.
	lua_pushnumber(state, (lua_Number)(routine_id + 1));
	
	// Create and push new coroutine.
	lua_State* new_state = lua_newthread(state);

	lua_rawset(state, table);
	lua_pop(state, 1);
	return new_state;
}

bool scp_util_delete_thread(lua_State* state, size_t routine_id)
{
	int table = get_thread_table(state);
	
	lua_pushnumber(state, (lua_Number)(routine_id + 1));
	lua_pushnil(state);
	lua_rawset(state, table);
	lua_pop(state, 1);
	return true;
}

void scp_util_set_context(lua_State* state, void* pRoutine)
{
	state->pRoutine = pRoutine;
}

void* scp_util_get_context(lua_State* state)
{
	return state->pRoutine;
}

void scp_util_push_context_method(lua_State* state, IMethodDesc* pMethod)
{
	if (state->pRoutine)
	{
		scp_routine_push_method((scp_routine_t*)state->pRoutine, pMethod);
	}
	else
	{
		frame_push_method(pMethod);
	}
}

void scp_util_pop_context_method(lua_State* state)
{
	if (state->pRoutine)
	{
		scp_routine_pop_method((scp_routine_t*)state->pRoutine);
	}
	else
	{
		frame_pop_method();
	}
}

IMethodDesc* scp_util_get_context_method(lua_State* state)
{
	if (state->pRoutine)
	{
		return scp_routine_get_method((scp_routine_t*)state->pRoutine);
	}
	else
	{
		return frame_get_method();
	}
}

static int inner_push_data(lua_State* state, const IArgData& value)
{
	ScriptArgData* pData = (ScriptArgData*)lua_newuserdata(state, 
		sizeof(ScriptArgData));
	
	new (pData) ScriptArgData(value);
	luaL_getmetatable(state, scp_get_meta_table_name());
	lua_setmetatable(state, -2);
	return 1;
}

int scp_util_push_int32(lua_State* state, int value)
{
	return inner_push_data(state, ScriptArgData(DT_INT32, value));
}

int scp_util_push_int64(lua_State* state, int64_t value)
{
	return inner_push_data(state, ScriptArgData(DT_INT64, value));
}

int scp_util_push_float(lua_State* state, float value)
{
	return inner_push_data(state, ScriptArgData(DT_FLOAT, value));
}

int scp_util_push_double(lua_State* state, double value)
{
	return inner_push_data(state, ScriptArgData(DT_DOUBLE, value));
}

int scp_util_push_uniqid(lua_State* state, const uniqid_t& value)
{
	return inner_push_data(state, ScriptArgData(DT_UNIQID, value));
}

int scp_util_push_arg_data(lua_State* state, const IArgData& value)
{
	switch (value.GetType())
	{
	case DT_NULL:
		lua_pushnil(state);
		return 1;
	case DT_BOOLEAN:
		lua_pushboolean(state, value.GetBoolean());
		return 1;
	case DT_INT32:
		lua_pushnumber(state, value.GetInt32());
		return 1;
	case DT_INT64:
		lua_pushnumber(state, (lua_Number)value.GetInt64());
		return 1;
	case DT_FLOAT:
		lua_pushnumber(state, value.GetFloat());
		return 1;
	case DT_DOUBLE:
		lua_pushnumber(state, value.GetDouble());
		return 1;
	case DT_STRING:
		lua_pushstring(state, value.GetString());
		return 1;
	default:
		break;
	}
	
	return inner_push_data(state, value);
}

int scp_util_push_arg_list(lua_State* state, const IArgList& value)
{
	int count = 0;
	const size_t value_num = value.GetCount();

	for (size_t i = 0; i < value_num; ++i)
	{
		switch (value.GetType(i))
		{
		case DT_BOOLEAN:
			lua_pushboolean(state, value.GetBoolean(i));
			break;
		case DT_INT32:
			lua_pushnumber(state, value.GetInt32(i));
			break;
		case DT_INT64:
			lua_pushnumber(state, (lua_Number)value.GetInt64(i));
			break;
		case DT_FLOAT:
			lua_pushnumber(state, value.GetFloat(i));
			break;
		case DT_DOUBLE:
			lua_pushnumber(state, value.GetDouble(i));
			break;
		case DT_STRING:
			lua_pushstring(state, value.GetString(i));
			break;
		case DT_UNIQID:
			scp_util_push_uniqid(state, value.GetUniqid(i));
			break;
		default:
			continue;
			break;
		}

		++count;
	}

	return count;
}

int scp_util_push_table(lua_State* state, const IArgList& value)
{
	lua_newtable(state);
	
	size_t num = value.GetCount();
	
	for (size_t i = 0; i < num; ++i)
	{
		switch (value.GetType(i))
		{
		case DT_BOOLEAN:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushboolean(state, value.GetBoolean(i));
			lua_rawset(state, -3);
			break;
		case DT_INT32:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushnumber(state, value.GetInt32(i));
			lua_rawset(state, -3);
			break;
		case DT_INT64:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushnumber(state, (lua_Number)value.GetInt64(i));
			lua_rawset(state, -3);
			break;
		case DT_FLOAT:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushnumber(state, value.GetFloat(i));
			lua_rawset(state, -3);
			break;
		case DT_DOUBLE:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushnumber(state, value.GetDouble(i));
			lua_rawset(state, -3);
			break;
		case DT_STRING:
			lua_pushnumber(state, lua_Number(i + 1));
			lua_pushstring(state, value.GetString(i));
			lua_rawset(state, -3);
			break;
		case DT_UNIQID:
			lua_pushnumber(state, lua_Number(i + 1));
			scp_util_push_uniqid(state, value.GetUniqid(i));
			lua_rawset(state, -3);
			break;
		default:
			break;
		}
	}

	return 1;
}

bool scp_util_read_number(lua_State* state, int index, double* value)
{
	switch (lua_type(state, index))
	{
	case LUA_TNUMBER:
		*value = lua_tonumber(state, index);
		break;
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, index);
			
		switch (pData->GetType())
		{
		case DT_INT32:
			*value = pData->GetInt32();
			break;
		case DT_INT64:
			*value = (double)pData->GetInt64();
			break;
		case DT_FLOAT:
			*value = pData->GetFloat();
			break;
		case DT_DOUBLE:
			*value = pData->GetDouble();
			break;
		default:
			return false;
		}

		break;
	}
	default:
		return false;
	}
	
	return true;
}

const char* scp_util_read_string(lua_State* state, int index)
{
	switch (lua_type(state, index))
	{
	case LUA_TSTRING:
		return lua_tostring(state, index);
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, index);

		if (pData->GetType() == DT_STRING)
		{
			return pData->GetString();
		}

		break;
	}
	default:
		break;
	}
	
	return NULL;
}

bool scp_util_read_uniqid(lua_State* state, int index, uniqid_t* value)
{
	if (lua_type(state, index) != LUA_TUSERDATA)
	{
		return false;
	}
	
	IArgData* pData = (IArgData*)lua_touserdata(state, index);
	
	if (pData->GetType() != DT_UNIQID)
	{
		return false;
	}
	
	*value = pData->GetUniqid();
	return true;
}

bool scp_util_read_arg_data(lua_State* state, int index, IArgData* value)
{
	switch (lua_type(state, index))
	{
	case LUA_TNIL:
		value->SetNull();
		break;
	case LUA_TBOOLEAN:
		value->SetBoolean(lua_toboolean(state, index) != 0);
		break;
	case LUA_TNUMBER:
		value->SetDouble(lua_tonumber(state, index));
		break;
	case LUA_TSTRING:
		value->SetString(lua_tostring(state, index));
		break;
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, index);

		value->Assign(*pData);
		break;
	}
	default:
		return false;
	}
	
	return true;
}

bool scp_util_read_arg_list(lua_State* state, int beg, int end, 
	IArgList* value)
{
	// Cannot clear result list.
	for (int index = beg; index <= end; ++index)
	{
		switch (lua_type(state, index))
		{
		case LUA_TBOOLEAN:
			value->AddBoolean(lua_toboolean(state, index) != 0);
			break;
		case LUA_TNUMBER:
			value->AddDouble(lua_tonumber(state, index));
			break;
		case LUA_TSTRING:
			value->AddString(lua_tostring(state, index));
			break;
		case LUA_TUSERDATA:
		{
			IArgData* pData = (IArgData*)lua_touserdata(state, index);
				
			switch (pData->GetType())
			{
			case DT_BOOLEAN:
				value->AddBoolean(pData->GetBoolean());
				break;
			case DT_INT32:
				value->AddInt32(pData->GetInt32());
				break;
			case DT_INT64:
				value->AddInt64(pData->GetInt64());
				break;
			case DT_FLOAT:
				value->AddFloat(pData->GetFloat());
				break;
			case DT_DOUBLE:
				value->AddDouble(pData->GetDouble());
				break;
			case DT_STRING:
				value->AddString(pData->GetString());
				break;
			case DT_UNIQID:
				value->AddUniqid(pData->GetUniqid());
				break;
			default:
				break;
			}

			break;
		}
		default:
			break;
		}
	}
	
	return true;
}

static int compare_int64(int64_t v1, int64_t v2)
{
	if (v1 > v2)
	{
		return 1;
	}

	if (v1 < v2)
	{
		return -1;
	}

	return 0;
}

static bool compare_arg_int64(IArgData* pData1, int64_t v2, int* res)
{
	switch (pData1->GetType())
	{
	case DT_BOOLEAN:
		break;
	case DT_INT32:
	case DT_INT64:
	{
		int64_t value1 = pData1->GetInt64();

		*res = compare_int64(value1, v2);
		return true;
	}
	case DT_FLOAT:
	case DT_DOUBLE:
	{
		double value1 = pData1->GetDouble();

		*res = compare_int64((int64_t)value1, v2);
		return true;
	}
	case DT_UNIQID:
		break;
	default:
		break;
	}

	return false;
}

static int compare_double(double v1, double v2)
{
	if (v1 > (v2 + DBL_EPSILON))
	{
		return 1;
	}

	if (v1 < (v2 - DBL_EPSILON))
	{
		return -1;
	}
	
	return 0;
}

static bool compare_arg_double(IArgData* pData1, double v2, int* res)
{
	switch (pData1->GetType())
	{
	case DT_BOOLEAN:
		break;
	case DT_INT32:
	case DT_INT64:
	{
		int64_t value1 = pData1->GetInt32();

		*res = compare_double((double)value1, v2);
		return true;
	}
	case DT_FLOAT:
	case DT_DOUBLE:
	{
		double value1 = pData1->GetDouble();
			
		*res = compare_double(value1, v2);
		return true;
	}
	case DT_UNIQID:
		break;
	default:
		break;
	}

	return false;
}

static bool compare_boolean(lua_State* state, int value1, int index2, int* res)
{
	switch (lua_type(state, index2))
	{
	case LUA_TBOOLEAN:
	{
		int value2 = lua_toboolean(state, index2);

		*res = value1 - value2;
		return true;
	}
	case LUA_TNUMBER:
		break;
	case LUA_TSTRING:
		break;
	case LUA_TUSERDATA:
	{
		IArgData* pData2 = (IArgData*)lua_touserdata(state, index2);

		if (pData2->GetType() == DT_BOOLEAN)
		{
			int value2 = (int)pData2->GetBoolean();

			*res = value1 - value2;
			return true;
		}

		break;
	}
	default:
		break;
	}

	return false;
}

static bool compare_number(lua_State* state, double value1, int index2,
	int* res)
{
	switch (lua_type(state, index2))
	{
	case LUA_TBOOLEAN:
		break;
	case LUA_TNUMBER:
	{
		double value2 = lua_tonumber(state, index2);

		*res = compare_double(value1, value2);
		return true;
	}
	case LUA_TSTRING:
		break;
	case LUA_TUSERDATA:
	{
		IArgData* pData2 = (IArgData*)lua_touserdata(state, index2);

		switch (pData2->GetType())
		{
		case DT_BOOLEAN:
			break;
		case DT_INT32:
		case DT_INT64:
		{
			int64_t value2 = pData2->GetInt64();
					
			*res = compare_double(value1, (double)value2);
			return true;
		}
		case DT_FLOAT:
		case DT_DOUBLE:
		{
			double value2 = pData2->GetDouble();
					
			*res = compare_double(value1, value2);
			return true;
		}
		case DT_UNIQID:
			break;
		default:
			break;
		}

		break;
	}
	default:
		break;
	}
	
	return false;
}

static bool compare_string(lua_State* state, const char* value1, int index2,
	int* res)
{
	switch (lua_type(state, index2))
	{
	case LUA_TBOOLEAN:
		break;
	case LUA_TNUMBER:
		break;
	case LUA_TSTRING:
	{
		const char* value2 = lua_tostring(state, index2);

		*res = strcmp(value1, value2);
		return true;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData = (IArgData*)lua_touserdata(state, index2);
			
		switch (pData->GetType())
		{
		case DT_STRING:
		{
			const char* value2 = pData->GetString();
					
			*res = strcmp(value1, value2);
			return true;
		}
		default:
			break;
		}

		break;
	}
	default:
		break;
	}
	
	return false;
}

static bool compare_userdata(lua_State* state, IArgData* pData1, int index2,
	int* res)
{
	switch (lua_type(state, index2))
	{
	case LUA_TBOOLEAN:
	{
		int value2 = lua_toboolean(state, index2);

		if (pData1->GetType() == DT_BOOLEAN)
		{
			int value1 = (int)pData1->GetBoolean();

			*res = value1 - value2;
			return true;
		}

		break;
	}
	case LUA_TNUMBER:
	{
		double value2 = lua_tonumber(state, index2);
			
		return compare_arg_double(pData1, value2, res);
	}
	case LUA_TSTRING:
	{
		const char* value2 = lua_tostring(state, index2);
			
		if (pData1->GetType() == DT_STRING)
		{
			const char* value1 = pData1->GetString();
			
			*res = strcmp(value1, value2);
			return true;
		}

		break;
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData2 = (IArgData*)lua_touserdata(state, index2);
			
		switch (pData2->GetType())
		{
		case DT_BOOLEAN:
		{
			int value2 = (int)pData2->GetBoolean();

			if (pData1->GetType() == DT_BOOLEAN)
			{
				int value1 = (int)pData1->GetBoolean();

				*res = value1 - value2;
				return true;
			}

			break;
		}
		case DT_INT32:
		case DT_INT64:
		{
			int64_t value2 = pData2->GetInt64();

			return compare_arg_int64(pData1, value2, res);
		}
		case DT_FLOAT:
		case DT_DOUBLE:
		{
			double value2 = pData2->GetDouble();
					
			return compare_arg_double(pData1, value2, res);
		}
		case DT_STRING:
		{
			const char* value2 = pData2->GetString();
					
			if (pData1->GetType() == DT_STRING)
			{
				const char* value1 = pData1->GetString();
				
				*res = strcmp(value1, value2);
				return true;
			}

			break;
		}
		case DT_UNIQID:
		{
			uniqid_t value2 = pData2->GetUniqid();

			if (pData1->GetType() == DT_UNIQID)
			{
				uniqid_t value1 = pData1->GetUniqid();

				*res = compare_int64(value1.nValue64, value2.nValue64);
				return true;
			}

			break;
		}
		default:
			break;
		}

		break;
	}
	default:
		break;
	}
	
	return false;
}

bool scp_util_compare(lua_State* state, int index1, int index2, int* res)
{
	switch (lua_type(state, index1))
	{
	case LUA_TBOOLEAN:
	{
		int value1 = lua_toboolean(state, index1);

		return compare_boolean(state, value1, index2, res);
	}
	case LUA_TNUMBER:
	{
		double value1 = lua_tonumber(state, index1);
			
		return compare_number(state, value1, index2, res);
	}
	case LUA_TSTRING:
	{
		const char* value1 = lua_tostring(state, index1);

		return compare_string(state, value1, index2, res);
	}
	case LUA_TUSERDATA:
	{
		IArgData* pData1 = (IArgData*)lua_touserdata(state, index1);

		return compare_userdata(state, pData1, index2, res);
	}
	default:
		break;
	}
	
	return false;
}
