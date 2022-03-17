// frame_object_hook.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_OBJECT_HOOK_H
#define __FRAME_OBJECT_HOOK_H

#include "../inc/common.h"
#include "../inc/i_object_hook.h"
#include "../inc/auto_string.h"
#include "../inc/pod_array.h"
#include "../inc/literal_hash_table.h"
#include "frame_inner_mem.h"

DEFINE_INNER_ALLOC(FrameObjectHook);

// Default object hook.

class FrameObjectHook : public IObjectHook
{
public:
	static FrameObjectHook* CreateInstance();

private:
	struct event_t 
	{
		size_t nSize;
		char* pEvent;
		char* pFile;
		char* pFunc;
	};

public:
	FrameObjectHook();
	virtual ~FrameObjectHook();

	// Destroy self.
	virtual void Release();
	// Get hook type.
	virtual int GetType() {	return OBJECT_HOOK_TYPE_INTERNAL; }
	// Get hook name.
	virtual const char* GetName();
	// Find event processor.
	virtual bool FindEventProcessor(const char* event);
	// Add event processor.
	virtual bool AddEventProcessor(const char* event, const char* file,
		const char* func);
	// Remove event processor.
	virtual bool RemoveEventProcessor(const char* event);
	// Process event.
	virtual bool ProcessEvent(const uniqid_t& id, int type, const char* event,
		const IArgList& args, IArgList* result);

	// Set hook name.
	void SetName(const char* value);
	
private:
	FrameObjectHook(const FrameObjectHook&);
	FrameObjectHook& operator=(const FrameObjectHook&);
	
private:
	TAutoString<char, 16, TCharTraits<char>, FrameObjectHookAlloc> m_sName;
	TPodArray<event_t*, 1, FrameObjectHookAlloc> m_EventHooks;
	TPodArray<size_t, 1, FrameObjectHookAlloc> m_FreeList;
	TLiteralHashTable<char, size_t, TCharTraits<char>, 
		FrameObjectHookAlloc> m_EventHookIndices;
};

#endif // __FRAME_OBJECT_HOOK_H
