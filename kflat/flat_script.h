// flat_script.h
// Created by lulimin on 2021/1/6.

#ifndef __FLAT_SCRIPT_H
#define __FLAT_SCRIPT_H

#include "../inc/common.h"
#include "../inc/i_object_hook.h"
#include "../inc/pod_array.h"
#include "../inc/frame_types.h"

#define OBJECT_HOOK_TYPE_FLAT_SCRIPT 1

// FlatScript

class FlatScript : public IObjectHook
{
public:
	static FlatScript* CreateInstance();

private:
	struct event_t 
	{
		unsigned int nHash;
		char* pEvent;
		char* pFile;
		char* pFunc;
	};

public:
	FlatScript();
	virtual ~FlatScript();

	// Destroy self.
	virtual void Release();
	// Get type.
	virtual int GetType() {	return OBJECT_HOOK_TYPE_FLAT_SCRIPT; }
	// Get name.
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

	// Script file name.
	void SetScriptFile(const char* name);
	
private:
	FlatScript(const FlatScript&);
	FlatScript& operator=(const FlatScript&);

	// Find index of event.
	bool FindEventIndex(const char* event, size_t* pIndex) const;
	
private:
	FrameString m_sScriptFile;
	TPodArray<event_t*, 1, FrameAlloc> m_EventHooks;
};

#endif // __FLAT_SCRIPT_H
