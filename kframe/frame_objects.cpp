// frame_objects.cpp
// Created by lulimin on 2019/2/18.

#include <new>
#include "frame_objects.h"
#include "frame_imp.h"
#include "frame_inner_mem.h"
#include "frame_object_desc.h"
#include "../inc/utility.h"
#include "../inc/pod_array.h"
#include "../inc/literal_hash_table.h"
#include "../inc/arg_list.h"
#include "../inc/auto_buffer.h"
#include "../inc/platform.h"
#include "../inc/i_object.h"
#include "../inc/i_object_creator.h"

DEFINE_INNER_ALLOC(FrameObjects);

typedef TArgList<8, 128, FrameInnerAlloc> InnerArgList;

struct frame_objects_t
{
	bool bModified;
	TPodArray<IObject*, 1, FrameObjectsAlloc> Works;
	TPodArray<IObject*, 1, FrameObjectsAlloc> CurrentWorks;
	TPodArray<FrameObjectDesc*, 8, FrameObjectsAlloc> ObjectDescList;
	TLiteralHashTable<char, size_t, TCharTraits<char>,
		FrameObjectsAlloc> ObjectDescIndices;
};

frame_objects_t* frame_objects_create()
{
	frame_objects_t* pObjects = K_INNER_NEW(frame_objects_t);
	
	pObjects->bModified = true;
	return pObjects;
}

void frame_objects_delete(frame_objects_t* pObjects)
{
	Assert(pObjects != NULL);
	
	for (size_t i = 0; i < pObjects->ObjectDescList.Size(); ++i)
	{
		FrameObjectDesc* pObjectDesc = pObjects->ObjectDescList[i];

		K_INNER_DELETE(pObjectDesc);
	}

	K_INNER_DELETE(pObjects);
}

bool frame_objects_add_creator(frame_objects_t* pObjects, 
	IObjectCreator* pCreator)
{
	Assert(pObjects != NULL);
	Assert(pCreator != NULL);

	if (pObjects->ObjectDescIndices.Exists(pCreator->GetName()))
	{
		frame_log_string(FRAME_DEFAULT_LOG, 
			"(frame_objects_add_creator)Object name repeat.");
		frame_log_string(FRAME_DEFAULT_LOG, pCreator->GetName());
	}

	FrameObjectDesc* pObjectDesc = K_INNER_NEW(FrameObjectDesc);

	pObjectDesc->SetCreator(pCreator);
	pObjectDesc->SetParentName(pCreator->GetParent());
	pObjectDesc->SetObjectName(pCreator->GetName());
	pObjectDesc->AddPropertyLink(pCreator->GetPropertyLink());
	pObjectDesc->AddMethodLink(pCreator->GetMethodLink());
	pObjects->ObjectDescIndices.Add(pCreator->GetName(), 
		pObjects->ObjectDescList.Size());
	pObjects->ObjectDescList.PushBack(pObjectDesc);
	return true;
}

void frame_objects_set_all_parent(frame_objects_t* pObjects)
{
	Assert(pObjects != NULL);

	const size_t object_desc_num = pObjects->ObjectDescList.Size();

	for (size_t i = 0; i < object_desc_num; ++i)
	{
		FrameObjectDesc* pObjectDesc = pObjects->ObjectDescList[i];
		const char* parent_name = pObjectDesc->GetParentName();

		if (strcmp(parent_name, "IObject") == 0)
		{
			continue;
		}

		FrameObjectDesc* pParent = frame_objects_get_desc(pObjects,
			parent_name);

		if (NULL == pParent)
		{
			// Parent class not exists.
			frame_log_string(FRAME_DEFAULT_LOG, 
				"(frame_objects_set_all_parent)No parent.");
			frame_log_string(FRAME_DEFAULT_LOG, parent_name);
			frame_log_string(FRAME_DEFAULT_LOG, pObjectDesc->GetObjectName());
			continue;
		}
		
		pObjectDesc->SetParentDesc(pParent);
	}
}

FrameObjectDesc* frame_objects_get_desc(frame_objects_t* pObjects,
	const char* name)
{
	Assert(pObjects != NULL);

	size_t index;

	if (!pObjects->ObjectDescIndices.GetData(name, &index))
	{
		return NULL;
	}

	return pObjects->ObjectDescList[index];
}

bool frame_objects_add_work(frame_objects_t* pObjects, IObject* pObj)
{
	Assert(pObjects != NULL);
	Assert(pObj != NULL);

	pObjects->Works.PushBack(pObj);
	pObjects->bModified = true;
	return true;
}

bool frame_objects_remove_work(frame_objects_t* pObjects, IObject* pObj)
{
	Assert(pObjects != NULL);
	Assert(pObj != NULL);

	const size_t size = pObjects->Works.Size();

	for (size_t i = 0; i < size; ++i)
	{
		if (pObjects->Works[i] == pObj)
		{
			pObjects->Works.Remove(i);
			pObjects->bModified = true;
			return true;
		}
	}

	return false;
}

void frame_objects_pre_work(frame_objects_t* pObjects, double frame_time)
{
	Assert(pObjects != NULL);

	if (pObjects->bModified)
	{
		// Update execute list.
		size_t new_size = pObjects->Works.Size();

		pObjects->CurrentWorks.Resize(new_size, NULL);

		if (new_size > 0)
		{
			memcpy(&pObjects->CurrentWorks[0], &pObjects->Works[0],
				sizeof(IObject*) * new_size);
		}

		pObjects->bModified = false;
	}

	size_t size = pObjects->CurrentWorks.Size();
	IObject** data = pObjects->CurrentWorks.Data();

	for (size_t i = 0; i < size; ++i)
	{
		data[i]->PreWork(frame_time);
	}
}

void frame_objects_work(frame_objects_t* pObjects, double frame_time)
{
	Assert(pObjects != NULL);

	if (pObjects->bModified)
	{
		// Update execute list.
		size_t new_size = pObjects->Works.Size();

		pObjects->CurrentWorks.Resize(new_size, NULL);

		if (new_size > 0)
		{
			memcpy(&pObjects->CurrentWorks[0], &pObjects->Works[0],
				sizeof(IObject*) * new_size);
		}

		pObjects->bModified = false;
	}

	size_t size = pObjects->CurrentWorks.Size();
	IObject** data = pObjects->CurrentWorks.Data();

	for (size_t i = 0; i < size; ++i)
	{
		data[i]->Work(frame_time);
	}
}

void frame_objects_post_work(frame_objects_t* pObjects, double frame_time)
{
	Assert(pObjects != NULL);

	if (pObjects->bModified)
	{
		// Update execute list.
		size_t new_size = pObjects->Works.Size();

		pObjects->CurrentWorks.Resize(new_size, NULL);

		if (new_size > 0)
		{
			memcpy(&pObjects->CurrentWorks[0], &pObjects->Works[0],
				sizeof(IObject*) * new_size);
		}

		pObjects->bModified = false;
	}

	size_t size = pObjects->CurrentWorks.Size();
	IObject** data = pObjects->CurrentWorks.Data();

	for (size_t i = 0; i < size; ++i)
	{
		data[i]->PostWork(frame_time);
	}
}

bool frame_objects_dump(frame_objects_t* pObjects, const char* file_name)
{
	Assert(pObjects != NULL);

	FILE* fp = plat_file_open(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	const size_t object_desc_num = pObjects->ObjectDescList.Size();

	for (size_t i = 0; i < object_desc_num; ++i)
	{
		FrameObjectDesc* pDesc = pObjects->ObjectDescList[i];

		fprintf(fp, "Object:%s parent:%s property:%zd method:%zd.\r\n",
			pDesc->GetObjectName(), pDesc->GetParentName(),
			pDesc->GetPropertyCount(), pDesc->GetMethodCount());

		InnerArgList properties;

		pDesc->GetPropertyList(&properties);

		for (size_t k = 0; k < properties.GetCount(); ++k)
		{
			const char* prop_name = properties.GetString(k);
			FramePropertyDesc* pPropDesc = pDesc->GetPropertyDesc(prop_name);
			int prop_type = pPropDesc->GetType();
			
			fprintf(fp, "\tproperty:%s[%s]\r\n", prop_name, 
				util_get_type_name(prop_type));
		}

		InnerArgList methods;

		pDesc->GetMethodList(&methods);

		for (size_t j = 0; j < methods.GetCount(); ++j)
		{
			fprintf(fp, "\tmethod:%s.\r\n", methods.GetString(j));
		}
	}

	fprintf(fp, "Total %zd object descriptions.\r\n", object_desc_num);
	fclose(fp);
	return true;
}
