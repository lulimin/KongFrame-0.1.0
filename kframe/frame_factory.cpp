// frame_factory.cpp
// Created by lulimin on 2019/2/18.

#include "frame_factory.h"
#include "frame_imp.h"
#include "frame_object_desc.h"
#include "frame_data_table.h"
#include "../inc/arg_data.h"
#include "../inc/platform.h"
#include "../inc/i_object.h"
#include "../inc/i_object_creator.h"

typedef TFrameDataTable<FrameFactoryAlloc> ObjectDataTable;

frame_factory_t* frame_factory_create()
{
	frame_factory_t* pFactory = K_INNER_NEW(frame_factory_t);
	
	pFactory->nOrder = 0;
	pFactory->nCount = 0;
	pFactory->nUsedSize = 1; // Position 0 not use.
	pFactory->nBoundSize = 1;
	pFactory->Objects.Resize(2, frame_object_t());
	return pFactory;
}

void frame_factory_delete(frame_factory_t* pFactory)
{
	Assert(pFactory != NULL);

	frame_factory_release_all(pFactory);
	K_INNER_DELETE(pFactory);
}

void frame_factory_delete_object(frame_factory_t* pFactory, IObject* pObj)
{
	Assert(pFactory != NULL);
	Assert(pObj != NULL);

	IDataTable* pExtra = pObj->GetExtra();
	IObjectCreator* pCreator = pObj->GetDesc()->GetCreator();

	pCreator->Destroy(pObj);

	if (pExtra)
	{
		K_INNER_DELETE(pExtra);
	}
}

void frame_factory_release_all(frame_factory_t* pFactory)
{
	Assert(pFactory != NULL);
	
	int size = (int)pFactory->Objects.Size();

	// Guarantee delete main object at last.
	for (int i = size - 1; i >= 0; --i)
	{
		if (pFactory->Objects[i].pObject[0] != NULL)
		{
			// Cannot call object Shutdown.
			frame_factory_delete_object(pFactory, 
				pFactory->Objects[i].pObject[0]);
		}
	}

	pFactory->Objects.Clear();
	pFactory->nUsedSize = 1; // Not use position 0.
	pFactory->nBoundSize = 1;
	pFactory->Objects.Resize(2, frame_object_t());
	pFactory->FreeList.Clear();
	pFactory->nCount = 0;
	frame_factory_clear_deleted(pFactory);
}

unsigned int frame_factory_get_order(frame_factory_t* pFactory, 
	unsigned int in_script)
{
	Assert(pFactory != NULL);
	Assert(in_script < 2);

	if (++pFactory->nOrder == 0)
	{
		++pFactory->nOrder;
	}

	return ((pFactory->nOrder & 0x3FFFFFFF) << 1) | in_script;
}

IObject* frame_factory_create_object(frame_factory_t* pFactory, 
	const char* name, const IArgList& args, unsigned int in_script)
{
	Assert(pFactory != NULL);
	Assert(name != NULL);

	IObjectDesc* pObjectDesc = frame_get_object_desc(name);

	if (NULL == pObjectDesc)
	{
		frame_log_print(FRAME_DEFAULT_LOG, 
			"(frame_factory_create_object)Class %s not found.", name);
		return NULL;
	}

	if (pObjectDesc->GetCreator()->IsBaseClass())
	{
		frame_log_print(FRAME_DEFAULT_LOG, 
			"(frame_factory_create_object)Class %s only for base.", name);
		return NULL;
	}

	IObject* pObj = pObjectDesc->GetCreator()->Create();

	if (NULL == pObj)
	{
		return NULL;
	}

	size_t index;

	if (pFactory->FreeList.Empty())
	{
		if (pFactory->nUsedSize == pFactory->Objects.Size())
		{
			// Guarantee container size is power of 2.
			pFactory->Objects.Resize(pFactory->nUsedSize * 2, frame_object_t());
			pFactory->nBoundSize = pFactory->Objects.Size() - 1;
		}

		Assert(pFactory->nUsedSize < pFactory->Objects.Size());

		index = pFactory->nUsedSize;
		pFactory->Objects[index].pObject[0] = pObj;
		pFactory->nUsedSize++;
	}
	else
	{
		index = pFactory->FreeList.Back();
		pFactory->FreeList.PopBack();
		pFactory->Objects[index].pObject[0] = pObj;
	}

	++pFactory->nCount;

	unsigned int serial = frame_factory_get_order(pFactory, in_script);

	pObj->FrameSetDescUID(pObjectDesc, uniqid_t((unsigned int)index, serial));
	//pObj->m_pExtra = K_INNER_NEW(ObjectDataTable);
	pFactory->Objects[index].ObjectId = pObj->GetUID();

	if (!pObj->Startup(args))
	{
		// Object startup failed.
		frame_log_print(FRAME_DEFAULT_LOG, 
			"(frame_factory_create_object)Class %s startup failed.", name);
		pFactory->Objects[index].pObject[0] = NULL;
		pFactory->Objects[index].ObjectId = uniqid_t();
		pFactory->FreeList.PushBack((unsigned int)index);
		--pFactory->nCount;
		frame_factory_delete_object(pFactory, pObj);
		return NULL;
	}

	return pObj;
}

void frame_factory_create_extra(frame_factory_t* pFactory, IObject* pObject)
{
	Assert(pFactory != NULL);
	Assert(pObject != NULL);
	Assert(NULL == pObject->GetExtra());

	pObject->FrameSetExtra(K_INNER_NEW(ObjectDataTable));
}

void frame_factory_set_deleted(frame_factory_t* pFactory, IObject* pObj)
{
	Assert(pFactory != NULL);
	Assert(pObj != NULL);

	if (frame_get_closing())
	{
		// Not allow delete object in system closing.
		Assert(0);
		abort();
	}

	unsigned int index = pObj->GetUID().nIndex;

	Assert(index < pFactory->Objects.Size());
	Assert(pFactory->Objects[index].pObject[0] == pObj);

	pObj->Shutdown();
	pFactory->Objects[index].pObject[0] = NULL;
	pFactory->Objects[index].ObjectId = uniqid_t();
	pFactory->FreeList.PushBack(index);
	--pFactory->nCount;
	pFactory->Deletes.PushBack(pObj);
}

void frame_factory_get_all(frame_factory_t* pFactory, IArgList* result)
{
	Assert(pFactory != NULL);
	Assert(result != NULL);

	result->Clear();

	const size_t object_size = pFactory->Objects.Size();

	for (size_t i = 0; i < object_size; ++i)
	{
		IObject* pObj = pFactory->Objects[i].pObject[0];

		if (NULL == pObj)
		{
			continue;
		}

		result->AddUniqid(pObj->GetUID());
	}
}

size_t frame_factory_get_count(frame_factory_t* pFactory)
{ 
	Assert(pFactory != NULL);
	
	return pFactory->nCount;
}

IObject* frame_factory_find(frame_factory_t* pFactory, const char* name)
{
	Assert(pFactory != NULL);
	Assert(name != NULL);

	const size_t object_size = pFactory->Objects.Size();

	for (size_t i = 0; i < object_size; ++i)
	{
		IObject* pObj = pFactory->Objects[i].pObject[0];

		if (NULL == pObj)
		{
			continue;
		}

		IObjectDesc* pDesc = pObj->GetDesc();

		if (strcmp(pDesc->GetObjectName(), name) == 0)
		{
			return pObj;
		}
	}

	return NULL;
}

void frame_factory_find_list(frame_factory_t* pFactory, const char* name,
	IArgList* result)
{
	Assert(pFactory != NULL);
	Assert(name != NULL);
	Assert(result != NULL);

	result->Clear();

	const size_t object_size = pFactory->Objects.Size();

	for (size_t i = 0; i < object_size; ++i)
	{
		IObject* pObj = pFactory->Objects[i].pObject[0];

		if (NULL == pObj)
		{
			continue;
		}

		IObjectDesc* pDesc = pObj->GetDesc();

		if (strcmp(pDesc->GetObjectName(), name) != 0)
		{
			continue;
		}

		result->AddUniqid(pObj->GetUID());
	}
}

void frame_factory_clear_deleted(frame_factory_t* pFactory)
{
	Assert(pFactory != NULL);

	if (pFactory->Deletes.Empty())
	{
		return;
	}

	// Container may changed when delete any object.
	for (size_t i = 0; i < pFactory->Deletes.Size(); ++i)
	{
		frame_factory_delete_object(pFactory, pFactory->Deletes[i]);
	}

	pFactory->Deletes.Clear();
}

bool frame_factory_dump(frame_factory_t* pFactory, const char* file_name)
{
	Assert(pFactory != NULL);
	Assert(file_name != NULL);

	FILE* fp = plat_file_open(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	int count = 0;
	const size_t object_size = pFactory->Objects.Size();

	for (size_t i = 0; i < object_size; ++i)
	{
		IObject* p = pFactory->Objects[i].pObject[0];

		if (NULL == p)
		{
			continue;
		}

		const char* script_name = "";
		IObjectHook* pHook = p->GetHook();

		if (pHook)
		{
			script_name = pHook->GetName();
		}

		fprintf(fp, "Object:%s script:%s id:%u#%u",
			p->GetDesc()->GetObjectName(), script_name, p->GetUID().nIndex,
			p->GetUID().nOrder);

		// Internal property Name.
		TArgData<4, FrameFactoryAlloc> prop;

		if (frame_get_property(p, "Name", &prop))
		{
			fprintf(fp, " property Name:%s", prop.GetString());
		}

		// Extra property name.
		IDataTable* pExtra = p->GetExtra();

		if (pExtra)
		{
			IArgData* pName = pExtra->GetValue("name");

			if (pName)
			{
				fprintf(fp, " extra name:%s", pName->GetString());
			}
		}

		fprintf(fp, ".\r\n");
		count++;
	}

	fprintf(fp, "Total %d objects.\r\n", count);
	fclose(fp);
	return true;
}
