// flat_parts.cpp
// Created by lulimin on 2020/9/16.

#include "flat_parts.h"
#include "flat_widget.h"
#include "../inc/frame_mem.h"
#include "../inc/pod_array.h"

// Part container.
struct flat_parts_t
{
	FlatWidget* pParent;
	TPodArray<FlatWidget*, 1, FrameAlloc> Widgets;
};

// Create part container.
flat_parts_t* flat_parts_create(FlatWidget* pParent)
{
	Assert(pParent != NULL);

	flat_parts_t* pParts = K_NEW(flat_parts_t);

	pParts->pParent = pParent;
	return pParts;
}

// Delete part container.
void flat_parts_delete(flat_parts_t* pParts)
{
	Assert(pParts != NULL);

	K_DELETE(pParts);
}

// Get part container size.
size_t flat_parts_get_size(flat_parts_t* pParts)
{
	Assert(pParts != NULL);

	return pParts->Widgets.Size();
}

// Get part container data.
FlatWidget** flat_parts_get_data(flat_parts_t* pParts)
{
	Assert(pParts != NULL);

	return pParts->Widgets.Data();
}

// Add child to container.
bool flat_parts_add(flat_parts_t* pParts, FlatWidget* pWidget)
{
	Assert(pParts != NULL);
	Assert(pWidget != NULL);

	if (pWidget->GetParent() != NULL)
	{
		return false;
	}

	pWidget->SetParent(pParts->pParent);
	pParts->Widgets.PushBack(pWidget);
	return true;
}

// Remove child from container.
bool flat_parts_remove(flat_parts_t* pParts, FlatWidget* pWidget)
{
	Assert(pParts != NULL);
	Assert(pWidget != NULL);

	if (pWidget->GetParent() == NULL)
	{
		return false;
	}
	
	for (size_t i = 0; i < pParts->Widgets.Size(); ++i)
	{
		if (pParts->Widgets[i] == pWidget)
		{
			pParts->Widgets.Remove(i);
			pWidget->SetParent(NULL);
			return true;
		}
	}
	
	return false;
}

// Erase all parts.
void flat_parts_erase_all(flat_parts_t* pParts)
{
	Assert(pParts != NULL);

	for (size_t i = 0; i < pParts->Widgets.Size(); ++i)
	{
		FlatWidget* pWidget = pParts->Widgets[i];

		pWidget->SetParent(NULL);
		pWidget->Release();
	}

	pParts->Widgets.Clear();
}
