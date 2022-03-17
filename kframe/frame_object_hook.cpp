// frame_object_hook.cpp
// Created by lulimin on 2019/2/18.

#include "frame_object_hook.h"
#include "frame_imp.h"
#include "../inc/i_arg_list.h"

// FrameObjectHook

FrameObjectHook* FrameObjectHook::CreateInstance()
{
	return K_INNER_NEW(FrameObjectHook);
}

FrameObjectHook::FrameObjectHook()
{
}

FrameObjectHook::~FrameObjectHook()
{
	for (size_t i = 0; i < m_EventHooks.Size(); ++i)
	{
		event_t* p = m_EventHooks[i];

		if (p)
		{
			K_INNER_FREE(p);
		}
	}
}

void FrameObjectHook::Release()
{
	K_INNER_DELETE(this);
}

const char* FrameObjectHook::GetName()
{
	return m_sName.CString();
}

bool FrameObjectHook::FindEventProcessor(const char* event)
{
	Assert(event != NULL);
	
	return m_EventHookIndices.Exists(event);
}

bool FrameObjectHook::AddEventProcessor(const char* event, const char* file,
	const char* func)
{
	Assert(event != NULL);
	Assert(file != NULL);
	Assert(func != NULL);

	size_t index;

	if (!m_FreeList.Empty())
	{
		index = m_FreeList.Back();
		m_FreeList.PopBack();
	}
	else
	{
		index = m_EventHooks.Size();
		m_EventHooks.PushBack(NULL);
	}

	size_t event_name_size = strlen(event) + 1;
	size_t file_name_size = strlen(file) + 1;
	size_t func_name_size = strlen(func) + 1;
	size_t size = sizeof(event_t) + event_name_size + file_name_size +
		func_name_size;
	event_t* p = (event_t*)K_INNER_ALLOC(size);

	p->nSize = size;
	p->pEvent = (char*)p + sizeof(event_t);
	p->pFile = p->pEvent + event_name_size;
	p->pFunc = p->pFile + file_name_size;
	memcpy(p->pEvent, event, event_name_size);
	memcpy(p->pFile, file, file_name_size);
	memcpy(p->pFunc, func, func_name_size);
	m_EventHooks[index] = p;
	m_EventHookIndices.Add(p->pEvent, index);
	return true;
}

bool FrameObjectHook::RemoveEventProcessor(const char* event)
{
	size_t index;

	if (!m_EventHookIndices.GetData(event, &index))
	{
		return false;
	}

	event_t* p = m_EventHooks[index];

	m_EventHookIndices.RemoveData(p->pEvent, index);
	m_EventHooks[index] = NULL;
	m_FreeList.PushBack(index);
	K_INNER_FREE(p);
	return true;
}

bool FrameObjectHook::ProcessEvent(const uniqid_t& id, int type,
	const char* event, const IArgList& args, IArgList* result)
{
	Assert(event != NULL);
	
	size_t index;

	if (!m_EventHookIndices.GetData(event, &index))
	{
		return false;
	}

	event_t* p = m_EventHooks[index];
	const char* file = p->pFile;

	if (util_string_empty(file))
	{
		file = m_sName.CString();
	}

	return frame_run_routine(file, p->pFunc, id, args, result);
}

void FrameObjectHook::SetName(const char* value)
{
	Assert(value != NULL);
	
	m_sName = value;
}
