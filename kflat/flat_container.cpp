// flat_container.cpp
// Created by lulimin on 2020/9/16.

#include "flat_container.h"
#include "flat_widget.h"
#include "../inc/frame_mem.h"
#include "../inc/pod_array.h"

// Children container.
struct flat_container_t
{
	FlatWidget* pParent;
	TPodArray<FlatWidget*, 1, FrameAlloc> Widgets;
};

// Create children container.
flat_container_t* flat_container_create(FlatWidget* pParent)
{
	Assert(pParent != NULL);

	flat_container_t* pContainer = K_NEW(flat_container_t);

	pContainer->pParent = pParent;
	return pContainer;
}

// Delete children container.
void flat_container_delete(flat_container_t* pContainer)
{
	Assert(pContainer != NULL);

	K_DELETE(pContainer);
}

// Get children container size.
size_t flat_container_get_size(flat_container_t* pContainer)
{
	Assert(pContainer != NULL);

	return pContainer->Widgets.Size();
}

// Get children container data.
FlatWidget** flat_container_get_data(flat_container_t* pContainer)
{
	Assert(pContainer != NULL);

	return pContainer->Widgets.Data();
}

// Add child to container.
bool flat_container_add(flat_container_t* pContainer, FlatWidget* pWidget)
{
	Assert(pContainer != NULL);
	Assert(pWidget != NULL);

	if (pWidget->GetParent() != NULL)
	{
		return false;
	}

	pWidget->SetParent(pContainer->pParent);
	pContainer->Widgets.PushBack(pWidget);
	return true;
}

// Remove child from container.
bool flat_container_remove(flat_container_t* pContainer, FlatWidget* pWidget)
{
	Assert(pContainer != NULL);
	Assert(pWidget != NULL);

	if (pWidget->GetParent() == NULL)
	{
		return false;
	}
	
	for (size_t i = 0; i < pContainer->Widgets.Size(); ++i)
	{
		if (pContainer->Widgets[i] == pWidget)
		{
			pContainer->Widgets.Remove(i);
			pWidget->SetParent(NULL);
			return true;
		}
	}
	
	return false;
}

// Erase all children.
void flat_container_erase_all(flat_container_t* pContainer)
{
	Assert(pContainer != NULL);

	for (size_t i = 0; i < pContainer->Widgets.Size(); ++i)
	{
		FlatWidget* pWidget = pContainer->Widgets[i];

		pWidget->SetParent(NULL);
		pWidget->Release();
	}

	pContainer->Widgets.Clear();
}

// Test container empty.
bool flat_container_empty(flat_container_t* pContainer)
{
	Assert(pContainer != NULL);

	return pContainer->Widgets.Empty();
}

// Find child in container.
bool flat_container_find(flat_container_t* pContainer, const char* name)
{
	Assert(pContainer != NULL);
	Assert(name != NULL);

	unsigned int hash = util_string_hash(name);
	
	for (size_t i = 0; i < pContainer->Widgets.Size(); ++i)
	{
		FlatWidget* pWidget = pContainer->Widgets[i];

		if ((pWidget->GetNameHash() == hash) &&
			(strcmp(pWidget->GetName(), name) == 0))
		{
			return true;
		}
	}

	return false;
}

// Bring child to front.
bool flat_container_to_front(flat_container_t* pContainer, FlatWidget* pWidget)
{
	Assert(pContainer != NULL);
	Assert(pWidget != NULL);

	for (size_t i = 0; i < pContainer->Widgets.Size(); ++i)
	{
		if (pContainer->Widgets[i] == pWidget)
		{
			pContainer->Widgets.Remove(i);
			pContainer->Widgets.PushBack(pWidget);
			return true;
		}
	}

	return false;
}

// Send child to back.
bool flat_container_to_back(flat_container_t* pContainer, FlatWidget* pWidget)
{
	Assert(pContainer != NULL);
	Assert(pWidget != NULL);

	for (size_t i = 0; i < pContainer->Widgets.Size(); ++i)
	{
		if (pContainer->Widgets[i] == pWidget)
		{
			pContainer->Widgets.Remove(i);
			pContainer->Widgets.Insert(0, pWidget);
			return true;
		}
	}
	
	return false;
}
