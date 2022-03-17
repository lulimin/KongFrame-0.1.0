// i_object_creator.h
// Created by lulimin on 2019/2/14.

#ifndef __I_OBJECT_CREATOR_H
#define __I_OBJECT_CREATOR_H

#include "common.h"

// Object creator.

class IObject;

// Object property description.
struct object_property_t
{
	const char* pName;
	int nType;
	void* pReadFunc;
	void* pWriteFunc;
	object_property_t* pNext;
};

// Object method description.
struct object_method_t
{
	const char* pName;
	void* pMethodFunc;
	bool bResultArray;
	object_method_t* pNext;
};

class IObjectCreator
{
public:
	IObjectCreator()
	{
		m_pNext = NULL;
		m_pProperty = NULL;
		m_pMethod = NULL;
	}
	
	IObjectCreator(IObjectCreator* pNext)
	{  
		m_pNext = pNext; 
		m_pProperty = NULL;
		m_pMethod = NULL;
	}

	virtual ~IObjectCreator() {}

	// Whether virtual base object class, derive from only.
	virtual bool IsBaseClass() = 0;
	// Get parent object class name.
	virtual const char* GetParent() = 0;
	// Get object class name.
	virtual const char* GetName() = 0;
	// Create object.
	virtual IObject* Create() = 0;
	// Delete object.
	virtual void Destroy(IObject* p) = 0;
	
	// Set next creator.
	void SetNext(IObjectCreator* value) { m_pNext = value; }
	// Get next creator.
	IObjectCreator* GetNext() const { return m_pNext; }
	// Get property description link.
	object_property_t* GetPropertyLink() const { return m_pProperty; }
	// Set property description link.
	void SetPropertyLink(object_property_t* value) { m_pProperty = value; }
	// Get method description link.
	object_method_t* GetMethodLink() const { return m_pMethod; }
	// Set method description link.
	void SetMethodLink(object_method_t* value) { m_pMethod = value; }

private:
	IObjectCreator* m_pNext;
	object_property_t* m_pProperty;
	object_method_t* m_pMethod;
};

#endif // __I_OBJECT_CREATOR_H
