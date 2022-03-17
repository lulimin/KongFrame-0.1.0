// script_file.h
// Created by lulimin on 2019/2/17.

#ifndef __SCRIPT_FILE_H
#define __SCRIPT_FILE_H

#include "../inc/common.h"
#include "../inc/i_file_access.h"

struct lua_State;
struct scp_file_t;

// Create script file struct.
scp_file_t* scp_file_create(lua_State* state, const char* path,
	const char* name);
// Delete script file struct.
void scp_file_delete(scp_file_t* pScpFile);

// Reset script VM.
void scp_file_set_state(scp_file_t* pScpFile, lua_State* state);
// Get script VM.
lua_State* scp_file_get_state(scp_file_t* pScpFile);
// Get script path.
const char* scp_file_get_path(scp_file_t* pScpFile);
// Get script file name.
const char* scp_file_get_name(scp_file_t* pScpFile);

// Set script file time.
void scp_file_set_time(scp_file_t* pScpFile, 
	const IFileAccess::file_time_t& value);
// Get script file time.
const IFileAccess::file_time_t& scp_file_get_time(scp_file_t* pScpFile);

// Search script function.
bool scp_file_find_function(scp_file_t* pScpFile, const char* name);
// Increase reference count.
void scp_file_inc_ref_count(scp_file_t* pScpFile);
// Decrease reference count.
void scp_file_dec_ref_count(scp_file_t* pScpFile);
// Get reference count.
int scp_file_get_ref_count(scp_file_t* pScpFile);

// Create script coroutine.
lua_State* scp_file_new_routine(scp_file_t* pScpFile, size_t* pRoutineId);
// Delete script coroutine.
bool scp_file_delete_routine(scp_file_t* pScpFile, size_t routine_id);

#endif // __SCRIPT_FILE_H
