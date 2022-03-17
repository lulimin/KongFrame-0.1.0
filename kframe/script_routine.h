// script_routine.h 
// Created by lulimin on 2019/2/17.

#ifndef __SCRIPT_ROUTINE_H
#define __SCRIPT_ROUTINE_H

#include <new>
#include "../inc/common.h"
#include "../inc/auto_string.h"
#include "../inc/string_hash_table.h"
#include "../inc/pod_array.h"
#include "frame_inner_mem.h"

// Script coroutine state.
#define SCP_ROUTINE_START 0
#define SCP_ROUTINE_PAUSE 1
#define SCP_ROUTINE_STOPPED 2

class IArgData;
class IArgList;
class IMethodDesc;
struct lua_State;
struct scp_file_t;
struct scp_cache_t;

DEFINE_INNER_ALLOC(ScriptRoutine);

typedef TAutoString<char, 16, TCharTraits<char>, 
	ScriptRoutineAlloc> ScpRoutineString;
typedef TStringHashTable<char, uniqid_t, TCharTraits<char>,
	ScriptRoutineAlloc> ScpRoutineSignals;
typedef TPodArray<IMethodDesc*, 4, ScriptRoutineAlloc> ScpRoutineMethod;

struct scp_routine_t
{
	size_t nRoutineId;
	lua_State* pState;
	scp_file_t* pScpFile;
	uniqid_t Self;
	int nProgress;
	bool bReturnElapse;
	double dTimeout;
	double dElapse;
	ScpRoutineSignals* pWaitSignals;
	ScpRoutineString sFunction;
	ScpRoutineMethod MethodStack;
};

// Get script VM.
inline lua_State* scp_routine_get_state(scp_routine_t* pScpRoutine)
{
	return pScpRoutine->pState;
}

// Get script function.
inline const char* scp_routine_get_function(scp_routine_t* pScpRoutine)
{ 
	return pScpRoutine->sFunction.CString();
}

// Get object id.
inline uniqid_t scp_routine_get_self(scp_routine_t* pScpRoutine)
{
	return pScpRoutine->Self;
}

// Get time limit seconds.
inline double scp_routine_get_timeout(scp_routine_t* pScpRoutine)
{ 
	return pScpRoutine->dTimeout;
}

// Get wait elapse time.
inline double scp_routine_get_elapse(scp_routine_t* pScpRoutine)
{ 
	return pScpRoutine->dElapse;
}

// Test stopped.
inline bool scp_routine_stopped(scp_routine_t* pScpRoutine)
{ 
	return SCP_ROUTINE_STOPPED == pScpRoutine->nProgress;
}

// Push object method to stack.
inline void scp_routine_push_method(scp_routine_t* pScpRoutine,
	IMethodDesc* pMethod)
{
	pScpRoutine->MethodStack.PushBack(pMethod);
}

// Pop object method from stack.
inline void scp_routine_pop_method(scp_routine_t* pScpRoutine)
{
	pScpRoutine->MethodStack.PopBack();
}

// Get object method from stack.
inline IMethodDesc* scp_routine_get_method(scp_routine_t* pScpRoutine)
{
	return pScpRoutine->MethodStack.Back();
}

// Wait any signal.
inline bool scp_routine_has_wait_signal(scp_routine_t* pScpRoutine)
{
	return pScpRoutine->pWaitSignals != NULL;
}

// Create script coroutine.
scp_routine_t* scp_routine_create(size_t routine_id, lua_State* state,
	scp_file_t* pScpFile, const char* func);
// Delete script coroutine.
void scp_routine_delete(scp_routine_t* pScpRoutine);
// Get script file name.
const char* scp_routine_get_script(scp_routine_t* pScpRoutine);
// Set object id.
void scp_routine_set_self(scp_routine_t* pScpRoutine, const uniqid_t& value);
// Get number of wait signals.
size_t scp_routine_wait_signal_count(scp_routine_t* pScpRoutine);
// Add wait signal.
bool scp_routine_add_wait_signal(scp_routine_t* pScpRoutine, 
	const uniqid_t& id, const char* event);
// Clear wait signals.
void scp_routine_clear_wait_signal(scp_routine_t* pScpRoutine);
// Find wait signal name.
bool scp_routine_find_wait_signal(scp_routine_t* pScpRoutine, 
	const char* signal);
// Get wait signal object id.
uniqid_t scp_routine_wait_signal_object(scp_routine_t* pScpRoutine, 
	const char* signal);
// Start coroutine.
bool scp_routine_start(scp_routine_t* pScpRoutine, const IArgList& args);
// Resume coroutine.
bool scp_routine_resume(scp_routine_t* pScpRoutine, const char* signal,
	const IArgList* args);
// Pause coroutine.
bool scp_routine_pause(scp_routine_t* pScpRoutine, double timeout, 
	bool return_elapse);
// Stop coroutine.
bool scp_routine_stop(scp_routine_t* pScpRoutine);
// Timing check coroutine.
bool scp_routine_check(scp_routine_t* pScpRoutine, double elapse);

// Script coroutine cache.

// Create script coroutine cache.
scp_cache_t* scp_cache_create();
// Delete script coroutine cache.
void scp_cache_delete(scp_cache_t* pScpCache);

// Get coroutine from cache.
scp_routine_t* scp_cache_new_routine(scp_cache_t* pScpCache, size_t routine_id,
	lua_State* state, scp_file_t* script, const char* func);
// Release coroutine to cache.
void scp_cache_delete_routine(scp_cache_t* pScpCache, scp_routine_t* pData);

#endif // __SCRIPT_ROUTINE_H
