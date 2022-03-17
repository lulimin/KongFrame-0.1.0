// frame_imp.cpp
// Created by lulimin on 2019/2/18.

#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "frame_services.h"
#include "frame_objects.h"
#include "frame_factory.h"
#include "frame_file_access.h"
#include "frame_object_desc.h"
#include "frame_object_hook.h"
#include "frame_data_table.h"
#include "script_manager.h"
#include "script_pool.h"
#include "script_file.h"
#include "script_utility.h"
#include "script_routine.h"
#include "../inc/arg_list.h"
#include "../inc/utility.h"
#include "../inc/platform.h"
#include "../inc/thread_lock.h"
#include "../inc/i_service_creator.h"
#include "../inc/i_object.h"
#include "../inc/i_object_creator.h"

DEFINE_INNER_ALLOC(GlobalTable);
DEFINE_INNER_ALLOC(MethodTable);

typedef TArgList<8, 128, FrameInnerAlloc> InnerArgList;
typedef TPodArray<IMethodDesc*, 4, MethodTableAlloc> MethodTable;

// Frame data.
struct frame_imp_t
{
	// Memory management function pointers.
	frame_debug_alloc_func_t pDebugAllocFunc;
	frame_alloc_func_t pAllocFunc;
	frame_realloc_func_t pReallocFunc;
	frame_free_func_t pFreeFunc;
	frame_dump_func_t pDumpFunc;
	// System in closing status.
	bool bClosing;
	// System exit flag.
	bool bExit;
	// Reload script file while changed.
	bool bScriptReload;
	// Profile script performance.
	int nScriptProfile;
	// Current frame time.
	double dPeriodSeconds;
	// Total frame time.
	double dTotalSeconds;
	// Script execute time.
	double dScriptSeconds;
	// Current work path.
	char* pWorkPath;
	// Scirpt file path.
	char* pScriptPath;
	// Coder file path.
	char* pCoderPath;
	// Asset file path.
	char* pAssetPath;
	// Output file path.
	char* pOutputPath;
	// Log level.
	int nLogLevel;
	// Log redirection function pointer.
	frame_log_func_t pLogFunc;
	// Log redirection context pointer.
	void* pLogContext;
	// Log lock.
	ThreadLock* pLogLock;
	// Global data.
	IDataTable* pGlobals;
	// Main object.
	IObject* pMainObject;
	// File access interface.
	IFileAccess* pFileAccess;
	// Script management.
	scp_manager_t* pScriptManager;
	// Script coroutine management.
	scp_pool_t* pScriptRoutinePool;
	// Script coroutine list.
	scp_cache_t* pScriptRoutineCache;
	// Service management.
	frame_services_t* pServiceManager;
	// Object management.
	frame_objects_t* pObjectManager;
	// Object factory.
	frame_factory_t* pObjectFactory;
	// Global object method stack.
	MethodTable* pMethodStack;
};

#ifdef K_MULTI_FRAME
#include "../inc/thread.h"
static plat_thread_key_t s_FrameKey; // Frame thread key.
#else
static frame_imp_t s_FrameImp; // Frame implement data.
#endif // K_MULTI_FRAME

// Get Pointer of frame data.
static inline frame_imp_t* get_frame_imp()
{
#ifdef K_MULTI_FRAME
	return (frame_imp_t*)plat_thread_key_get(&s_FrameKey);
#else
	return &s_FrameImp;
#endif // K_MULTI_FRAME
}

// Get system in closing.
bool frame_get_closing()
{
	return get_frame_imp()->bClosing;
}

// Get flag of profile script.
int frame_get_script_profile()
{
	return get_frame_imp()->nScriptProfile;
}

// Increase script executing time.
void frame_add_script_seconds(double value)
{
	get_frame_imp()->dScriptSeconds += value;
}

// Delete script coroutine.
void frame_delete_routine(scp_routine_t* pRoutine)
{
	Assert(pRoutine != NULL);

	scp_cache_delete_routine(get_frame_imp()->pScriptRoutineCache, pRoutine);
}

// Push method to stack.
void frame_push_method(IMethodDesc* pMethod)
{
	get_frame_imp()->pMethodStack->PushBack(pMethod);
}

// Pop method from stack.
void frame_pop_method()
{
	get_frame_imp()->pMethodStack->PopBack();
}

// Get method at top of stack.
IMethodDesc* frame_get_method()
{
	return get_frame_imp()->pMethodStack->Back();
}

// Allocate string memory.
static char* alloc_string(char* old, const char* src)
{
	Assert(src != NULL);
	
	size_t size = strlen(src) + 1;
	char* dst = (char*)K_INNER_ALLOC(size);

	memcpy(dst, src, size);
	
	if (old)
	{
		K_INNER_FREE(old);
	}

	return dst;
}

// Allocate memory in debug mode.
static void* mem_debug_alloc(size_t size, const char* file, int line)
{
#if (defined(K_PLATFORM_WINDOWS) && defined(K_DEBUG))
	return _malloc_dbg(size, _NORMAL_BLOCK, file, line);
#else
	return malloc(size);
#endif // K_PLATFORM_WINDOWS && K_DEBUG
}

// Internal allocate memory in debug mode.
void* frame_inner_debug_alloc(size_t size, const char* file, int line)
{
	return get_frame_imp()->pDebugAllocFunc(size, file, line);
}

// Internal allocate memory.
void* frame_inner_alloc(size_t size)
{
	return get_frame_imp()->pAllocFunc(size);
}

// Internal free memory.
void frame_inner_free(void* ptr)
{
	get_frame_imp()->pFreeFunc(ptr);
}

FRAME_API void* frame_debug_alloc(size_t size, const char* file, int line)
{
	return get_frame_imp()->pDebugAllocFunc(size, file, line);
}

FRAME_API void* frame_alloc(size_t size)
{
	return get_frame_imp()->pAllocFunc(size);
}

FRAME_API void* frame_realloc(void* ptr, size_t new_size)
{
	return get_frame_imp()->pReallocFunc(ptr, new_size);
}

FRAME_API void frame_free(void* ptr)
{
	get_frame_imp()->pFreeFunc(ptr);
}

FRAME_API bool frame_multithread_setup()
{
#ifdef K_MULTI_FRAME
	return plat_thread_key_create(&s_FrameKey);
#else
	return false;
#endif // K_MULTI_FRAME
}

FRAME_API bool frame_multithread_clean()
{
#ifdef K_MULTI_FRAME
	return plat_thread_key_destroy(&s_FrameKey);
#else
	return false;
#endif // K_MULTI_FRAME
}

FRAME_API void frame_create(frame_debug_alloc_func_t debug_alloc_func, 
	frame_alloc_func_t alloc_func, frame_realloc_func_t realloc_func,
	frame_free_func_t free_func, frame_dump_func_t dump_func)
{
#ifdef K_MULTI_FRAME
	frame_imp_t* pFrameImp = (frame_imp_t*)malloc(sizeof(frame_imp_t));

	plat_thread_key_set(&s_FrameKey, pFrameImp);
#else
	frame_imp_t* pFrameImp = get_frame_imp();
#endif // K_MULTI_FRAME

	memset(pFrameImp, 0, sizeof(frame_imp_t));
	
	if (NULL == debug_alloc_func)
	{
		pFrameImp->pDebugAllocFunc = mem_debug_alloc;
	}
	else
	{
		pFrameImp->pDebugAllocFunc = debug_alloc_func;
	}

	if (NULL == alloc_func)
	{
		pFrameImp->pAllocFunc = malloc;
	}
	else
	{
		pFrameImp->pAllocFunc = alloc_func;
	}

	if (NULL == realloc_func)
	{
		pFrameImp->pReallocFunc = realloc;
	}
	else
	{
		pFrameImp->pReallocFunc = realloc_func;
	}

	if (NULL == free_func)
	{
		pFrameImp->pFreeFunc = free;
	}
	else
	{
		pFrameImp->pFreeFunc = free_func;
	}

	pFrameImp->pDumpFunc = dump_func;
	pFrameImp->pLogLock = K_INNER_NEW(ThreadLock);
	pFrameImp->pGlobals = K_INNER_NEW(TFrameDataTable<GlobalTableAlloc>);
	pFrameImp->pMethodStack = K_INNER_NEW(MethodTable);
	pFrameImp->pScriptManager = scp_manager_create();
	pFrameImp->pScriptRoutineCache = scp_cache_create();
	pFrameImp->pScriptRoutinePool = scp_pool_create();
	pFrameImp->pServiceManager = frame_services_create();
	pFrameImp->pObjectManager = frame_objects_create();
	pFrameImp->pObjectFactory = frame_factory_create();
}

FRAME_API void frame_delete()
{
	frame_imp_t* pFrameImp = get_frame_imp();

	frame_factory_delete(pFrameImp->pObjectFactory);
	pFrameImp->pObjectFactory = NULL;
	frame_objects_delete(pFrameImp->pObjectManager);
	pFrameImp->pObjectManager = NULL;
	frame_services_delete(pFrameImp->pServiceManager);
	pFrameImp->pServiceManager = NULL;
	scp_pool_delete(pFrameImp->pScriptRoutinePool);
	pFrameImp->pScriptRoutinePool = NULL;
	scp_cache_delete(pFrameImp->pScriptRoutineCache);
	pFrameImp->pScriptRoutineCache = NULL;
	scp_manager_delete(pFrameImp->pScriptManager);
	pFrameImp->pScriptManager = NULL;
	K_INNER_DELETE(pFrameImp->pMethodStack);
	pFrameImp->pMethodStack = NULL;
	K_INNER_DELETE(pFrameImp->pGlobals);
	pFrameImp->pGlobals = NULL;
	K_INNER_DELETE(pFrameImp->pLogLock);
	pFrameImp->pLogLock = NULL;

	if (pFrameImp->pWorkPath)
	{
		K_INNER_FREE(pFrameImp->pWorkPath);
		pFrameImp->pWorkPath = NULL;
	}

	if (pFrameImp->pScriptPath)
	{
		K_INNER_FREE(pFrameImp->pScriptPath);
		pFrameImp->pScriptPath = NULL;
	}

	if (pFrameImp->pCoderPath)
	{
		K_INNER_FREE(pFrameImp->pCoderPath);
		pFrameImp->pCoderPath = NULL;
	}

	if (pFrameImp->pAssetPath)
	{
		K_INNER_FREE(pFrameImp->pAssetPath);
		pFrameImp->pAssetPath = NULL;
	}

	if (pFrameImp->pOutputPath)
	{
		K_INNER_FREE(pFrameImp->pOutputPath);
		pFrameImp->pOutputPath = NULL;
	}

#ifdef K_MULTI_FRAME
	free(pFrameImp);
	plat_thread_key_set(&s_FrameKey, NULL);
#endif // K_MULTI_FRAME
}

FRAME_API const char* frame_get_work_path()
{
	Assert(get_frame_imp()->pWorkPath != NULL);

	return get_frame_imp()->pWorkPath;
}

FRAME_API const char* frame_get_script_path()
{
	Assert(get_frame_imp()->pScriptPath != NULL);

	return get_frame_imp()->pScriptPath;
}

FRAME_API const char* frame_get_coder_path()
{
	Assert(get_frame_imp()->pCoderPath != NULL);

	return get_frame_imp()->pCoderPath;
}

FRAME_API const char* frame_get_asset_path()
{
	Assert(get_frame_imp()->pAssetPath != NULL);

	return get_frame_imp()->pAssetPath;
}

FRAME_API const char* frame_get_output_path()
{
	Assert(get_frame_imp()->pOutputPath != NULL);

	return get_frame_imp()->pOutputPath;
}

FRAME_API double frame_get_period_seconds()
{ 
	return get_frame_imp()->dPeriodSeconds;
}

FRAME_API double frame_get_total_seconds()
{ 
	return get_frame_imp()->dTotalSeconds;
}

FRAME_API double frame_get_script_seconds()
{
	return get_frame_imp()->dScriptSeconds;
}

FRAME_API bool frame_set_log_level(int level)
{
	get_frame_imp()->nLogLevel = level;
	return true;
}

FRAME_API int frame_get_log_level()
{
	return get_frame_imp()->nLogLevel;
}

FRAME_API bool frame_set_log_func(frame_log_func_t func, void* context)
{
	frame_imp_t* pFrameImp = get_frame_imp();
	
	pFrameImp->pLogFunc = func;
	pFrameImp->pLogContext = context;
	return true;
}

FRAME_API bool frame_clear_log()
{
	FILE* fp = plat_file_open("frame.log", "wb");

	if (NULL == fp)
	{
		return false;
	}

	fclose(fp);
	return true;
}

FRAME_API bool frame_log_string(int level, const char* log)
{
	Assert(log != NULL);

	frame_imp_t* pFrameImp = get_frame_imp();
	
	if (pFrameImp->pLogFunc)
	{
		return pFrameImp->pLogFunc(pFrameImp->pLogContext, level, log);
	}

	if (pFrameImp->nLogLevel != 0)
	{
		if ((pFrameImp->nLogLevel & level) == 0)
		{
			return false;
		}
	}

	ScopedLock scoped_lock(pFrameImp->pLogLock);

	FILE* fp = plat_file_open("frame.log", "ab");

	if (NULL == fp)
	{
		return false;
	}

	plat_date_time_t dt;
	char buffer[128];

	plat_get_local_time(&dt);
	plat_safe_sprintf(buffer, sizeof(buffer), 
		"(%04d-%02d-%02d %02d:%02d:%02d.%03d)[%d]", dt.nYear, dt.nMonth,
		dt.nDay, dt.nHour, dt.nMinute, dt.nSecond, dt.nMilliseconds, level);
	fprintf(fp, "%s%s\r\n", buffer, log);
	fclose(fp);
	return true;
}

FRAME_API bool __cdecl frame_log_print(int level, const char* log, ...)
{
	va_list	args;
	char buf[256];

	va_start(args, log);
	plat_safe_sprint_list(buf, sizeof(buf), log, args);
	va_end(args);
	return frame_log_string(level, buf);
}

FRAME_API void frame_set_exit(bool value)
{
	get_frame_imp()->bExit = value;
}

FRAME_API bool frame_get_exit()
{
	return get_frame_imp()->bExit;
}

FRAME_API bool frame_dump_information(const char* type, const char* file)
{
	Assert(type != NULL);
	Assert(file != NULL);

	frame_imp_t* pFrameImp = get_frame_imp();
	
	if (stricmp(type, "script") == 0)
	{
		// Dump script files.
		return scp_manager_dump(pFrameImp->pScriptManager, file);
	}
	else if (stricmp(type, "routine") == 0)
	{
		// Dump script coroutines.
		return scp_pool_dump(pFrameImp->pScriptRoutinePool, file);
	}
	else if (stricmp(type, "object_desc") == 0)
	{
		// Dump object descriptions.
		return frame_objects_dump(pFrameImp->pObjectManager, file);
	}
	else if (stricmp(type, "object") == 0)
	{
		// Dump object list.
		return frame_factory_dump(pFrameImp->pObjectFactory, file);
	}
	else if (stricmp(type, "service") == 0)
	{
		// Dump service list.
		return frame_services_dump(pFrameImp->pServiceManager, file);
	}
	else if (stricmp(type, "memory") == 0)
	{
		if (pFrameImp->pDumpFunc)
		{
			return pFrameImp->pDumpFunc(file);
		}
	}

	return false;
}

IFileAccess* frame_file_access()
{
	return get_frame_imp()->pFileAccess;
}

FRAME_API IFileAccess* frame_get_file_access()
{
	return get_frame_imp()->pFileAccess;
}

FRAME_API bool frame_find_service(const char* name)
{
	Assert(name != NULL);
	
	return frame_services_find(get_frame_imp()->pServiceManager, name);
}

FRAME_API IService* frame_get_service(const char* name)
{
	Assert(name != NULL);

	return frame_services_get_reference(get_frame_imp()->pServiceManager, name);
}

FRAME_API void frame_release_service(IService* pService)
{
	frame_services_release_reference(get_frame_imp()->pServiceManager,
		pService);
}

FRAME_API IObjectDesc* frame_get_object_desc(const char* name)
{
	Assert(name != NULL);
	
	return frame_objects_get_desc(get_frame_imp()->pObjectManager, name);
}

FRAME_API IObject* frame_get_main_object()
{
	return get_frame_imp()->pMainObject;
}

FRAME_API IObject* frame_get_object(const uniqid_t& id)
{
	return frame_factory_get(get_frame_imp()->pObjectFactory, id);
}

FRAME_API void frame_get_all_object(IArgList* result)
{
	frame_factory_get_all(get_frame_imp()->pObjectFactory, result);
}

FRAME_API IObject* frame_find_object(const char* name)
{
	Assert(name != NULL);
	
	return frame_factory_find(get_frame_imp()->pObjectFactory, name);
}

FRAME_API void frame_find_object_list(const char* name, IArgList* result)
{
	Assert(name != NULL);
	
	frame_factory_find_list(get_frame_imp()->pObjectFactory, name, result);
}

FRAME_API IObject* frame_create_object(const char* name)
{
	Assert(name != NULL);
	
	return frame_factory_create_object(get_frame_imp()->pObjectFactory, name,
		InnerArgList(), 0);
}

FRAME_API IObject* frame_create_object_args(const char* name,
	const IArgList& args)
{
	Assert(name != NULL);
	
	return frame_factory_create_object(get_frame_imp()->pObjectFactory, name,
		args, 0);
}

FRAME_API void frame_create_extra(IObject* pObj)
{
	Assert(pObj != NULL);

	frame_factory_create_extra(get_frame_imp()->pObjectFactory, pObj);
}

IObject* frame_script_create_object(const char* name, const IArgList& args)
{
	Assert(name != NULL);

	return frame_factory_create_object(get_frame_imp()->pObjectFactory, name,
		args, 1);
}

FRAME_API bool frame_delete_object(const uniqid_t& id)
{
	frame_factory_t* pFactory = get_frame_imp()->pObjectFactory;
	IObject* pObj = frame_factory_get(pFactory, id);

	if (NULL == pObj)
	{
		return false;
	}

	frame_factory_set_deleted(pFactory, pObj);
	return true;
}

typedef void (*read_func_t)(IObject*, IArgData*);

FRAME_API bool frame_get_property(IObject* pObj, const char* prop,
	IArgData* value)
{
	Assert(pObj != NULL);
	Assert(prop != NULL);
	Assert(value != NULL);

	IPropertyDesc* pPropertyDesc = pObj->GetDesc()->FindPropertyDesc(prop);
	
	if (NULL == pPropertyDesc)
	{
		return false;
	}
	
	read_func_t read_func = (read_func_t)pPropertyDesc->GetReadFunc();

	if (NULL == read_func)
	{
		return false;
	}
	
	read_func(pObj, value);
	return true;
}

typedef void (*write_func_t)(IObject*, const IArgData&);

FRAME_API bool frame_set_property(IObject* pObj, const char* prop, 
	const IArgData& value)
{
	Assert(pObj != NULL);
	Assert(prop != NULL);
	
	IPropertyDesc* pPropertyDesc = pObj->GetDesc()->FindPropertyDesc(prop);
	
	if (NULL == pPropertyDesc)
	{
		return false;
	}
	
	write_func_t write_func = (write_func_t)pPropertyDesc->GetWriteFunc();
	
	if (NULL == write_func)
	{
		return false;
	}
	
	write_func(pObj, value);
	return true;
}

typedef bool (*method_func_t)(IObject*, const IArgList&, IArgList*);

FRAME_API bool frame_run_method(IObject* pObj, const char* func, 
	const IArgList& args, IArgList* result)
{
	Assert(pObj != NULL);
	Assert(func != NULL);
	Assert(result != NULL);

	IMethodDesc* pMethodDesc = pObj->GetDesc()->FindMethodDesc(func);

	if (NULL == pMethodDesc)
	{
		return false;
	}

	method_func_t method_func = (method_func_t)pMethodDesc->GetMethodFunc();

	if (NULL == method_func)
	{
		return false;
	}

	result->Clear();
	return method_func(pObj, args, result);
}

//FRAME_API bool frame_run_script(const char* script, const char* func,
//	const IArgList& args, IArgList* res)
//{
//	Assert(script != NULL);
//	Assert(func != NULL);
//	
//	ScriptFile* pScript = s_pSyncScriptManager->FindScript(script);
//	
//	if (NULL == pScript)
//	{
//		pScript = s_pSyncScriptManager->AddScript(frame_get_script_path(),
//			script);
//		
//		if (NULL == pScript)
//		{
//			frame_log_print("(frame_run_script)Load script %s function %s failed.", 
//				script, func);
//			return false;
//		}
//		
//		scp_util_set_context(pScript->GetState(), NULL);
//	} 
//	else if (s_bScriptReload)
//	{
//		if (!s_pSyncScriptManager->ReloadScript(pScript))
//		{
//			frame_log_print("(frame_run_script)Reload script %s function %s failed.", 
//				script, func);
//			return false;
//		}
//	}
//	
//	if (!pScript->FindFunction(func))
//	{
//		frame_log_print("(frame_run_script)Script %s function %s not found.", 
//			script, func);
//		return false;
//	}
//
//	lua_State* state = pScript->GetState();
//
//	// Set script debug.
//	if (scp_debug_get_opened())
//	{
//		scp_debug_set_hook(state);
//	}
//
//	//s_pCurrentScript = pScript;
//
//	lua_getglobal(state, func);
//	
//	size_t max_arg_num = args.GetCount();
//
//	if (max_arg_num > 16)
//	{
//		// Check script stack space.
//		if (!lua_checkstack(state, (int)(max_arg_num + 1)))
//		{
//			frame_log_print(
//				"(frame_run_script)Script %s function %s lua_checkstack failed.",
//				script, func);
//			return false;
//		}
//	}
//
//	int arg_num = scp_util_push_arg_list(state, args);
//	// Multiple return values.
//	int	ret = lua_pcall(state, arg_num, LUA_MULTRET, 0);
//	
//	if (ret != 0)
//	{
//		scp_util_run_error(state, script, func, ret);
//		lua_settop(state, 0);
//		return false;
//	}
//
//	if (res != NULL)
//	{
//		res->Clear();
//
//		int res_num = lua_gettop(state);
//		
//		scp_util_read_arg_list(state, 1, res_num, res);
//	}
//
//	lua_settop(state, 0);
//	return true;
//}

FRAME_API bool frame_find_script(const char* script, const char* func)
{
	Assert(script != NULL);
	Assert(func != NULL);
	
	scp_manager_t* pScpManager = get_frame_imp()->pScriptManager;
	scp_file_t* pScpFile = scp_manager_find_script(pScpManager, script);

	if (NULL == pScpFile)
	{
		pScpFile = scp_manager_add_script(pScpManager, 
			frame_get_script_path(), script);

		if (NULL == pScpFile)
		{
			return false;
		}
	}

	if (util_string_empty(func))
	{
		return true; // Search script file only.
	}

	return scp_file_find_function(pScpFile, func);
}

FRAME_API bool frame_run_routine(const char* script, const char* func,
	const uniqid_t& id, const IArgList& args, IArgList* res)
{
	Assert(script != NULL);
	Assert(func != NULL);

	frame_imp_t* pFrameImp = get_frame_imp();
	scp_manager_t* pScpManager = pFrameImp->pScriptManager;
	scp_file_t* pScpFile = scp_manager_find_script(pScpManager, script);

	if (NULL == pScpFile)
	{
		pScpFile = scp_manager_add_script(pScpManager, 
			frame_get_script_path(), script);

		if (NULL == pScpFile)
		{
			frame_log_print(FRAME_DEFAULT_LOG,
				"(frame_run_routine)Load script %s function %s failed.",
				script, func);
			return false;
		}
	}
	else if (pFrameImp->bScriptReload)
	{
		if (!scp_manager_reload_script(pScpManager, pScpFile))
		{
			frame_log_print(FRAME_DEFAULT_LOG,
				"(frame_run_routine)Reload script %s function %s failed.",
				script, func);
			return false;
		}
	}

	if (!scp_file_find_function(pScpFile, func))
	{
		frame_log_print(FRAME_DEFAULT_LOG, 
			"(frame_run_routine)Script %s function %s not found.",
			script, func);
		return false;
	}

	size_t routine_id;
	lua_State* new_state = scp_file_new_routine(pScpFile, &routine_id);

	if (NULL == new_state)
	{
		frame_log_print(FRAME_DEFAULT_LOG,
			"(frame_run_routine)Script %s function %s new coroutine error.",
			script, func);
		return false;
	}

	scp_cache_t* pScpCache = pFrameImp->pScriptRoutineCache;
	scp_routine_t* pRoutine = scp_cache_new_routine(pScpCache, routine_id,
		new_state, pScpFile, func);

	scp_util_set_context(new_state, pRoutine);
	scp_routine_set_self(pRoutine, id);

	if (scp_routine_start(pRoutine, args))
	{
		// Get script return value.
		if (res != NULL)
		{
			int res_num = lua_gettop(new_state);

			res->Clear();
			scp_util_read_arg_list(new_state, 1, res_num, res);
		}
	}

	if (scp_routine_stopped(pRoutine))
	{
		// Script ended.
		scp_cache_delete_routine(pScpCache, pRoutine);
	}
	else
	{
		// Script paused.
		scp_pool_add(pFrameImp->pScriptRoutinePool, pRoutine);
	}

	return true;
}

FRAME_API bool frame_find_routine(const char* script, const char* func,
	const uniqid_t& id)
{
	return scp_pool_find(get_frame_imp()->pScriptRoutinePool, script, func,
		id) != NULL;
}

FRAME_API bool frame_kill_routine(const char* script, const char* func,
	const uniqid_t& id)
{
	return scp_pool_kill(get_frame_imp()->pScriptRoutinePool, script, func, id);
}

FRAME_API size_t frame_send_routine_signal(const uniqid_t& id,
	const char* signal, const IArgList& args)
{
	return scp_pool_send_signal(get_frame_imp()->pScriptRoutinePool, id,
		signal, &args);
}

FRAME_API bool frame_bind_script(IObject* pObj, const char* script)
{
	Assert(pObj != NULL);
	Assert(script != NULL);

	if (util_string_empty(script))
	{
		// Remove script hook.
		pObj->SetHook(NULL);
		return true;
	}
	
	scp_manager_t* pScpManager = get_frame_imp()->pScriptManager;
	scp_file_t* pScpFile = scp_manager_find_script(pScpManager, script);

	if (NULL == pScpFile)
	{
		pScpFile = scp_manager_add_script(pScpManager, 
			frame_get_script_path(), script);

		if (NULL == pScpFile)
		{
			frame_log_print(FRAME_DEFAULT_LOG, 
				"(frame_bind_script)Load script %s failed.", script);
			return false;
		}
	}

	FrameObjectHook* pHook = FrameObjectHook::CreateInstance();
	
	pHook->SetName(script);
	pObj->SetHook(pHook);
	return true;
}

FRAME_API bool frame_find_event_hook(IObject* pObj, const char* event)
{
	Assert(pObj != NULL);

	IObjectHook* pHook = pObj->GetHook();

	if (NULL == pHook)
	{
		return false;
	}

	return pHook->FindEventProcessor(event);
}

FRAME_API bool frame_process_event(IObject* pObj, int type, const char* event,
	const IArgList& args, IArgList* res)
{
	Assert(pObj != NULL);
	Assert(event != NULL);
	
	IObjectHook* pHook = pObj->GetHook();

	if (NULL == pHook)
	{
		return false;
	}

	return pHook->ProcessEvent(pObj->GetUID(), type, event, args, res);
}

FRAME_API bool frame_reload_all_script()
{
	return scp_manager_reload_all_script(get_frame_imp()->pScriptManager);
}

FRAME_API bool frame_add_work(IObject* pObj)
{
	Assert(pObj != NULL);
	
	return frame_objects_add_work(get_frame_imp()->pObjectManager, pObj);
}

FRAME_API bool frame_remove_work(IObject* pObj)
{
	Assert(pObj != NULL);

	return frame_objects_remove_work(get_frame_imp()->pObjectManager, pObj);
}

FRAME_API bool frame_find_global_value(const char* name)
{
	Assert(name != NULL);

	return get_frame_imp()->pGlobals->Exists(name);
}

FRAME_API bool frame_remove_global_value(const char* name)
{
	Assert(name != NULL);

	return get_frame_imp()->pGlobals->Remove(name);
}

FRAME_API bool frame_set_global_value(const char* name, const IArgData& value)
{
	Assert(name != NULL);

	IDataTable* pGlobals = get_frame_imp()->pGlobals;
	IArgData* pData = pGlobals->GetValue(name);

	if (NULL == pData)
	{
		return pGlobals->Add(name, value) != NULL;
	}

	pData->Assign(value);
	return true;
}

IArgData* frame_get_global(const char* name)
{
	Assert(name != NULL);

	return get_frame_imp()->pGlobals->GetValue(name);
}

FRAME_API IArgData* frame_get_global_value(const char* name)
{
	Assert(name != NULL);

	IDataTable* pGlobals = get_frame_imp()->pGlobals;
	IArgData* pData = pGlobals->GetValue(name);

	if (NULL == pData)
	{
		pData = pGlobals->Add(name, TArgData<4, FrameInnerAlloc>());
	}

	return pData;
}

FRAME_API size_t frame_get_global_count()
{
	return get_frame_imp()->pGlobals->GetCount();
}

FRAME_API void frame_get_global_list(IArgList* result)
{
	Assert(result != NULL);
	
	get_frame_imp()->pGlobals->GetKeyList(result);
}

FRAME_API bool frame_startup(const frame_preset_t& preset)
{
	frame_imp_t* pFrameImp = get_frame_imp();
	
	pFrameImp->bScriptReload = preset.bScriptReload;
	pFrameImp->nScriptProfile = preset.nScriptProfile;
	pFrameImp->pWorkPath = alloc_string(pFrameImp->pWorkPath,
		preset.szWorkPath);
	pFrameImp->pScriptPath = alloc_string(pFrameImp->pScriptPath,
		preset.szScriptPath);
	pFrameImp->pCoderPath = alloc_string(pFrameImp->pCoderPath,
		preset.szCoderPath);
	pFrameImp->pAssetPath = alloc_string(pFrameImp->pAssetPath,
		preset.szAssetPath);
	pFrameImp->pOutputPath = alloc_string(pFrameImp->pOutputPath,
		preset.szOutputPath);

	// Get object creators.
	IObjectCreator* pObjectCreator = preset.pObjectCreator;

	while (pObjectCreator != NULL)
	{
		const char* obj_name = pObjectCreator->GetName();

		if (frame_objects_get_desc(pFrameImp->pObjectManager, obj_name) != NULL)
		{
			frame_log_print(FRAME_DEFAULT_LOG, 
				"(frame_startup)Repeat object name %s.", obj_name);
		}
		else
		{
			frame_objects_add_creator(pFrameImp->pObjectManager,
				pObjectCreator);
		}

		pObjectCreator = pObjectCreator->GetNext();
	}

	// Get service creators.
	IServiceCreator* pServiceCreator = preset.pServiceCreator;

	while (pServiceCreator != NULL)
	{
		const char* svc_name = pServiceCreator->GetName();

		if (frame_services_find(pFrameImp->pServiceManager, svc_name))
		{
			frame_log_print(FRAME_DEFAULT_LOG, 
				"(frame_startup)repeat service name %s", svc_name);
		}
		else
		{
			frame_services_add_creator(pFrameImp->pServiceManager,
				pServiceCreator);
		}

		pServiceCreator = pServiceCreator->GetNext();
	}

	// Set object inheritance information.
	frame_objects_set_all_parent(pFrameImp->pObjectManager);

	// File access interface.
	if (preset.pFileAccess != NULL)
	{
		pFrameImp->pFileAccess = preset.pFileAccess;
	}
	else
	{
		pFrameImp->pFileAccess = FrameFileAccess::CreateInstance();
	}
	
	// Main object.
	if (!util_string_empty(preset.szMainObject))
	{
		// Create main object.
		pFrameImp->pMainObject = frame_create_object(preset.szMainObject);
		
		if (NULL == pFrameImp->pMainObject)
		{
			frame_log_print(FRAME_DEFAULT_LOG, 
				"(frame_startup)Create main object %s failed.", 
				preset.szMainObject);
			return false;
		}
	}
	
	return true;
}

FRAME_API bool frame_shutdown()
{
	frame_imp_t* pFrameImp = get_frame_imp();
	
	if (pFrameImp->pMainObject)
	{
		// Delete main object.
		frame_factory_set_deleted(pFrameImp->pObjectFactory,
			pFrameImp->pMainObject);
		pFrameImp->pMainObject = NULL;

#ifdef K_DEBUG
		if (frame_factory_get_count(pFrameImp->pObjectFactory) > 0)
		{
			// Output all no deleted object.
			frame_factory_dump(pFrameImp->pObjectFactory, "object.log");
		}
#endif // K_DEBUG
	}

	// Clear deleted objects.
	frame_factory_clear_deleted(pFrameImp->pObjectFactory);
	// Set system closing flag.
	// System will crash if any object release other object in destructor.
	pFrameImp->bClosing = true;
	// Release all objects.
	frame_factory_release_all(pFrameImp->pObjectFactory);
	// Release all services.
	frame_services_release_all(pFrameImp->pServiceManager);
	// Release file access interface.
	pFrameImp->pFileAccess->Release();
	pFrameImp->pFileAccess = NULL;
	return true;
}

FRAME_API void frame_period_set_time(double seconds)
{
	frame_imp_t* pFrameImp = get_frame_imp();
	
	pFrameImp->dPeriodSeconds = seconds;
	pFrameImp->dTotalSeconds += seconds;
}

FRAME_API void frame_period_check_routine()
{
	frame_imp_t* pFrameImp = get_frame_imp();
	
	// Check script coroutine.
	scp_pool_check(pFrameImp->pScriptRoutinePool, pFrameImp->dPeriodSeconds);
}

FRAME_API void frame_period_pre_work()
{
	frame_imp_t* pFrameImp = get_frame_imp();

	// Execute object pre-work.
	frame_objects_pre_work(pFrameImp->pObjectManager, 
		pFrameImp->dPeriodSeconds);
	// Clear deleted objects.
	frame_factory_clear_deleted(pFrameImp->pObjectFactory);
}

FRAME_API void frame_period_work()
{
	frame_imp_t* pFrameImp = get_frame_imp();

	// Execute object work.
	frame_objects_work(pFrameImp->pObjectManager, pFrameImp->dPeriodSeconds);
	// Clear deleted objects.
	frame_factory_clear_deleted(pFrameImp->pObjectFactory);
}

FRAME_API void frame_period_post_work()
{
	frame_imp_t* pFrameImp = get_frame_imp();

	// Execute object post-work.
	frame_objects_post_work(pFrameImp->pObjectManager, 
		pFrameImp->dPeriodSeconds);
	// Clear deleted objects.
	frame_factory_clear_deleted(pFrameImp->pObjectFactory);
}
