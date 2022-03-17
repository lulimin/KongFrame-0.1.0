// flat_input_service.cpp
// Created by lulimin on 2020/9/23.

#include "flat_input_service.h"

/// \file flat_input_service.cpp
/// \brief User input router service.

/// service: FlatInputService
/// \brief User input router service.
DEF_SERVICE(FlatInputService);

// FlatInputService

FlatInputService::FlatInputService()
{
}

FlatInputService::~FlatInputService()
{
}

bool FlatInputService::Startup()
{
	return true;
}

bool FlatInputService::Shutdown()
{
	return true;
}

bool FlatInputService::AddProcessor(processor_func_t func, void* context,
	const uniqid_t& id)
{
	Assert(func != NULL);

	processor_t data;

	data.pFunc = func;
	data.pContext = context;
	data.ObjectId = id;
	m_Processors.PushBack(data);
	return true;
}

bool FlatInputService::RemoveProcessor(processor_func_t func, void* context,
	const uniqid_t& id)
{
	for (size_t i = 0; i < m_Processors.Size(); ++i)
	{
		processor_t* p = &m_Processors[i];
		
		if ((p->pFunc == func) && (p->pContext == context) &&
			uniqid_equal(p->ObjectId,id))
		{
			m_Processors.Remove(i);
			return true;
		}
	}

	return false;
}

bool FlatInputService::ProcessInput(const input_t& msg)
{
	for (size_t i = 0; i < m_Processors.Size(); ++i)
	{
		processor_t* p = &m_Processors[i];
		void* pContext = p->pContext;

		if (NULL == pContext)
		{
			pContext = frame_get_object(p->ObjectId);

			if (NULL == pContext)
			{
				continue;
			}
		}

		int res = p->pFunc(pContext, msg);

		if (res != 0)
		{
			return true;
		}
	}
	
	return false;
}
