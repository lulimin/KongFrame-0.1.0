// frame_object_desc.cpp
// Created by lulimin on 2019/2/18.

#include <new>
#include "frame_object_desc.h"
#include "frame_imp.h"
#include "../inc/i_object.h"
#include "../inc/i_object_creator.h"

// FrameObjectDesc

FrameObjectDesc::FrameObjectDesc()
{
	m_pParent = NULL;
	m_pNext = NULL;
	m_nHash = 0;
	m_pCreator = NULL;
	m_pPropertyDescs = NULL;
	m_nPropertyCount = 0;
	m_pMethodDescs = NULL;
	m_nMethodCount = 0;
}

FrameObjectDesc::~FrameObjectDesc()
{
	if (m_pPropertyDescs) 
	{
		for (size_t i = 0; i < m_nPropertyCount; ++i)
		{
			m_pPropertyDescs[i].~FramePropertyDesc();
		}
		
		K_INNER_FREE(m_pPropertyDescs);
	}

	if (m_pMethodDescs)
	{
		for (size_t i = 0; i < m_nMethodCount; ++i)
		{
			m_pMethodDescs[i].~FrameMethodDesc();
		}

		K_INNER_FREE(m_pMethodDescs);
	}
}

IObjectCreator* FrameObjectDesc::GetCreator()
{
	return m_pCreator;
}

const char* FrameObjectDesc::GetParentName()
{
	return m_sParentName.CString();
}

const char* FrameObjectDesc::GetObjectName()
{
	return m_sObjectName.CString();
}

IObjectDesc* FrameObjectDesc::GetParent()
{
	return m_pParent;
}

bool FrameObjectDesc::DeriveFrom(const char* name)
{
	Assert(name != NULL);

	if (strcmp(m_sObjectName.CString(), name) == 0)
	{
		return true;
	}

	if (NULL == m_pParent)
	{
		return false;
	}
	else
	{
		return m_pParent->DeriveFrom(name);
	}
}

size_t FrameObjectDesc::GetPropertyCount() const
{
	return m_nPropertyCount;
}

void FrameObjectDesc::InnerGetPropertyList(IArgList* result) const
{
	for (size_t i = 0; i < m_nPropertyCount; ++i)
	{
		result->AddString(m_pPropertyDescs[i].GetName());
	}
}

void FrameObjectDesc::GetPropertyList(IArgList* result) const
{
	result->Clear();
	this->InnerGetPropertyList(result);
}

FramePropertyDesc* FrameObjectDesc::GetPropertyDesc(const char* name) const
{
	size_t index;

	if (!m_PropertyIndices.GetData(name, &index))
	{
		return NULL;
	}

	return &m_pPropertyDescs[index];
}

IPropertyDesc* FrameObjectDesc::FindPropertyDesc(const char* name)
{
	FramePropertyDesc* pPropertyDesc = this->GetPropertyDesc(name);

	if (NULL == pPropertyDesc)
	{
		FrameObjectDesc* p = GetParentDesc();
		
		while (p)
		{
			pPropertyDesc = p->GetPropertyDesc(name);

			if (pPropertyDesc != NULL)
			{
				break;
			}
			
			p = p->GetParentDesc();
		}
	}

	return pPropertyDesc;
}

void FrameObjectDesc::GetPropertyAll(IArgList* result)
{
	result->Clear();
	this->InnerGetPropertyList(result);

	FrameObjectDesc* p = (FrameObjectDesc*)GetParent();

	while (p)
	{
		p->InnerGetPropertyList(result);
		p = (FrameObjectDesc*)p->GetParent();
	}
}

size_t FrameObjectDesc::GetMethodCount() const
{
	return m_nMethodCount;
}

void FrameObjectDesc::InnerGetMethodList(IArgList* result) const
{
	for (size_t i = 0; i < m_nMethodCount; ++i)
	{
		result->AddString(m_pMethodDescs[i].GetName());
	}
}

void FrameObjectDesc::GetMethodList(IArgList* result) const
{
	result->Clear();
	this->InnerGetMethodList(result);
}

FrameMethodDesc* FrameObjectDesc::GetMethodDesc(const char* name) const
{
	size_t index;

	if (!m_MethodIndices.GetData(name, &index))
	{
		return NULL;
	}

	return &m_pMethodDescs[index];
}

IMethodDesc* FrameObjectDesc::FindMethodDesc(const char* name)
{
	FrameMethodDesc* pMethodDesc = this->GetMethodDesc(name);

	if (NULL == pMethodDesc)
	{
		FrameObjectDesc* p = this->GetParentDesc();

		while (p)
		{
			pMethodDesc = p->GetMethodDesc(name);

			if (pMethodDesc != NULL)
			{
				break;
			}

			p = p->GetParentDesc();
		}
	}

	return pMethodDesc;
}

void FrameObjectDesc::GetMethodAll(IArgList* result)
{
	result->Clear();
	this->InnerGetMethodList(result);
	
	FrameObjectDesc* p = (FrameObjectDesc*)GetParent();
	
	while (p) 
	{
		p->InnerGetMethodList(result);
		p = (FrameObjectDesc*)p->GetParent();
	}
}

void FrameObjectDesc::SetCreator(IObjectCreator* value)
{
	Assert(value != NULL);
	
	m_pCreator = value;
}

void FrameObjectDesc::SetParentName(const char* value)
{
	Assert(value != NULL);

	m_sParentName = value;
}

void FrameObjectDesc::SetObjectName(const char* value)
{
	Assert(value != NULL);

	m_sObjectName = value;
}

void FrameObjectDesc::SetParentDesc(FrameObjectDesc* pParent)
{
	Assert(pParent != NULL);

	m_pParent = pParent;
}

FrameObjectDesc* FrameObjectDesc::GetParentDesc() const
{
	return m_pParent;
}

size_t FrameObjectDesc::AddPropertyLink(object_property_t* pPropertyLink)
{
	if (NULL == pPropertyLink)
	{
		return 0;
	}

	// Property counter.
	size_t count = 0;
	object_property_t* p = pPropertyLink;

	while (p)
	{
		++count;
		p = p->pNext;
	}

	Assert(NULL == m_pPropertyDescs);

	m_pPropertyDescs = (FramePropertyDesc*)K_INNER_ALLOC(
		sizeof(FramePropertyDesc) * count);
	
	for (size_t i = 0; i < count; ++i)
	{
		new (m_pPropertyDescs + i) FramePropertyDesc;
	}
	
	m_nPropertyCount = count;

	// Reverse order.
	size_t index = count - 1;

	p = pPropertyLink;

	while (p)
	{
		FramePropertyDesc& data = m_pPropertyDescs[index];

		data.SetName(p->pName);
		data.SetType(p->nType);
		data.SetReadFunc(p->pReadFunc);
		data.SetWriteFunc(p->pWriteFunc);
		m_PropertyIndices.Add(data.GetName(), index);
		index--;
		p = p->pNext;
	}
		
	return count;
}

size_t FrameObjectDesc::AddMethodLink(object_method_t* pMethodLink)
{
	if (NULL == pMethodLink)
	{
		return 0;
	}
	
	// Method counter.
	size_t count = 0;
	object_method_t* p = pMethodLink;
	
	while (p)
	{
		++count;
		p = p->pNext;
	}
	
	Assert(NULL == m_pMethodDescs);

	m_pMethodDescs = (FrameMethodDesc*)K_INNER_ALLOC(
		sizeof(FrameMethodDesc) * count);
	
	for (size_t i = 0; i < count; ++i)
	{
		new (m_pMethodDescs + i) FrameMethodDesc;
	}

	m_nMethodCount = count;
	
	// Reverse order.
	size_t index = count - 1;
	
	p = pMethodLink;
	
	while (p)
	{
		FrameMethodDesc& data = m_pMethodDescs[index];
		
		data.SetName(p->pName);
		data.SetMethodFunc(p->pMethodFunc);
		data.SetResultArray(p->bResultArray);
		m_MethodIndices.Add(data.GetName(), index);
		index--;
		p = p->pNext;
	}
	
	return count;
}
