// i_service.h
// Created by lulimin on 2019/2/14.

#ifndef __I_SERVICE_H
#define __I_SERVICE_H

#include "common.h"

// Service interface.

class IServiceCreator;

class IService 
{
public:
	IService()
	{
		m_pCreator = NULL;
	}
	
	virtual ~IService()
	{
	}

	// Initialize service.
	virtual bool Startup() = 0;
	// Shutdown service.
	virtual bool Shutdown() = 0;
	
	// Destroy self.
	virtual void Release()
	{
		frame_release_service(this);
	}
	
	// Get service creator.
	IServiceCreator* GetCreator() const
	{
		return m_pCreator;
	}

	// Donot call, used by frame only.
	void FrameSetCreator(IServiceCreator* value)
	{
		Assert(value != NULL);
		Assert(NULL == m_pCreator);

		m_pCreator = value;
	}

private:
	IService(const IService&);
	IService& operator=(const IService&);
	
private:
	IServiceCreator* m_pCreator;
};

#endif // __I_SERVICE_H
