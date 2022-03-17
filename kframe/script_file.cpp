// script_file.cpp
// Created by lulimin on 2019/2/17.

#include "script_file.h"
#include "script_utility.h"
#include "frame_inner_mem.h"
#include "../inc/auto_string.h"
#include "../inc/pod_array.h"

DEFINE_INNER_ALLOC(ScriptFile);

struct scp_file_t
{
	lua_State* pState;
	TAutoString<char, 128, TCharTraits<char>, ScriptFileAlloc> sPath;
	TAutoString<char, 16, TCharTraits<char>, ScriptFileAlloc> sName;
	TPodArray<char, 4, ScriptFileAlloc> RoutineIdMap;
	TPodArray<size_t, 4, ScriptFileAlloc> RoutineIdFree;
	IFileAccess::file_time_t ScriptTime;
	int nRefCount;
};

scp_file_t* scp_file_create(lua_State* state, const char* path,
	const char* name)
{
	Assert(state != NULL);
	Assert(path != NULL);
	Assert(name != NULL);

	scp_file_t* pScpFile = K_INNER_NEW(scp_file_t);

	pScpFile->pState = state;
	pScpFile->sPath = path;
	pScpFile->sName = name;
	pScpFile->nRefCount = 0;
	memset(&pScpFile->ScriptTime, 0, sizeof(pScpFile->ScriptTime));
	return pScpFile;
}

void scp_file_delete(scp_file_t* pScpFile)
{
	Assert(pScpFile != NULL);

	K_INNER_DELETE(pScpFile);
}

void scp_file_set_state(scp_file_t* pScpFile, lua_State* state)
{
	Assert(pScpFile != NULL);

	pScpFile->pState = state;
}

lua_State* scp_file_get_state(scp_file_t* pScpFile)
{ 
	Assert(pScpFile != NULL);

	return pScpFile->pState;
}

const char* scp_file_get_path(scp_file_t* pScpFile)
{ 
	Assert(pScpFile != NULL);

	return pScpFile->sPath.CString();
}

const char* scp_file_get_name(scp_file_t* pScpFile)
{ 
	Assert(pScpFile != NULL);

	return pScpFile->sName.CString();
}

void scp_file_set_time(scp_file_t* pScpFile, 
	const IFileAccess::file_time_t& value)
{ 
	Assert(pScpFile != NULL);

	pScpFile->ScriptTime = value;
}

const IFileAccess::file_time_t& scp_file_get_time(scp_file_t* pScpFile)
{ 
	Assert(pScpFile != NULL);

	return pScpFile->ScriptTime;
}

bool scp_file_find_function(scp_file_t* pScpFile, const char* name)
{
	Assert(pScpFile != NULL);
	Assert(name != NULL);
	
	lua_getglobal(pScpFile->pState, name);

	bool result = lua_isfunction(pScpFile->pState, -1);

	lua_pop(pScpFile->pState, 1);
	return result;
}

void scp_file_inc_ref_count(scp_file_t* pScpFile)
{
	Assert(pScpFile != NULL);

	pScpFile->nRefCount++;
}

void scp_file_dec_ref_count(scp_file_t* pScpFile)
{
	Assert(pScpFile != NULL);

	pScpFile->nRefCount--;
}

int scp_file_get_ref_count(scp_file_t* pScpFile)
{
	Assert(pScpFile != NULL);

	return pScpFile->nRefCount;
}

lua_State* scp_file_new_routine(scp_file_t* pScpFile, size_t* pRoutineId)
{
	Assert(pScpFile != NULL);
	Assert(pRoutineId != NULL);

	size_t index;
	
	if (!pScpFile->RoutineIdFree.Empty())
	{
		index = pScpFile->RoutineIdFree.Back();
		
		Assert(index < pScpFile->RoutineIdMap.Size());
		Assert(0 == pScpFile->RoutineIdMap[index]);
		
		pScpFile->RoutineIdMap[index] = 1;
		pScpFile->RoutineIdFree.PopBack();
	}
	else
	{
		index = pScpFile->RoutineIdMap.Size();
		pScpFile->RoutineIdMap.PushBack(1);
	}

	*pRoutineId = index;
	return scp_util_new_thread(pScpFile->pState, index);
}

bool scp_file_delete_routine(scp_file_t* pScpFile, size_t routine_id)
{
	Assert(pScpFile != NULL);

	scp_util_delete_thread(pScpFile->pState, routine_id);
	
	Assert(routine_id < pScpFile->RoutineIdMap.Size());
	
	if ((routine_id < pScpFile->RoutineIdMap.Size()) && 
		(pScpFile->RoutineIdMap[routine_id] == 1))
	{
		pScpFile->RoutineIdMap[routine_id] = 0;
		pScpFile->RoutineIdFree.PushBack(routine_id);
	}

	return true;
}
