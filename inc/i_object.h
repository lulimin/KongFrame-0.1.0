// i_object.h
// Created by lulimin on 2019/2/14.

#ifndef __I_OBJECT_H
#define __I_OBJECT_H

#include "common.h"
#include "uniqid.h"
#include "i_object_hook.h"

// Object base interface.

class IArgList;
class IDataTable;
class IObjectDesc;

class IObject
{
public:
	// Validate parent object class only.
	static void FrameCheckValidate() {}

public:
	IObject()
	{
		m_pExtra = NULL;
		m_pDesc = NULL;
		m_pHook = NULL;
	}

	virtual ~IObject()
	{
		if (m_pHook)
		{
			m_pHook->Release();
		}
	}

	// Object initialize.
	virtual bool Startup(const IArgList& args) = 0;
	// Object shutdown.
	virtual bool Shutdown() = 0;
	// Execute per frame.
	virtual void PreWork(double frame_time) {}
	virtual void Work(double frame_time) {}
	virtual void PostWork(double frame_time) {}

	// Delete self.
	virtual void Release()
	{
		frame_delete_object(m_UID);
	}

	// Get super class id.
	virtual int GetSuperClassID() { return 0; }
	// Get class id.
	virtual int GetClassID() { return 0; }
	
	// Get extra property table.
	IDataTable* GetExtra() const
	{
		return m_pExtra;
	}

	// Get object description.
	IObjectDesc* GetDesc() const
	{
		return m_pDesc;
	}

	// Set event processor interface.
	void SetHook(IObjectHook* value)
	{
		if (m_pHook)
		{
			m_pHook->Release();
		}
		
		m_pHook = value;
	}

	// Get event processor interface.
	IObjectHook* GetHook() const
	{
		return m_pHook;
	}
	
	// Get object id.
	uniqid_t GetUID() const
	{
		return m_UID;
	}

	// Donot call, use by frame only.
	void FrameSetExtra(IDataTable* value)
	{
		Assert(value != NULL);
		Assert(NULL == m_pExtra);
		
		m_pExtra = value;
	}

	// Donot call, use by frame only.
	void FrameSetDescUID(IObjectDesc* pDesc, const uniqid_t& id)
	{
		Assert(pDesc != NULL);
		Assert(NULL == m_pDesc);

		m_pDesc = pDesc;
		m_UID = id;
	}

private:
	IObject(const IObject&);
	IObject& operator=(const IObject&);
	
private:
	IDataTable* m_pExtra;
	IObjectDesc* m_pDesc;
	IObjectHook* m_pHook;
	uniqid_t m_UID;
};

#endif // __I_OBJECT_H
