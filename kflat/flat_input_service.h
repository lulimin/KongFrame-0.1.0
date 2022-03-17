// flat_input_service.h
// Created by lulimin on 2020/9/23.

#ifndef __FLAT_INPUT_SERVICE_H
#define __FLAT_INPUT_SERVICE_H

#include "../kgraphic/i_input_service.h"
#include "../inc/pod_array.h"

// FlatInputService

class FlatInputService : public IInputService
{
private:
	struct processor_t
	{
		processor_func_t pFunc;
		void* pContext;
		uniqid_t ObjectId;
	};

public:
	FlatInputService();
	virtual ~FlatInputService();

	virtual bool Startup();
	virtual bool Shutdown();

	// Add user input processor.
	virtual bool AddProcessor(processor_func_t func, void* context,
		const uniqid_t& id);
	// Remove user input processor.
	virtual bool RemoveProcessor(processor_func_t func, void* context,
		const uniqid_t& id);
	// Process user input.
	virtual bool ProcessInput(const input_t& msg);

private:
	TPodArray<processor_t, 1, FrameAlloc> m_Processors;
};

#endif // __FLAT_INPUT_SERVICE_H
