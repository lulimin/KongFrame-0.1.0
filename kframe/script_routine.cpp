// script_routine.cpp 
// Created by lulimin on 2019/2/17.

#include "script_routine.h"
#include "script_utility.h"
#include "script_file.h"
#include "frame_imp.h"
#include "../inc/platform.h"
#include "../inc/arg_list.h"

typedef TArgList<8, 128, FrameInnerAlloc> InnerArgList;

void scp_routine_construct(scp_routine_t* pScpRoutine, size_t routine_id,
	lua_State* state, scp_file_t* pScpFile, const char* func)
{
	Assert(pScpRoutine != NULL);
	Assert(state != NULL);
	Assert(pScpFile != NULL);
	Assert(func != NULL);

	new (&pScpRoutine->sFunction) ScpRoutineString(func);
	new (&pScpRoutine->MethodStack) ScpRoutineMethod();
	pScpRoutine->nRoutineId = routine_id;
	pScpRoutine->pState = state;
	pScpRoutine->pScpFile = pScpFile;
	pScpRoutine->nProgress = 0;
	pScpRoutine->bReturnElapse = false;
	pScpRoutine->dTimeout = 0.0;
	pScpRoutine->dElapse = 0.0;
	pScpRoutine->pWaitSignals = NULL;
	scp_file_inc_ref_count(pScpFile);
}

scp_routine_t* scp_routine_create(size_t routine_id, lua_State* state,
	scp_file_t* pScpFile, const char* func)
{
	Assert(state != NULL);
	Assert(pScpFile != NULL);
	Assert(func != NULL);

	scp_routine_t* pScpRoutine = (scp_routine_t*)K_INNER_ALLOC(
		sizeof(scp_routine_t));

	scp_routine_construct(pScpRoutine, routine_id, state, pScpFile, func);
	return pScpRoutine;
}

void scp_routine_destruct(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);

	scp_file_t* pScpFile = pScpRoutine->pScpFile;
	
	scp_file_dec_ref_count(pScpFile);
	scp_file_delete_routine(pScpFile, pScpRoutine->nRoutineId);

	if (pScpRoutine->pWaitSignals)
	{
		K_INNER_DELETE(pScpRoutine->pWaitSignals);
	}

	pScpRoutine->sFunction.~ScpRoutineString();
	pScpRoutine->MethodStack.~ScpRoutineMethod();
}

void scp_routine_delete(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);
	
	scp_routine_destruct(pScpRoutine);
	K_INNER_FREE(pScpRoutine);
}

const char* scp_routine_get_script(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);

	return scp_file_get_name(pScpRoutine->pScpFile);
}

void scp_routine_set_self(scp_routine_t* pScpRoutine, const uniqid_t& value)
{
	Assert(pScpRoutine != NULL);

	pScpRoutine->Self = value;
}

size_t scp_routine_wait_signal_count(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);

	if (NULL == pScpRoutine->pWaitSignals)
	{
		return 0;
	}

	return pScpRoutine->pWaitSignals->GetCount();
}

bool scp_routine_add_wait_signal(scp_routine_t* pScpRoutine, 
	const uniqid_t& id, const char* event)
{
	Assert(pScpRoutine != NULL);
	Assert(event != NULL);

	if (NULL == pScpRoutine->pWaitSignals)
	{
		pScpRoutine->pWaitSignals = K_INNER_NEW(ScpRoutineSignals);
	}

	pScpRoutine->pWaitSignals->Add(event, id);
	return true;
}

void scp_routine_clear_wait_signal(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);

	if (pScpRoutine->pWaitSignals)
	{
		K_INNER_DELETE(pScpRoutine->pWaitSignals);
		pScpRoutine->pWaitSignals = NULL;
	}
}

bool scp_routine_find_wait_signal(scp_routine_t* pScpRoutine,
	const char* event)
{
	Assert(pScpRoutine != NULL);
	Assert(event != NULL);

	if (NULL == pScpRoutine->pWaitSignals)
	{
		return false;
	}

	return pScpRoutine->pWaitSignals->Exists(event);
}

uniqid_t scp_routine_wait_signal_object(scp_routine_t* pScpRoutine, 
	const char* event)
{
	Assert(pScpRoutine != NULL);
	Assert(event != NULL);

	if (NULL == pScpRoutine->pWaitSignals)
	{
		return uniqid_t();
	}

	uniqid_t id;

	if (!pScpRoutine->pWaitSignals->GetData(event, &id))
	{
		return uniqid_t();
	}

	return id;
}

bool scp_routine_start(scp_routine_t* pScpRoutine, const IArgList& args)
{
	Assert(pScpRoutine != NULL);

	pScpRoutine->nProgress = SCP_ROUTINE_START;
	lua_getglobal(pScpRoutine->pState, pScpRoutine->sFunction.CString());

	size_t max_arg_num = args.GetCount() + 1;

	if (max_arg_num > 16)
	{
		// Guarantee enough script stack space.
		if (!lua_checkstack(pScpRoutine->pState, (int)(max_arg_num + 1)))
		{
			pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;

			char info[256];

			plat_safe_sprintf(info, sizeof(info),
				"(scp_routine_start)Script %s function %s check stack failed.",
				scp_file_get_name(pScpRoutine->pScpFile), 
				pScpRoutine->sFunction.CString());
			frame_log_string(FRAME_DEFAULT_LOG, info);
			return false;
		}
	}

	int arg_num = 0;

	if (uniqid_not_null(pScpRoutine->Self))
	{
		arg_num += scp_util_push_uniqid(pScpRoutine->pState, pScpRoutine->Self);
	}
	else
	{
		// Try get object id.
		pScpRoutine->Self = args.GetUniqid(0);
	}

	arg_num += scp_util_push_arg_list(pScpRoutine->pState, args);

	int script_profile = frame_get_script_profile();
	int res;

	if (script_profile > 0)
	{
		double beg_time = plat_get_time_count();

		// lua 5.1
		//res = lua_resume(m_pState, arg_num);
		// lua 5.3
		res = lua_resume(pScpRoutine->pState, NULL, arg_num);

		double run_time = plat_get_time_count() - beg_time;

		frame_add_script_seconds(run_time);

		if (script_profile > 1)
		{
			char info[256];

			plat_safe_sprintf(info, sizeof(info),
				"Script start use %f seconds in file:%s function:%s.",
				run_time, scp_routine_get_script(pScpRoutine),
				scp_routine_get_function(pScpRoutine));
			frame_log_string(FRAME_DEFAULT_LOG, info);
		}
	}
	else
	{
		// lua 5.1
		//res = lua_resume(m_pState, arg_num);
		// lua 5.3
		res = lua_resume(pScpRoutine->pState, NULL, arg_num);
	}

	// lua 5.1
	if (res == LUA_YIELD)
	{
		// Paused.
		pScpRoutine->nProgress = SCP_ROUTINE_PAUSE;
		return true;
	}

	if (res != 0)
	{
		scp_util_run_error(pScpRoutine->pState, 
			scp_routine_get_script(pScpRoutine), 
			scp_routine_get_function(pScpRoutine), res);
		pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;
		return false;
	}

	if (pScpRoutine->nProgress != SCP_ROUTINE_PAUSE)
	{
		pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;
	}

	return true;
}

bool scp_routine_resume(scp_routine_t* pScpRoutine, const char* signal,
	const IArgList* args)
{
	Assert(pScpRoutine != NULL);

	// Clear current wait signals.
	scp_routine_clear_wait_signal(pScpRoutine);
	// Restart script.
	pScpRoutine->nProgress = SCP_ROUTINE_START;

	int arg_num = 0;

	// Wake signal name.
	if (signal != NULL)
	{
		lua_pushstring(pScpRoutine->pState, signal);
		arg_num++;
	}

	// Arguments.
	if (args != NULL)
	{
		arg_num += scp_util_push_arg_list(pScpRoutine->pState, *args);
	}

	int script_profile = frame_get_script_profile();
	int res;

	if (script_profile > 0)
	{
		double beg_time = plat_get_time_count();

		// lua 5.1
		//res = lua_resume(m_pState, arg_num);
		// lua 5.3
		res = lua_resume(pScpRoutine->pState, NULL, arg_num);

		double run_time = plat_get_time_count() - beg_time;

		frame_add_script_seconds(run_time);

		if (script_profile > 1)
		{
			char info[256];

			plat_safe_sprintf(info, sizeof(info),
				"Script resume use %f seconds in file:%s function:%s.",
				run_time, scp_routine_get_script(pScpRoutine),
				scp_routine_get_function(pScpRoutine));
			frame_log_string(FRAME_DEFAULT_LOG, info);
		}
	}
	else
	{
		// lua 5.1
		//res = lua_resume(m_pState, arg_num);
		// lua 5.3
		res = lua_resume(pScpRoutine->pState, NULL, arg_num);
	}

	// lua 5.1
	if (res == LUA_YIELD)
	{
		// Paused.
		pScpRoutine->nProgress = SCP_ROUTINE_PAUSE;
		return true;
	}

	if (res != 0)
	{
		scp_util_run_error(pScpRoutine->pState, 
			scp_routine_get_script(pScpRoutine),
			scp_routine_get_function(pScpRoutine), res);
		pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;
		return false;
	}

	if (pScpRoutine->nProgress != SCP_ROUTINE_PAUSE)
	{
		pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;
	}

	return true;
}

bool scp_routine_pause(scp_routine_t* pScpRoutine, double timeout, 
	bool return_elapse)
{
	Assert(pScpRoutine != NULL);
	Assert(pScpRoutine->nProgress == SCP_ROUTINE_START);

	pScpRoutine->dTimeout = timeout;
	pScpRoutine->dElapse = 0.0;
	pScpRoutine->bReturnElapse = return_elapse;
	pScpRoutine->nProgress = SCP_ROUTINE_PAUSE;
	return true;
}

bool scp_routine_stop(scp_routine_t* pScpRoutine)
{
	Assert(pScpRoutine != NULL);

	pScpRoutine->nProgress = SCP_ROUTINE_STOPPED;
	return true;
}

bool scp_routine_check(scp_routine_t* pScpRoutine, double elapse)
{
	Assert(pScpRoutine != NULL);

	pScpRoutine->dElapse += elapse;

	if (pScpRoutine->dTimeout < 0.0)
	{
		// Waiting forever.
		return false;
	}
	
	if (pScpRoutine->dElapse >= pScpRoutine->dTimeout)
	{
		if (pScpRoutine->bReturnElapse)
		{
			// Return elapse time.
			InnerArgList args;

			args.AddDouble(pScpRoutine->dElapse);
			return scp_routine_resume(pScpRoutine, NULL, &args);
		}
		else
		{
			return scp_routine_resume(pScpRoutine, NULL, NULL);
		}
	}

	return true;
}

// Script coroutine cache.

struct scp_cache_node_t
{
	scp_routine_t RoutineData;
	scp_cache_node_t* pNext;
};

struct scp_cache_t
{
	scp_cache_node_t* pFreeList;
};

scp_cache_t* scp_cache_create()
{
	scp_cache_t* pScpCache = K_INNER_NEW(scp_cache_t);
	
	pScpCache->pFreeList = NULL;
	return pScpCache;
}

void scp_cache_delete(scp_cache_t* pScpCache)
{
	Assert(pScpCache != NULL);

	scp_cache_node_t* p = pScpCache->pFreeList;

	while (p)
	{
		scp_cache_node_t* t = p;

		p = p->pNext;
		K_INNER_FREE(t);
	}
	
	K_INNER_DELETE(pScpCache);
}

scp_routine_t* scp_cache_new_routine(scp_cache_t* pScpCache, size_t routine_id,
	lua_State* state, scp_file_t* script, const char* func)
{
	Assert(pScpCache != NULL);

	if (pScpCache->pFreeList)
	{
		scp_cache_node_t* p = pScpCache->pFreeList;

		pScpCache->pFreeList = p->pNext;
		scp_routine_construct(&p->RoutineData, routine_id, state, script,
			func);
		return &p->RoutineData;
	}

	scp_cache_node_t* p = (scp_cache_node_t*)K_INNER_ALLOC(
		sizeof(scp_cache_node_t));

	scp_routine_construct(&p->RoutineData, routine_id, state, script, func);
	p->pNext = NULL;
	return &p->RoutineData;
}

void scp_cache_delete_routine(scp_cache_t* pScpCache, scp_routine_t* pData)
{
	Assert(pScpCache != NULL);

	scp_cache_node_t* p = (scp_cache_node_t*)pData;

	scp_routine_destruct(&p->RoutineData);
	p->pNext = pScpCache->pFreeList;
	pScpCache->pFreeList = p;
}
