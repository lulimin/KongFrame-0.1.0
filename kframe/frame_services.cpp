// frame_services.cpp
// Created by lulimin on 2019/2/18.

#include "frame_services.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "../inc/pod_array.h"
#include "../inc/utility.h"
#include "../inc/platform.h"
#include "../inc/i_service.h"
#include "../inc/i_service_creator.h"

DEFINE_INNER_ALLOC(FrameServices);

struct frame_creator_t
{
	IServiceCreator* pCreator;
	unsigned int nHash;
	IService* pService;
	int64_t nRefCount;
	int64_t nOrder;
};

struct frame_services_t
{
	TPodArray<frame_creator_t, 1, FrameServicesAlloc> Creators;
	int64_t nOrderCount;
};

frame_services_t* frame_services_create()
{
	frame_services_t* pServices = K_INNER_NEW(frame_services_t);

	pServices->nOrderCount = 0;
	return pServices;
}

void frame_services_delete(frame_services_t* pServices)
{
	Assert(pServices != NULL);

	frame_services_release_all(pServices);
	K_INNER_DELETE(pServices);
}

static int sort_service_compare(const void* elem1, const void* elem2)
{
	frame_creator_t* p1 = (frame_creator_t*)elem1;
	frame_creator_t* p2 = (frame_creator_t*)elem2;

	if (p1->nOrder < p2->nOrder)
	{
		return 1;
	}

	if (p1->nOrder > p2->nOrder)
	{
		return -1;
	}

	return 0;
}

void frame_services_release_all(frame_services_t* pServices)
{
	Assert(pServices != NULL);

	if (pServices->Creators.Empty())
	{
		return;
	}

	// Sort by reference order.
	qsort(&pServices->Creators[0], pServices->Creators.Size(),
		sizeof(frame_creator_t), sort_service_compare);

	// Call shutdown functions.
	for (size_t k = 0; k < pServices->Creators.Size(); ++k)
	{
		if (pServices->Creators[k].pService != NULL)
		{
			Assert(pServices->Creators[k].nRefCount > 0);

			pServices->Creators[k].pService->Shutdown();
		}
	}

	// Delete services.
	for (size_t i = 0; i < pServices->Creators.Size(); ++i)
	{
		if (pServices->Creators[i].pService != NULL)
		{
			Assert(pServices->Creators[i].nRefCount > 0);

			pServices->Creators[i].pCreator->Destroy(
				pServices->Creators[i].pService);
		}
	}

	pServices->Creators.Clear();
}

bool frame_services_add_creator(frame_services_t* pServices, 
	IServiceCreator* pCreator)
{
	Assert(pServices != NULL);
	Assert(pCreator != NULL);

	if (frame_services_find(pServices, pCreator->GetName()))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(frame_services_add_creator)Service name exists.");
		frame_log_string(FRAME_DEFAULT_LOG, pCreator->GetName());
	}

	pServices->Creators.PushBack(frame_creator_t());

	frame_creator_t& data = pServices->Creators.Back();

	data.pCreator = pCreator;
	data.nHash = util_string_hash(pCreator->GetName());
	data.nRefCount = 0;
	data.nOrder = 0;
	data.pService = NULL;
	return true;
}

static bool frame_services_find_index(frame_services_t* pServices,
	const char* name, size_t* pIndex)
{
	Assert(pServices != NULL);
	Assert(name != NULL);
	Assert(pIndex != NULL);

	unsigned int hash = util_string_hash(name);
	const size_t creator_num = pServices->Creators.Size();

	for (size_t i = 0; i < creator_num; ++i)
	{
		if ((pServices->Creators[i].nHash == hash) &&
			(strcmp(pServices->Creators[i].pCreator->GetName(), name) == 0))
		{
			*pIndex = i;
			return true;
		}
	}

	return false;
}

bool frame_services_find(frame_services_t* pServices, const char* name)
{
	Assert(pServices != NULL);

	size_t index;

	return frame_services_find_index(pServices, name, &index);
}

IService* frame_services_get_reference(frame_services_t* pServices, 
	const char* name)
{
	Assert(pServices != NULL);

	size_t index;

	if (!frame_services_find_index(pServices, name, &index))
	{
		return NULL;
	}

	if (pServices->Creators[index].nRefCount == 0)
	{
		Assert(pServices->Creators[index].pService == NULL);

		IServiceCreator* pCreator = pServices->Creators[index].pCreator;
		IService* pSvc = pCreator->Create();

		pSvc->FrameSetCreator(pCreator);
		pServices->Creators[index].pService = pSvc;
		pServices->Creators[index].nRefCount = 1;
		pSvc->Startup();
		pServices->Creators[index].nOrder = ++pServices->nOrderCount;
		return pSvc;
	}
	else
	{
		pServices->Creators[index].nRefCount++;
		return pServices->Creators[index].pService;
	}
}

void frame_services_release_reference(frame_services_t* pServices,
	IService* pSvc)
{
	Assert(pServices != NULL);
	Assert(pSvc != NULL);

	if (frame_get_exit())
	{
		// Cannot release service after system closed.
		return;
	}

	const char* name = pSvc->GetCreator()->GetName();
	size_t index;

	if (!frame_services_find_index(pServices, name, &index))
	{
		Assert(0);
		return;
	}

	Assert(pServices->Creators[index].nRefCount > 0);
	Assert(pServices->Creators[index].pService == pSvc);

	if (--pServices->Creators[index].nRefCount == 0)
	{
		pSvc->Shutdown();
		pServices->Creators[index].pCreator->Destroy(pSvc);
		pServices->Creators[index].pService = NULL;
	}
}

bool frame_services_dump(frame_services_t* pServices, const char* file_name)
{
	Assert(pServices != NULL);
	
	FILE* fp = plat_file_open(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	const size_t size = pServices->Creators.Size();

	for (size_t i = 0; i < size; ++i)
	{
		frame_creator_t* p = &pServices->Creators[i];

		fprintf(fp, "Service:%s reference count:%lld order:%lld.\r\n",
			p->pCreator->GetName(), (long long)p->nRefCount, 
			(long long)p->nOrder);
	}

	fprintf(fp, "Total %zd services.\r\n", size);
	fclose(fp);
	return true;
}
