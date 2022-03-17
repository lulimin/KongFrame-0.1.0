// i_object_hook.h
// Created by lulimin on 2019/2/14.

#ifndef __I_OBJECT_HOOK_H
#define __I_OBJECT_HOOK_H

#include "common.h"
#include "uniqid.h"

// Object hook type for internal using.
#define OBJECT_HOOK_TYPE_INTERNAL 0

// Object event processor interface.

class IArgList;

class IObjectHook
{
public:
	virtual ~IObjectHook() {}

	// Destroy self.
	virtual void Release() = 0;
	// Get type of hook.
	virtual int GetType() = 0;
	// Get name of hook.
	virtual const char* GetName() = 0;
	// Find event processor.
	virtual bool FindEventProcessor(const char* event) = 0;
	// Add event processor.
	virtual bool AddEventProcessor(const char* event, const char* file,
		const char* func) = 0;
	// Remove event processor.
	virtual bool RemoveEventProcessor(const char* event) = 0;
	// Process event.
	virtual bool ProcessEvent(const uniqid_t& id, int type, const char* event,
		const IArgList& args, IArgList* result) = 0;
};

#endif // __I_OBJECT_HOOK_H
