// frame_object_desc.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_OBJECT_DESC_H
#define __FRAME_OBJECT_DESC_H

#include "../inc/common.h"
#include "../inc/i_object_desc.h"
#include "../inc/auto_string.h"
#include "../inc/literal_hash_table.h"
#include "frame_inner_mem.h"

DEFINE_INNER_ALLOC(FrameObjectDesc);

struct object_property_t;
struct object_method_t;
class IObjectCreator;

// Object property description.

class FramePropertyDesc : public IPropertyDesc
{
public:
	FramePropertyDesc()
	{
		m_nType = 0;
		m_pReadFunc = NULL;
		m_pWriteFunc = NULL;
	}
	
	virtual ~FramePropertyDesc()
	{
	}
	
	// Get property name.
	virtual const char* GetName()
	{
		return m_sName.CString();
	}

	// Get property type.
	virtual int GetType()
	{
		return m_nType;
	}

	// Get function pointer of read property.
	virtual void* GetReadFunc()
	{
		return m_pReadFunc;
	}

	// Get function pointer of write property.
	virtual void* GetWriteFunc()
	{
		return m_pWriteFunc;
	}

	// Set property name.
	void SetName(const char* value)
	{
		m_sName = value;
	}

	// Set property type.
	void SetType(int value)
	{
		m_nType = value;
	}

	// Set function pointer of read property.
	void SetReadFunc(void* value)
	{
		m_pReadFunc = value;
	}

	// Set function pointer of write property.
	void SetWriteFunc(void* value)
	{
		m_pWriteFunc = value;
	}

private:
	TAutoString<char, 16, TCharTraits<char>, FrameObjectDescAlloc> m_sName;
	int m_nType;
	void* m_pReadFunc;
	void* m_pWriteFunc;
};

// Object method description.

class FrameMethodDesc : public IMethodDesc 
{
public:
	FrameMethodDesc()
	{
		m_pMethodFunc = NULL;
		m_bResultArray = false;
	}
	
	virtual ~FrameMethodDesc()
	{
	}
	
	// Get method name.
	virtual const char* GetName()
	{
		return m_sName.CString();
	}

	// Get method function pointer.
	virtual void* GetMethodFunc()
	{
		return m_pMethodFunc;
	}

	// Get flag of method result is array.
	virtual bool GetResultArray()
	{
		return m_bResultArray;
	}

	// Set method name.
	void SetName(const char* value)
	{
		m_sName = value;
	}

	// Set method funtion pointer.
	void SetMethodFunc(void* value)
	{
		m_pMethodFunc = value;
	}

	// Set flag of method result is array.
	void SetResultArray(bool value)
	{
		m_bResultArray = value;
	}

private:
	TAutoString<char, 16, TCharTraits<char>, FrameObjectDescAlloc> m_sName;
	void* m_pMethodFunc;
	bool m_bResultArray;
};

// Object description.

class FrameObjectDesc : public IObjectDesc
{
public:
	FrameObjectDesc();
	virtual ~FrameObjectDesc();

	// Get object creator.
	virtual IObjectCreator* GetCreator();
	// Get parent object class name.
	virtual const char* GetParentName();
	// Get object class name.
	virtual const char* GetObjectName();
	// Get parent object description.
	virtual IObjectDesc* GetParent();
	// Test derive from.
	virtual bool DeriveFrom(const char* name);
	// Get property description in object class or parent object class.
	virtual IPropertyDesc* FindPropertyDesc(const char* name);
	// Get property names in object class or parent object class.
	virtual void GetPropertyAll(IArgList* result);
	// Get method description in object class or parent object class.
	virtual IMethodDesc* FindMethodDesc(const char* name);
	// Get method names in object class or parent object class.
	virtual void GetMethodAll(IArgList* result);

	// Parent descriptor.
	void SetParentDesc(FrameObjectDesc* pParent);
	FrameObjectDesc* GetParentDesc() const;
	
	// Get property number.
	size_t GetPropertyCount() const;
	// Get property names.
	void GetPropertyList(IArgList* result) const;
	// Get property description.
	FramePropertyDesc* GetPropertyDesc(const char* name) const;
	
	// Get method number.
	size_t GetMethodCount() const;
	// Get method names.
	void GetMethodList(IArgList* result) const;
	// Get method description.
	FrameMethodDesc* GetMethodDesc(const char* name) const;
	
	// Set object creator.
	void SetCreator(IObjectCreator* value);
	// Set parent name.
	void SetParentName(const char* value);
	// Set name of object.
	void SetObjectName(const char* value);

	// Add properties(reverse order).
	size_t AddPropertyLink(object_property_t* pPropertyLink);
	// Add methods(reverse order).
	size_t AddMethodLink(object_method_t* pMethodLink);

	// Link object description.
	void SetNext(FrameObjectDesc* value) { m_pNext = value; }
	FrameObjectDesc* GetNext() const { return m_pNext; }

	// Hash value.
	void SetHash(size_t value) { m_nHash = value; }
	size_t GetHash() const { return m_nHash; }

private:
	FrameObjectDesc(const FrameObjectDesc&);
	FrameObjectDesc& operator=(const FrameObjectDesc&);
	
	void InnerGetPropertyList(IArgList* result) const;
	void InnerGetMethodList(IArgList* result) const;

private:
	FrameObjectDesc* m_pParent;
	FrameObjectDesc* m_pNext;
	size_t m_nHash;
	IObjectCreator* m_pCreator;
	TAutoString<char, 16, TCharTraits<char>, 
		FrameObjectDescAlloc> m_sParentName;
	TAutoString<char, 16, TCharTraits<char>,
		FrameObjectDescAlloc> m_sObjectName;
	FramePropertyDesc* m_pPropertyDescs;
	size_t m_nPropertyCount;
	FrameMethodDesc* m_pMethodDescs;
	size_t m_nMethodCount;
	TLiteralHashTable<char, size_t, TCharTraits<char>, 
		FrameObjectDescAlloc> m_PropertyIndices;
	TLiteralHashTable<char, size_t, TCharTraits<char>, 
		FrameObjectDescAlloc> m_MethodIndices;
};

#endif // __FRAME_OBJECT_DESC_H
