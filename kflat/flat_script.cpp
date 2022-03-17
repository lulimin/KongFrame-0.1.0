// flat_script.cpp
// Created by lulimin on 2021/1/6.

#include "flat_script.h"
#include "../inc/i_arg_list.h"

// FlatScript

FlatScript* FlatScript::CreateInstance()
{
	return K_NEW(FlatScript);
}

FlatScript::FlatScript()
{
}

FlatScript::~FlatScript()
{
	for (size_t i = 0; i < m_EventHooks.Size(); ++i)
	{
		K_FREE(m_EventHooks[i]);
	}
}

void FlatScript::Release()
{
	K_DELETE(this);
}

const char* FlatScript::GetName()
{
	return m_sScriptFile.CString();
}

bool FlatScript::FindEventProcessor(const char* event)
{
	Assert(event != NULL);

	return this->FindEventIndex(event, NULL);
}

bool FlatScript::AddEventProcessor(const char* event, const char* file,
	const char* func)
{
	Assert(event != NULL);
	Assert(file != NULL);
	Assert(func != NULL);

	size_t event_name_size = strlen(event) + 1;
	size_t file_name_size = strlen(file) + 1;
	size_t func_name_size = strlen(func) + 1;
	size_t size = sizeof(event_t) + event_name_size + file_name_size + 
		func_name_size;
	event_t* p = (event_t*)K_ALLOC(size);

	p->nHash = util_string_hash(event);
	p->pEvent = (char*)p + sizeof(event_t);
	p->pFile = p->pEvent + event_name_size;
	p->pFunc = p->pFile + file_name_size;
	memcpy(p->pEvent, event, event_name_size);
	memcpy(p->pFile, file, file_name_size);
	memcpy(p->pFunc, func, func_name_size);
	m_EventHooks.PushBack(p);
	return true;
}

bool FlatScript::RemoveEventProcessor(const char* event)
{
	Assert(event != NULL);

	size_t index;

	if (!this->FindEventIndex(event, &index))
	{
		return false;
	}

	K_FREE(m_EventHooks[index]);
	m_EventHooks.Remove(index);
	return true;
}

bool FlatScript::ProcessEvent(const uniqid_t& id, int type, const char* event,
	const IArgList& args, IArgList* result)
{
	Assert(event != NULL);
	
	size_t index;

	if (!this->FindEventIndex(event, &index))
	{
		return false;
	}

	event_t* pEvent = m_EventHooks[index];
	const char* file = pEvent->pFile;

	if (util_string_empty(file))
	{
		file = this->GetName();
	}

	return frame_run_routine(file, pEvent->pFunc, id, args, result);
}

void FlatScript::SetScriptFile(const char* name)
{
	Assert(name != NULL);

	m_sScriptFile = name;
}

bool FlatScript::FindEventIndex(const char* event, size_t* pIndex) const
{
	Assert(event != NULL);
	
	unsigned int hash = util_string_hash(event);

	for (size_t i = 0; i < m_EventHooks.Size(); ++i)
	{
		event_t* p = m_EventHooks[i];

		if ((p->nHash == hash) && (strcmp(p->pEvent, event) == 0))
		{
			if (pIndex)
			{
				*pIndex = i;
			}

			return true;
		}
	}

	return false;
}
