// script_pool.cpp
// Created by lulimin on 2019/2/18.

#include "script_pool.h"
#include "script_routine.h"
#include "frame_imp.h"
#include "../inc/platform.h"

struct scp_pool_node_t
{
	scp_routine_t* pRoutine;
	scp_pool_node_t* pPrev;
	scp_pool_node_t* pNext;
};

struct scp_pool_t
{
	scp_pool_node_t* pHead;
	scp_pool_node_t* pTail;
	size_t nCount;
};

scp_pool_t* scp_pool_create()
{
	scp_pool_t* pScpPool = K_INNER_NEW(scp_pool_t);
	
	pScpPool->pHead = NULL;
	pScpPool->pTail = NULL;
	pScpPool->nCount = 0;
	return pScpPool;
}

static void scp_pool_delete_node(scp_pool_node_t* p)
{
	Assert(p != NULL);

	frame_delete_routine(p->pRoutine);
	K_INNER_FREE(p);
}

void scp_pool_delete(scp_pool_t* pScpPool)
{
	Assert(pScpPool != NULL);

	scp_pool_node_t* p = pScpPool->pHead;

	while (p)
	{
		scp_pool_node_t* t = p;

		p = p->pNext;
		scp_pool_delete_node(t);
	}

	K_INNER_DELETE(pScpPool);
}

bool scp_pool_add(scp_pool_t* pScpPool, scp_routine_t* pRoutine)
{
	Assert(pScpPool != NULL);
	Assert(pRoutine != NULL);

	scp_pool_node_t* pNode = (scp_pool_node_t*)K_INNER_ALLOC(
		sizeof(scp_pool_node_t));

	pNode->pRoutine = pRoutine;

	// Add to last.
	if (pScpPool->pTail != NULL)
	{
		pNode->pPrev = pScpPool->pTail;
		pNode->pNext = NULL;
		pScpPool->pTail->pNext = pNode;
	}
	else
	{
		Assert(NULL == pScpPool->pHead);

		pNode->pPrev = NULL;
		pNode->pNext = NULL;
		pScpPool->pHead = pNode;
	}

	pScpPool->pTail = pNode;
	pScpPool->nCount++;
	return true;
}

scp_routine_t* scp_pool_find(scp_pool_t* pScpPool, const char* script,
	const char* func, const uniqid_t& id)
{
	Assert(pScpPool != NULL);
	Assert(script != NULL);
	Assert(func != NULL);

	scp_pool_node_t* p = pScpPool->pHead;

	while (p)
	{
		scp_routine_t* pRoutine = p->pRoutine;

		if ((!scp_routine_stopped(pRoutine)) &&
			(strcmp(scp_routine_get_function(pRoutine), func) == 0) &&
			(stricmp(scp_routine_get_script(pRoutine), script) == 0))
		{
			if (uniqid_is_null(id))
			{
				return pRoutine;
			}

			if (uniqid_equal(scp_routine_get_self(pRoutine), id))
			{
				return pRoutine;
			}
		}

		p = p->pNext;
	}

	return NULL;
}

bool scp_pool_kill(scp_pool_t* pScpPool, const char* script, const char* func,
	const uniqid_t& id)
{
	Assert(pScpPool != NULL);
	Assert(script != NULL);
	Assert(func != NULL);

	scp_pool_node_t* p = pScpPool->pHead;

	while (p)
	{
		scp_routine_t* pRoutine = p->pRoutine;

		if ((!scp_routine_stopped(pRoutine)) &&
			(strcmp(scp_routine_get_function(pRoutine), func) == 0) &&
			(stricmp(scp_routine_get_script(pRoutine), script) == 0))
		{
			if (uniqid_is_null(id) ||
				uniqid_equal(scp_routine_get_self(pRoutine), id))
			{
				// Set stopped status.
				scp_routine_stop(pRoutine);
				return true;
			}
		}

		p = p->pNext;
	}

	return false;
}

static void scp_pool_erase_node(scp_pool_t* pScpPool, scp_pool_node_t* p)
{
	Assert(pScpPool != NULL);
	Assert(p != NULL);

	// Delete in link.
	if (p->pPrev != NULL)
	{
		p->pPrev->pNext = p->pNext;
	}

	if (p->pNext != NULL)
	{
		p->pNext->pPrev = p->pPrev;
	}

	if (pScpPool->pHead == p)
	{
		pScpPool->pHead = p->pNext;
	}

	if (pScpPool->pTail == p)
	{
		pScpPool->pTail = p->pPrev;
	}

	scp_pool_delete_node(p);
	pScpPool->nCount--;
}

void scp_pool_check(scp_pool_t* pScpPool, double elapse)
{
	Assert(pScpPool != NULL);

	scp_pool_node_t* p = pScpPool->pHead;

	if (NULL == p)
	{
		return;
	}

	// Current tail node.
	scp_pool_node_t* tail = pScpPool->pTail;

	while (p)
	{
		scp_routine_t* pRoutine = p->pRoutine;

		// Test coroutine stopped.
		if (scp_routine_stopped(pRoutine))
		{
			scp_pool_node_t* t = p;

			p = p->pNext;
			scp_pool_erase_node(pScpPool, t);

			// Donot check new added node.
			if (t == tail)
			{
				break;
			}

			continue;
		}

		scp_routine_check(pRoutine, elapse); // Check coroutine time out.

		if (p == tail)
		{
			break;
		}

		p = p->pNext;
	}
}

size_t scp_pool_send_signal(scp_pool_t* pScpPool, const uniqid_t& id,
	const char* signal, const IArgList* args)
{
	Assert(pScpPool != NULL);
	Assert(signal != NULL);

	scp_pool_node_t* p = pScpPool->pHead;

	if (NULL == p)
	{
		return 0;
	}

	scp_pool_node_t* tail = pScpPool->pTail;
	size_t count = 0;

	while (p)
	{
		scp_routine_t* pRoutine = p->pRoutine;

		if (scp_routine_stopped(pRoutine))
		{
			if (p == tail)
			{
				break;
			}
			
			p = p->pNext;
			continue;
		}

		// Test wait signal.
		if (!scp_routine_has_wait_signal(pRoutine))
		{
			if (p == tail)
			{
				break;
			}

			p = p->pNext;
			continue;
		}

		// Match signal name.
		if (!scp_routine_find_wait_signal(pRoutine, signal))
		{
			if (p == tail)
			{
				break;
			}

			p = p->pNext;
			continue;
		}

		// Test relate object.
		if (uniqid_not_null(id))
		{
			uniqid_t wait_id = scp_routine_wait_signal_object(pRoutine, signal);

			if (uniqid_not_equal(id, wait_id))
			{
				if (p == tail)
				{
					break;
				}

				p = p->pNext;
				continue;
			}
		}

		// Wake up coroutine.
		if (scp_routine_wait_signal_count(pRoutine) > 1)
		{
			// Return signal name when wait multiple signal.
			scp_routine_resume(pRoutine, signal, args);
		}
		else
		{
			// Wait one signal.
			scp_routine_resume(pRoutine, NULL, args);
		}

		count++;

		if (p == tail)
		{
			break;
		}

		p = p->pNext;
	}

	return count;
}

bool scp_pool_dump(scp_pool_t* pScpPool, const char* name)
{
	Assert(pScpPool != NULL);

	FILE* fp = plat_file_open(name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	scp_pool_node_t* p = pScpPool->pHead;

	while (p)
	{
		scp_routine_t* pRoutine = p->pRoutine;

		fprintf(fp, "Script:%s function:%s timeout:%f.\r\n",
			scp_routine_get_script(pRoutine), 
			scp_routine_get_function(pRoutine),
			scp_routine_get_timeout(pRoutine));
		p = p->pNext;
	}

	fprintf(fp, "Total %zd script coroutines.\r\n", pScpPool->nCount);
	fclose(fp);
	return true;
}
