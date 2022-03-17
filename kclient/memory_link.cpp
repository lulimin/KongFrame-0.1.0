// memory_link.cpp
// Created by lulimin on 2020/2/18.

#include "../inc/common.h"
#include "../inc/thread_lock.h"
#include "../inc/platform.h"

// Detect memory leak.
struct mem_head_t
{
	mem_head_t* pPrev;
	mem_head_t* pNext;
	const char* pFile;
	uint64_t nLineAndSize;
};

// Memory share lock.
static ThreadLock s_MemoryLinkLock;
// Head pointer to memory blocks.
static mem_head_t* s_pMemoryHead = NULL;

// Initialize memory link.
void memory_link_init()
{
	s_pMemoryHead = NULL;
}

// Shutdown memory link.
void memory_link_shut()
{
}

// Allocate memory in debug mode.
void* memory_link_debug_alloc(size_t size, const char* file, int line)
{
#if (defined(K_PLATFORM_WINDOWS) && defined(K_DEBUG))
	mem_head_t* p = (mem_head_t*)_malloc_dbg(sizeof(mem_head_t) + size,
		_NORMAL_BLOCK, file, line);
#else
	mem_head_t* p = (mem_head_t*)malloc(sizeof(mem_head_t) + size);
#endif

	if (NULL == p)
	{
		return NULL;
	}

	p->pFile = file;
	p->nLineAndSize = (((uint64_t)line) << 48) + size;
	p->pPrev = NULL;

	s_MemoryLinkLock.Lock();

	if (s_pMemoryHead)
	{
		s_pMemoryHead->pPrev = p;
	}

	p->pNext = s_pMemoryHead;
	s_pMemoryHead = p;

	s_MemoryLinkLock.Unlock();
	return p + 1;
}

// Allocate memory.
void* memory_link_alloc(size_t size)
{
	mem_head_t* p = (mem_head_t*)malloc(sizeof(mem_head_t) + size);

	if (NULL == p)
	{
		return NULL;
	}

	p->pFile = "";
	p->nLineAndSize = size;
	p->pPrev = NULL;

	s_MemoryLinkLock.Lock();

	if (s_pMemoryHead)
	{
		s_pMemoryHead->pPrev = p;
	}
	
	p->pNext = s_pMemoryHead;
	s_pMemoryHead = p;

	s_MemoryLinkLock.Unlock();
	return p + 1;
}

// Free memory.
void memory_link_free(void* ptr)
{
	if (NULL == ptr)
	{
		return;
	}
	
	mem_head_t* p = ((mem_head_t*)ptr) - 1;

	s_MemoryLinkLock.Lock();

	if (p->pNext)
	{
		p->pNext->pPrev = p->pPrev;
	}

	if (p->pPrev)
	{
		p->pPrev->pNext = p->pNext;
	}

	if (s_pMemoryHead == p)
	{
		s_pMemoryHead = p->pNext;
	}

	s_MemoryLinkLock.Unlock();

	free(p);
}

// Reallocate memory.
void* memory_link_realloc(void* old_ptr, size_t new_size)
{
	if (0 == new_size)
	{
		memory_link_free(old_ptr);
		return NULL;
	}
	
	void* new_ptr = memory_link_alloc(new_size);

	if (new_ptr)
	{
		if (old_ptr)
		{
			mem_head_t* p = ((mem_head_t*)old_ptr) - 1;
			size_t copy_size = p->nLineAndSize & 0xFFFFFFFFFFFF;

			if (copy_size > new_size)
			{
				copy_size = new_size;
			}

			memcpy(new_ptr, old_ptr, copy_size);
		}
	}
	
	memory_link_free(old_ptr);
	return new_ptr;
}

// Dump memory informations.
bool memory_link_dump(const char* file_name)
{
	Assert(file_name != NULL);

	if (NULL == s_pMemoryHead)
	{
		// No memory leaking.
		return false;
	}
	
	FILE* fp = plat_file_open(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	size_t count = 0;
	size_t size = 0;
	ScopedLock scoped_lock(&s_MemoryLinkLock);
	mem_head_t* p = s_pMemoryHead;

	while (p)
	{
		uint64_t line = (p->nLineAndSize >> 48) & 0xFFFF;
		uint64_t num = p->nLineAndSize & 0xFFFFFFFFFFFF;

		fprintf(fp, "Memory size:%zd file:%s line:%zd.\r\n",
			(size_t)num, p->pFile, (size_t)line);
		++count;
		size += num;
		p = p->pNext;
	}

	fprintf(fp, "Total memory %zd size %zd.\r\n", count, size);
	fclose(fp);
	return true;
}
