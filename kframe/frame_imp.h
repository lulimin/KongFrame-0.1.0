// frame_imp.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_IMP_H
#define __FRAME_IMP_H

#include "../inc/common.h"
#include "../inc/uniqid.h"
#include "../inc/frame_preset.h"

#ifdef K_DYNAMIC_LIB
	#define FRAME_API extern "C" K_LIB_EXPORT
#else
	#define FRAME_API
#endif // K_DYNAMIC_LIB

#include "../inc/frame_func.h"

class IMethodDesc;
struct scp_routine_t;

// Get system in closing status.
bool frame_get_closing();
// Get flag of profile script.
int frame_get_script_profile();
// Increase script execute time.
void frame_add_script_seconds(double value);
// Kill script coroutine.
void frame_delete_routine(scp_routine_t* pRoutine);
// Push method to stack.
void frame_push_method(IMethodDesc* pMethod);
// Pop method from stack.
void frame_pop_method();
// Get current method.
IMethodDesc* frame_get_method();
// Get file access interface.
IFileAccess* frame_file_access();
// Create object in script.
IObject* frame_script_create_object(const char* name, const IArgList& args);
// Get global value.
IArgData* frame_get_global(const char* name);

#endif // __FRAME_IMP_H
