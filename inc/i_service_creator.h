// i_service_creator.h
// Created by lulimin on 2019/2/14.

#ifndef __I_SERVICE_CREATOR_H
#define __I_SERVICE_CREATOR_H

#include "common.h"

// Interface of service creator.

class IService;

class IServiceCreator
{
public:
	IServiceCreator() { m_pNext = NULL; }
	IServiceCreator(IServiceCreator* pNext) { m_pNext = pNext; }
	virtual ~IServiceCreator() {}

	// Get service name.
	virtual const char* GetName() = 0;
	// Create service.
	virtual IService* Create() = 0;
	// Destroy service.
	virtual void Destroy(IService* p) = 0;
	
	// Set next creator.
	void SetNext(IServiceCreator* value) { m_pNext = value; }
	// Get next creator.
	IServiceCreator* GetNext() const { return m_pNext; }

private:
	IServiceCreator* m_pNext;
};

#endif // __I_SERVICE_CREATOR_H
