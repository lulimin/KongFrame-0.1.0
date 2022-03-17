// frame_func.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_FUNC_H
#define __FRAME_FUNC_H

// Donot include this file directly.
// Frame functions.

// Default log level.
#define FRAME_DEFAULT_LOG 1

class IArgData;
class IArgList;
class IObject;
class IObjectDesc;
class IService;
class IFileAccess;

// Record log function.
typedef bool(*frame_log_func_t)(void* context, int level, const char* info);
// Allocate memory function.
typedef void* (*frame_alloc_func_t)(size_t size);
// Allocate memory function in debug mode.
typedef void* (*frame_debug_alloc_func_t)(size_t size, const char* file,
	int line);
// Reallocate memory function.
typedef void* (*frame_realloc_func_t)(void* ptr, size_t new_size);
// Free memory function.
typedef void(*frame_free_func_t)(void* ptr);
// Dump memory information function.
typedef bool(*frame_dump_func_t)(const char* file_name);

// Setup thread key in multi-thread mode.
FRAME_API bool frame_multithread_setup();
// Clean thread key in multi-thread mode.
FRAME_API bool frame_multithread_clean();

// Create frame.
FRAME_API void frame_create(frame_debug_alloc_func_t debug_alloc_func, 
	frame_alloc_func_t alloc_func, frame_realloc_func_t realloc_func,
	frame_free_func_t free_func, frame_dump_func_t dump_func);
// Delete frame.
FRAME_API void frame_delete();
// Start frame.
FRAME_API bool frame_startup(const frame_preset_t& preset);
// Shutdown frame.
FRAME_API bool frame_shutdown();

// Get work path.
FRAME_API const char* frame_get_work_path();
// Get script file path.
FRAME_API const char* frame_get_script_path();
// Get coder file path.
FRAME_API const char* frame_get_coder_path();
// Get asset file path.
FRAME_API const char* frame_get_asset_path();
// Get output file path.
FRAME_API const char* frame_get_output_path();

// Set flag of exit program.
FRAME_API void frame_set_exit(bool value);
// Get flag of exit program.
FRAME_API bool frame_get_exit();

// Get current frame time.
FRAME_API double frame_get_period_seconds();
// Get current total time counter.
FRAME_API double frame_get_total_seconds();
// Get script execute total time.
FRAME_API double frame_get_script_seconds();

// Execute per frame.
FRAME_API void frame_period_set_time(double seconds);
FRAME_API void frame_period_check_routine();
FRAME_API void frame_period_pre_work();
FRAME_API void frame_period_work();
FRAME_API void frame_period_post_work();

// Get current log level.
FRAME_API bool frame_set_log_level(int level);
// Set current log level.
FRAME_API int frame_get_log_level();
// Clear log file.
FRAME_API bool frame_clear_log();
// Write log to file.
FRAME_API bool frame_log_string(int level, const char* log);
FRAME_API bool __cdecl frame_log_print(int level, const char* log, ...);
// Redirect log to custom function.
FRAME_API bool frame_set_log_func(frame_log_func_t func, void* context);
// Dump program information to file.
FRAME_API bool frame_dump_information(const char* type, const char* file);

// Search service class.
FRAME_API bool frame_find_service(const char* name);
// Get service interface.
FRAME_API IService* frame_get_service(const char* name);
// Release service interface.
FRAME_API void frame_release_service(IService* pService);

// Find object description.
FRAME_API IObjectDesc* frame_get_object_desc(const char* name);
// Get main object.
FRAME_API IObject* frame_get_main_object();
// Get object by object id.
FRAME_API IObject* frame_get_object(const uniqid_t& id);
// Get all object id.
FRAME_API void frame_get_all_object(IArgList* result);
// Get first object match the name.
FRAME_API IObject* frame_find_object(const char* name);
// Get all object match the name.
FRAME_API void frame_find_object_list(const char* name, IArgList* result);
// Create object.
FRAME_API IObject* frame_create_object(const char* name);
// Create object with parameters.
FRAME_API IObject* frame_create_object_args(const char* name, 
	const IArgList& args);
// Create extra property table of object.
FRAME_API void frame_create_extra(IObject* pObj);
// Delete object.
FRAME_API bool frame_delete_object(const uniqid_t& id);

// Get object property.
FRAME_API bool frame_get_property(IObject* pObj, const char* prop,
	IArgData* value);
// Set object property.
FRAME_API bool frame_set_property(IObject* pObj, const char* prop,
	const IArgData& value);
// Call object method.
FRAME_API bool frame_run_method(IObject* pObj, const char* func,
	const IArgList& args, IArgList* result);

// Test script function exists.
FRAME_API bool frame_find_script(const char* script, const char* func);
// Execute script coroutine.
FRAME_API bool frame_run_routine(const char* script, const char* func,
	const uniqid_t& id, const IArgList& args, IArgList* res);
// Search script coroutine.
FRAME_API bool frame_find_routine(const char* script, const char* func,
	const uniqid_t& id);
// Kill script coroutine.
FRAME_API bool frame_kill_routine(const char* script, const char* func,
	const uniqid_t& id);
// Send signal, return number of triggered coroutines.
FRAME_API size_t frame_send_routine_signal(const uniqid_t& id,
	const char* signal, const IArgList& args);
// Bind script to object.
FRAME_API bool frame_bind_script(IObject* pObj, const char* script);
// Search event processor.
FRAME_API bool frame_find_event_hook(IObject* pObj, const char* event);
// Execute event process.
FRAME_API bool frame_process_event(IObject* pObj, int type, const char* event,
	const IArgList& args, IArgList* res);
// Reload all script file.
FRAME_API bool frame_reload_all_script();

// Add object to frame work queue.
FRAME_API bool frame_add_work(IObject* pObj);
// Remove object from frame work queue.
FRAME_API bool frame_remove_work(IObject* pObj);

// Global values.
FRAME_API bool frame_find_global_value(const char* name);
FRAME_API bool frame_remove_global_value(const char* name);
FRAME_API bool frame_set_global_value(const char* name,
	const IArgData& value);
FRAME_API IArgData* frame_get_global_value(const char* name);
FRAME_API size_t frame_get_global_count();
FRAME_API void frame_get_global_list(IArgList* result);

// Get file access interface.
FRAME_API IFileAccess* frame_get_file_access();

// Cross module memory management.
FRAME_API void* frame_debug_alloc(size_t size, const char* file, int line);
FRAME_API void* frame_alloc(size_t size);
FRAME_API void* frame_realloc(void* ptr, size_t new_size);
FRAME_API void frame_free(void* ptr);

#endif // __FRAME_FUNC_H
