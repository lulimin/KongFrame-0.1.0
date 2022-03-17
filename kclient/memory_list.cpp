// memory_list.cpp
// Created by lulimin on 2020/2/18.

#include "../inc/common.h"
#include "../inc/frame_def.h"
#include "../inc/thread_lock.h"
#include "../inc/platform.h"

// Memory information node.
struct mem_node_t
{
	void* pMemory;
	size_t nSize;
	const char* pFile;
	int nLine;
};

// Memory share lock.
static ThreadLock s_MemoryListLock;
// Pointer to buckets.
static mem_node_t* s_pBuckets = NULL;
// Number of buckets.
static size_t s_nBucketNum = 0;
// Number of memory blocks.
static size_t s_nDataNum = 0;

// Initialize memory list.
void memory_list_init()
{
	Assert(NULL == s_pBuckets);

	s_pBuckets = NULL;
	s_nBucketNum = 0;
	s_nDataNum = 0;
}

// Shutdown memory list.
void memory_list_shut()
{
	if (s_pBuckets)
	{
		delete[] s_pBuckets;
		s_pBuckets = NULL;
	}
}

// Get hash value of memory block.
static size_t memory_list_hash(void* ptr)
{
	return (((size_t)ptr) >> 3) * 31;
}

// Expand memory list.
static void memory_list_expand()
{
	size_t new_bucket_num;

	if (0 == s_nBucketNum)
	{
		new_bucket_num = 0x10000;
	}
	else
	{
		new_bucket_num = s_nBucketNum * 2;
	}

	size_t new_bucket_num_1 = new_bucket_num - 1;
	mem_node_t* new_buckets = NEW mem_node_t[new_bucket_num];

	memset(new_buckets, 0, sizeof(mem_node_t) * new_bucket_num);

	for (size_t i = 0; i < s_nBucketNum; ++i)
	{
		mem_node_t* p = &s_pBuckets[i];

		if (p->pMemory)
		{
			for (size_t k = 0; k < new_bucket_num; ++k)
			{
				size_t hash = memory_list_hash(p->pMemory);
				size_t bucket = (hash + k) & new_bucket_num_1;

				if (NULL == new_buckets[bucket].pMemory)
				{
					new_buckets[bucket] = *p;
					break;
				}
			}
		}
	}

	if (s_pBuckets)
	{
		delete[] s_pBuckets;
	}

	s_pBuckets = new_buckets;
	s_nBucketNum = new_bucket_num;
}

// Add memory block to list.
static bool memory_list_add(void* ptr, size_t size, const char* file, int line)
{
	ScopedLock scoped_lock(&s_MemoryListLock);

	if ((s_nDataNum * 2) == s_nBucketNum)
	{
		memory_list_expand();
	}

	size_t hash = memory_list_hash(ptr);
	size_t bucket_num_1 = s_nBucketNum - 1;

	for (size_t i = 0; i < s_nBucketNum; ++i)
	{
		size_t bucket = (hash + i) & bucket_num_1;
		mem_node_t* pNode = &s_pBuckets[bucket];

		if (NULL == pNode->pMemory)
		{
			pNode->pMemory = ptr;
			pNode->nSize = size;
			pNode->pFile = file;
			pNode->nLine = line;
			s_nDataNum++;
			return true;
		}
		else if (pNode->pMemory == ptr)
		{
			return false; // Same data exists.
		}
	}

	Assert(0);
	return false;
}

// Remove memory block from list.
static bool memory_list_remove(void* ptr)
{
	ScopedLock scoped_lock(&s_MemoryListLock);

	if (0 == s_nBucketNum)
	{
		return false;
	}

	size_t hash = memory_list_hash(ptr);
	size_t bucket_num_1 = s_nBucketNum - 1;

	for (size_t i = 0; i < s_nBucketNum; ++i)
	{
		size_t bucket = (hash + i) & bucket_num_1;
		mem_node_t* pNode = &s_pBuckets[bucket];

		if ((NULL == pNode->pMemory) && (NULL == pNode->pFile))
		{
			return false;
		}

		if (pNode->pMemory == ptr)
		{
			pNode->pMemory = NULL;
			pNode->pFile = NULL;
			s_nDataNum--;
			return true;
		}
	}

	return false;
}

// Allocate memory in debug mode.
void* memory_list_debug_alloc(size_t size, const char* file, int line)
{
#if (defined(K_PLATFORM_WINDOWS) && defined(K_DEBUG))
	void* ptr = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
#else
	void* ptr = malloc(size);
#endif 

	if (NULL == ptr)
	{
		// Allocate memory failed.
		frame_log_print(FRAME_DEFAULT_LOG,
			"(memory_list_debug_alloc)No enough memory size:%zd.", size);
		return NULL;
	}

	if (!memory_list_add(ptr, size, file, line))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(memory_list_debug_alloc)Memory check failed.");
		abort();
	}

	return ptr;
}

// Allocate memory.
void* memory_list_alloc(size_t size)
{
	void* ptr = malloc(size);

	if (NULL == ptr)
	{
		// Allocate memory failed.
		frame_log_print(FRAME_DEFAULT_LOG,		
			"(memory_list_alloc)No enough memory size:%zd.", size);
		return NULL;
	}

	if (!memory_list_add(ptr, size, "", 0))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(memory_list_alloc)Memory check failed.");
		abort();
	}

	return ptr;
}

// Reallocate memory.
void* memory_list_realloc(void* old_ptr, size_t new_size)
{
	void* new_ptr = realloc(old_ptr, new_size);

	if (NULL == new_ptr)
	{
		if (new_size > 0)
		{
			// Allocate memory failed.
			frame_log_print(FRAME_DEFAULT_LOG,
				"(memory_list_realloc)No enough memory size:%zd.", new_size);
		}
		else if (old_ptr)
		{
			if (!memory_list_remove(old_ptr))
			{
				frame_log_string(FRAME_DEFAULT_LOG, 
					"(memory_list_realloc)Remove failed.");
				abort();
			}
		}

		return NULL;
	}

	if (old_ptr)
	{
		if (!memory_list_remove(old_ptr))
		{
			frame_log_string(FRAME_DEFAULT_LOG, 
				"(memory_list_realloc)Check remove failed.");
			abort();
		}
	}

	if (!memory_list_add(new_ptr, new_size, "", 0))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(memory_list_realloc)Check add failed.");
		abort();
	}

	return new_ptr;
}

// Free memory.
void memory_list_free(void* ptr)
{
	if (NULL == ptr)
	{
		return;
	}

	if (!memory_list_remove(ptr))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(memory_list_free)Memory check failed.");
		abort();
	}

	free(ptr);
}

// Dump memory informations.
bool memory_list_dump(const char* file_name)
{
	Assert(file_name != NULL);
	
	if (0 == s_nDataNum)
	{
		// No memory leaking.
		return false;
	}
	
	ScopedLock scoped_lock(&s_MemoryListLock);

	FILE* fp = plat_file_open(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	int count = 0;
	int size = 0;

	for (size_t i = 0; i < s_nBucketNum; ++i)
	{
		mem_node_t* pNode = &s_pBuckets[i];

		if (NULL == pNode->pMemory)
		{
			continue;
		}

		fprintf(fp, "Block pointer:%p size:%zd file:%s line:%d.\r\n",
			pNode->pMemory, pNode->nSize, pNode->pFile, pNode->nLine);
		count++;
		size += (int)pNode->nSize;
	}

	fprintf(fp, "Total %d blocks %d bytes.\r\n", count, size);
	fclose(fp);
	return true;
}
