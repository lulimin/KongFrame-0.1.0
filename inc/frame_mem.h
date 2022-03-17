// frame_mem.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_MEM_H
#define __FRAME_MEM_H

#include "frame_api.h"
#include <new>

// Cross module safety memory management.

template<typename TYPE>
TYPE* frame_debug_new(TYPE*, const char* file, int line)
{
	TYPE* ptr = (TYPE*)frame_debug_alloc(sizeof(TYPE), file, line);

	new (ptr) TYPE;
	return ptr;
}

template<typename TYPE>
TYPE* frame_new(TYPE*)
{
	TYPE* ptr = (TYPE*)frame_alloc(sizeof(TYPE));

	new (ptr) TYPE;
	return ptr;
}

template<typename TYPE>
void frame_delete(TYPE* ptr)
{
	if (ptr)
	{
		ptr->~TYPE();
	}

	frame_free(ptr);
}

// Allocate memory and free memory.
#ifdef K_DEBUG
	#define K_ALLOC(size) frame_debug_alloc(size, __FILE__, __LINE__)
	#define K_REALLOC(ptr, new_size) frame_realloc(ptr, new_size)
	#define K_FREE(ptr) frame_free(ptr)
	#define K_NEW(type) frame_debug_new((type*)0, __FILE__, __LINE__)
	#define K_DELETE(ptr) frame_delete(ptr)
#else
	#define K_ALLOC(size) frame_alloc(size)
	#define K_REALLOC(ptr, new_size) frame_realloc(ptr, new_size)
	#define K_FREE(ptr) frame_free(ptr)
	#define K_NEW(type) frame_new((type*)0)
	#define K_DELETE(ptr) frame_delete(ptr)
#endif // K_DEBUG

// Memory allocator for template.
// Define memory allocator of specified class.
#define DEFINE_FRAME_ALLOC(cls) \
class cls##Alloc \
{ \
public: \
	cls##Alloc() {} \
	~cls##Alloc() {} \
	void* Alloc(size_t size) { return K_ALLOC(size); } \
	void Free(void* ptr) { K_FREE(ptr); } \
};

// Define class FrameAlloc.
DEFINE_FRAME_ALLOC(Frame);

#endif // __FRAME_MEM_H
