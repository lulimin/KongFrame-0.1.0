// i_object_desc.h
// Created by lulimin on 2019/2/14.

#ifndef __I_OBJECT_DESC_H
#define __I_OBJECT_DESC_H

#include "common.h"
#include "i_arg_list.h"

// Object property description interface.

class IPropertyDesc
{
public:
	virtual ~IPropertyDesc() {}

	virtual const char* GetName() = 0;
	virtual int GetType() = 0;
	virtual void* GetReadFunc() = 0;
	virtual void* GetWriteFunc() = 0;
};

// Object method description interface.

class IMethodDesc
{
public:
	virtual ~IMethodDesc() {}

	virtual const char* GetName() = 0;
	virtual void* GetMethodFunc() = 0;
	virtual bool GetResultArray() = 0;
};

// Object description interface.

class IObjectCreator;

class IObjectDesc
{
public:
	virtual ~IObjectDesc() {}

	// Get object class creator.
	virtual IObjectCreator* GetCreator() = 0;
	// Get parent object class name.
	virtual const char* GetParentName() = 0;
	// Get object class name.
	virtual const char* GetObjectName() = 0;
	// Get parent object description.
	virtual IObjectDesc* GetParent() = 0;
	// Whether specified object class or sub class.
	virtual bool DeriveFrom(const char* name) = 0;
	// Get property description in this and parent object class.
	virtual IPropertyDesc* FindPropertyDesc(const char* name) = 0;
	// Get property name list in this and parent object class.
	virtual void GetPropertyAll(IArgList* result) = 0;
	// Get method description in this and parent object class.
	virtual IMethodDesc* FindMethodDesc(const char* name) = 0;
	// Get mdthod name list in this and parent object class.
	virtual void GetMethodAll(IArgList* result) = 0;
};

#endif // __I_OBJECT_DESC_H
