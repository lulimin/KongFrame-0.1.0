// script_utility.h
// Created by lulimin on 2019/2/17.

#ifndef __SCRIPT_UTILITY_H
#define __SCRIPT_UTILITY_H

#include "../inc/common.h"
#include "../inc/uniqid.h"

extern "C"
{
	#include "../ext/lua-5.3.6/lua.h"
	#include "../ext/lua-5.3.6/lualib.h"
	#include "../ext/lua-5.3.6/lauxlib.h"
}

// Script utilities.

class IArgData;
class IArgList;
class IMethodDesc;

// Script error handler.
int __cdecl scp_util_error_handler(lua_State* state, const char* err, ...);
int scp_util_error_handler_list(lua_State* state, const char* err, 
	va_list args);
// Script run error process.
bool scp_util_run_error(lua_State* state, const char* script, 
	const char* func, int res);

// Create script VM.
lua_State* scp_util_open_state();
// Delete script VM.
bool scp_util_close_state(lua_State* state);

// Set script search path.
//bool scp_util_set_search_path(lua_State* state, const char* path);
// Set script searcher.
bool scp_util_set_searcher(lua_State* state);

// Clear script loaded modules.
//bool scp_util_clear_loaded_module(lua_State* state);

// Load script file.
bool scp_util_load_file(lua_State* state, const char* name);

// Create script coroutine.
lua_State* scp_util_new_thread(lua_State* state, size_t routine_id);
// Delete script coroutine.
bool scp_util_delete_thread(lua_State* state, size_t routine_id);

// Set script context parameter.
void scp_util_set_context(lua_State* state, void* pRoutine);
// Get scritp context parameter.
void* scp_util_get_context(lua_State* state);

// Object method stack.
void scp_util_push_context_method(lua_State* state, IMethodDesc* pMethod);
void scp_util_pop_context_method(lua_State* state);
IMethodDesc* scp_util_get_context_method(lua_State* state);

// Script push data.
int scp_util_push_int32(lua_State* state, int value);
int scp_util_push_int64(lua_State* state, int64_t value);
int scp_util_push_float(lua_State* state, float value);
int scp_util_push_double(lua_State* state, double value);
int scp_util_push_uniqid(lua_State* state, const uniqid_t& value);
int scp_util_push_arg_data(lua_State* state, const IArgData& value);
int scp_util_push_arg_list(lua_State* state, const IArgList& value);
int scp_util_push_table(lua_State* state, const IArgList& value);

// Script read data.
bool scp_util_read_number(lua_State* state, int index, double* value);
const char* scp_util_read_string(lua_State* state, int index);
bool scp_util_read_uniqid(lua_State* state, int index, uniqid_t* value);
bool scp_util_read_arg_data(lua_State* state, int index, IArgData* value);
bool scp_util_read_arg_list(lua_State* state, int beg, int end, 
	IArgList* value);

// Script compare data value.
bool scp_util_compare(lua_State* state, int index1, int index2, int* res);

#endif // __SCRIPT_UTILITY_H
