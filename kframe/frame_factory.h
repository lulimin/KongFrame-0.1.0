// frame_factory.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_FACTORY_H
#define __FRAME_FACTORY_H

#include "../inc/common.h"
#include "../inc/uniqid.h"
#include "../inc/pod_array.h"
#include "frame_inner_mem.h"

DEFINE_INNER_ALLOC(FrameFactory);

class IArgList;
class IObject;

struct frame_object_t
{
	uniqid_t ObjectId;
	IObject* pObject[2];

	frame_object_t()
	{
		pObject[0] = NULL;
		pObject[1] = NULL;
	}
};

struct frame_factory_t
{
	TPodArray<frame_object_t, 1, FrameFactoryAlloc> Objects;
	TPodArray<unsigned int, 1, FrameFactoryAlloc> FreeList;
	TPodArray<IObject*, 1, FrameFactoryAlloc> Deletes;
	unsigned int nOrder;
	size_t nCount;
	size_t nUsedSize;
	size_t nBoundSize;
};

// Whether create in script.
inline bool frame_factory_create_in_script(const uniqid_t& id)
{
	return id.nOrder & 0x1;
}

// Get object by id.
inline IObject* frame_factory_get(frame_factory_t* pFactory,
	const uniqid_t& id)
{
	// Ignore bound test.
	size_t index = id.nIndex & pFactory->nBoundSize;
	bool selector = uniqid_not_equal(pFactory->Objects[index].ObjectId, id);
	
	return pFactory->Objects[index].pObject[(size_t)selector];
}

// Create object factory.
frame_factory_t* frame_factory_create();
// Delete object factory.
void frame_factory_delete(frame_factory_t* pFactory);
// Create object.
IObject* frame_factory_create_object(frame_factory_t* pFactory, 
	const char* name, const IArgList& args, unsigned int in_script);
// Create extra property table.
void frame_factory_create_extra(frame_factory_t* pFactory, IObject* pObject);
// Set object deleted flag.
void frame_factory_set_deleted(frame_factory_t* pFactory, IObject* pObject);
// Get all objects.
void frame_factory_get_all(frame_factory_t* pFactory, IArgList* result);
// Get object number.
size_t frame_factory_get_count(frame_factory_t* pFactory);
// Search object.
IObject* frame_factory_find(frame_factory_t* pFactory, const char* name);
void frame_factory_find_list(frame_factory_t* pFactory, const char* name,
	IArgList* result);
// Clear deleted objects.
void frame_factory_clear_deleted(frame_factory_t* pFactory);
// Release all objects.
void frame_factory_release_all(frame_factory_t* pFactory);
// Dump all objects.
bool frame_factory_dump(frame_factory_t* pFactory, const char* file_name);

#endif // __FRAME_FACTORY_H
