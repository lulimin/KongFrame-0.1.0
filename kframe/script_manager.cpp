// script_manager.cpp
// Created by lulimin on 2019/2/18.

#include <new>
#include "script_manager.h"
#include "script_file.h"
#include "script_utility.h"
#include "script_extend.h"
#include "script_meta.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "../inc/literal_hash_table.h"
#include "../inc/platform.h"

DEFINE_INNER_ALLOC(ScriptManager);

typedef TLiteralHashTable<char, scp_file_t*, TCharTraitsCI<char>,
	ScriptManagerAlloc> ScriptContainer;

struct scp_manager_t
{
	ScriptContainer Scripts;
};

scp_manager_t* scp_manager_create()
{
	return K_INNER_NEW(scp_manager_t);
}

void scp_manager_delete(scp_manager_t* pScpManager)
{
	Assert(pScpManager != NULL);

	ScriptContainer::iterator it = pScpManager->Scripts.Begin();

	for (; it != pScpManager->Scripts.End(); ++it)
	{
		scp_file_t* pScript = it.GetData();

		scp_util_close_state(scp_file_get_state(pScript));
		scp_file_delete(pScript);
	}
	
	K_INNER_DELETE(pScpManager);
}

// Generate script file name.
static const char* generate_file_name(char* buf, size_t size, const char* path,
	const char* name)
{
	if (strchr(name, '.') == NULL)
	{
		// Add extension name.
		plat_safe_sprintf(buf, size, "%s%s.lua", path, name);
	}
	else
	{
		plat_safe_sprintf(buf, size, "%s%s", path, name);
	}

	return buf;
}

scp_file_t* scp_manager_add_script(scp_manager_t* pScpManager, 
	const char* path, const char* name)
{
	Assert(pScpManager != NULL);
	Assert(path != NULL);
	Assert(name != NULL);

	char fname[256];

	generate_file_name(fname, sizeof(fname), path, name);

	lua_State* state = scp_util_open_state();

	// lua 5.1
	//scp_util_set_search_path(state, path);
	// lua 5.3
	scp_util_set_searcher(state);

	if (!scp_util_load_file(state, fname))
	{
		scp_util_close_state(state);
		return NULL;
	}

	scp_file_t* pScpFile = scp_file_create(state, path, name);

	if (!pScpManager->Scripts.Add(scp_file_get_name(pScpFile), pScpFile))
	{
		scp_util_close_state(state);
		scp_file_delete(pScpFile);
		return NULL;
	}

	IFileAccess::file_time_t ft;

	frame_file_access()->FileTime(fname, &ft);
	scp_file_set_time(pScpFile, ft);
	scp_add_meta_table(state);
	register_extend_functions(state);
	return pScpFile;
}

scp_file_t* scp_manager_find_script(scp_manager_t* pScpManager, 
	const char* name)
{
	Assert(pScpManager != NULL);
	Assert(name != NULL);

	scp_file_t* pScpFile;

	if (!pScpManager->Scripts.GetData(name, &pScpFile))
	{
		return NULL;
	}

	return pScpFile;
}

bool scp_manager_reload_script(scp_manager_t* pScpManager, scp_file_t* pScpFile)
{
	Assert(pScpManager != NULL);
	Assert(pScpFile != NULL);

	if (scp_file_get_ref_count(pScpFile) != 0)
	{
		return true; // Reference exists.
	}

	char fname[256];

	generate_file_name(fname, sizeof(fname), scp_file_get_path(pScpFile),
		scp_file_get_name(pScpFile));

	IFileAccess::file_time_t ft;

	frame_file_access()->FileTime(fname, &ft);

	if (memcmp(&ft, &scp_file_get_time(pScpFile), sizeof(ft)) == 0)
	{
		return true; // File time no change.
	}

	// lua 5.1
	//scp_util_set_search_path(pScript->GetState(), pScript->GetPath());
	// lua 5.3
	scp_util_set_searcher(scp_file_get_state(pScpFile));

	if (!scp_util_load_file(scp_file_get_state(pScpFile), fname))
	{
		return false;
	}

	scp_file_set_time(pScpFile, ft);
	return true;
}

bool scp_manager_reload_all_script(scp_manager_t* pScpManager)
{
	Assert(pScpManager != NULL);

	ScriptContainer::iterator it = pScpManager->Scripts.Begin();
	ScriptContainer::iterator end = pScpManager->Scripts.End();

	for (; it != end; ++it)
	{
		scp_file_t* pScpFile = it.GetData();

		if (scp_file_get_ref_count(pScpFile) != 0)
		{
			continue; // Reference exists.
		}

		char fname[256];

		generate_file_name(fname, sizeof(fname), scp_file_get_path(pScpFile),
			scp_file_get_name(pScpFile));

		lua_State* state = scp_util_open_state();

		// lua 5.1
		//scp_util_set_search_path(state, pScript->GetPath());
		// lua 5.3
		scp_util_set_searcher(state);

		if (!scp_util_load_file(state, fname))
		{
			scp_util_close_state(state);
			continue;
		}

		scp_add_meta_table(state);
		register_extend_functions(state);
		scp_util_close_state(scp_file_get_state(pScpFile));
		scp_file_set_state(pScpFile, state);
	}

	return true;
}

bool scp_manager_dump(scp_manager_t* pScpManager, const char* name)
{
	Assert(pScpManager != NULL);
	Assert(name != NULL);

	FILE* fp = plat_file_open(name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	ScriptContainer::iterator it = pScpManager->Scripts.Begin();
	ScriptContainer::iterator end = pScpManager->Scripts.End();

	for (; it != end; ++it)
	{
		scp_file_t* pScpFile = it.GetData();

		fprintf(fp, "Script file:%s%s reference count:%d.\r\n",
			scp_file_get_path(pScpFile), scp_file_get_name(pScpFile),
			scp_file_get_ref_count(pScpFile));
	}

	fprintf(fp, "Total %zd script files.\r\n", 
		pScpManager->Scripts.GetCount());
	fclose(fp);
	return true;
}
