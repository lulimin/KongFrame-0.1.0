// frame_inner_mem.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_INNER_MEM_H
#define __FRAME_INNER_MEM_H

#include <new>

void* frame_inner_debug_alloc(size_t size, const char* file, int line);
void* frame_inner_alloc(size_t size);
void frame_inner_free(void* ptr);

// Framework internal memory management.

template<typename TYPE>
TYPE* frame_inner_debug_new(TYPE*, const char* file, int line)
{
	TYPE* ptr = (TYPE*)frame_inner_debug_alloc(sizeof(TYPE), file, line);

	new (ptr) TYPE;
	return ptr;
}

template<typename TYPE>
TYPE* frame_inner_new(TYPE*)
{
	TYPE* ptr = (TYPE*)frame_inner_alloc(sizeof(TYPE));

	new (ptr) TYPE;
	return ptr;
}

template<typename TYPE>
void frame_inner_delete(TYPE* ptr)
{
	if (ptr)
	{
		ptr->~TYPE();
	}

	frame_inner_free(ptr);
}

// Memory allocate and free.
#ifdef K_DEBUG
	#define K_INNER_ALLOC(size) frame_inner_debug_alloc(size, __FILE__, __LINE__)
	#define K_INNER_FREE(ptr) frame_inner_free(ptr)
	#define K_INNER_NEW(type) frame_inner_debug_new((type*)0, __FILE__, __LINE__)
	#define K_INNER_DELETE(ptr) frame_inner_delete(ptr)
#else
	#define K_INNER_ALLOC(size) frame_inner_alloc(size)
	#define K_INNER_FREE(ptr) frame_inner_free(ptr)
	#define K_INNER_NEW(type) frame_inner_new((type*)0)
	#define K_INNER_DELETE(ptr) frame_inner_delete(ptr)
#endif // K_DEBUG

// Specified class allocator.
#define DEFINE_INNER_ALLOC(cls) \
class cls##Alloc \
{ \
public: \
	cls##Alloc() {} \
	~cls##Alloc() {} \
	void* Alloc(size_t size) { return K_INNER_ALLOC(size); } \
	void Free(void* ptr) { K_INNER_FREE(ptr); } \
};

// Define class FrameInnerAlloc.
DEFINE_INNER_ALLOC(FrameInner);

#endif // __FRAME_INNER_MEM_H
