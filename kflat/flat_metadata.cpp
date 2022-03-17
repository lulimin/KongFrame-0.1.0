// flat_metadata.cpp
// Created by lulimin on 2020/12/30.

#include "flat_metadata.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

// Metadata property.
struct meta_prop_t
{
	FrameString sName;
	int nType;
	int nFlags;
	FrameArgData* pDefValue;
};

// Metadata event.
struct meta_event_t
{
	FrameString sName;
};

// Metadata part.
struct meta_part_t
{
	FrameString sName;
	FrameString sWidgetType;
};

// User interface metadata.
struct flat_metadata_t
{
	TPodArray<meta_prop_t*, 1, FrameAlloc> Props;
	TPodArray<meta_event_t*, 1, FrameAlloc> Events;
	TPodArray<meta_part_t*, 1, FrameAlloc> Parts;
	uniqid_t ObjectID;
};

// Create metadata.
flat_metadata_t* flat_metadata_create()
{
	flat_metadata_t* pMetadata = K_NEW(flat_metadata_t);

	return pMetadata;
}

// Delete metadata.
void flat_metadata_delete(flat_metadata_t* pMetadata)
{
	Assert(pMetadata != NULL);

	for (size_t i = 0; i < pMetadata->Props.Size(); ++i)
	{
		meta_prop_t* pProp = pMetadata->Props[i];

		if (pProp->pDefValue)
		{
			K_DELETE(pProp->pDefValue);
		}
		
		K_DELETE(pProp);
	}

	for (size_t k = 0; k < pMetadata->Events.Size(); ++k)
	{
		K_DELETE(pMetadata->Events[k]);
	}

	for (size_t j = 0; j < pMetadata->Parts.Size(); ++j)
	{
		K_DELETE(pMetadata->Parts[j]);
	}

	K_DELETE(pMetadata);
}

// Set relate object id.
void flat_metadata_set_object(flat_metadata_t* pMetadata, const uniqid_t& id)
{
	Assert(pMetadata != NULL);

	pMetadata->ObjectID = id;
}

// Get relate object id.
uniqid_t flat_metadata_get_object(flat_metadata_t* pMetadata)
{
	Assert(pMetadata != NULL);

	return pMetadata->ObjectID;
}

// Add property to metadata.
bool flat_metadata_add_prop(flat_metadata_t* pMetadata, const char* name,
	int type, const IArgData* pDefValue)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);

	meta_prop_t* pProp = K_NEW(meta_prop_t);

	pProp->sName = name;
	pProp->nType = type;
	pProp->nFlags = FLAT_METAFLAG_SAVE;
	//pProp->nFlags = flags;

	if (pDefValue)
	{
		pProp->pDefValue = (FrameArgData*)K_ALLOC(sizeof(FrameArgData));
		new (pProp->pDefValue) FrameArgData(*pDefValue);
	}
	else
	{
		pProp->pDefValue = NULL;
	}
	
	pMetadata->Props.PushBack(pProp);
	return true;
}

// Remove property from metadata.
bool flat_metadata_remove_prop(flat_metadata_t* pMetadata, const char* name)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);

	for (size_t i = 0; i < pMetadata->Props.Size(); ++i)
	{
		meta_prop_t* pProp = pMetadata->Props[i];
		
		if (pProp->sName == name)
		{
			if (pProp->pDefValue)
			{
				K_DELETE(pProp->pDefValue);
			}
			
			K_DELETE(pProp);
			pMetadata->Props.Remove(i);
			return true;
		}
	}
	
	return false;
}

// Set property saving as part widget.
bool flat_metadata_part_prop(flat_metadata_t* pMetadata, const char* name)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);

	for (size_t i = 0; i < pMetadata->Props.Size(); ++i)
	{
		meta_prop_t* pProp = pMetadata->Props[i];

		if (pProp->sName == name)
		{
			pProp->nFlags |= FLAT_METAFLAG_PART_SAVE;
			return true;
		}
	}

	return false;
}

// Get property count.
size_t flat_metadata_prop_count(flat_metadata_t* pMetadata)
{
	Assert(pMetadata != NULL);

	return pMetadata->Props.Size();
}

// Get property name.
const char* flat_metadata_prop_name(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Props.Size());

	return pMetadata->Props[index]->sName.CString();
}

// Get property type.
int flat_metadata_prop_type(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Props.Size());

	return pMetadata->Props[index]->nType;
}

// Get property flags.
int flat_metadata_prop_flags(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Props.Size());

	return pMetadata->Props[index]->nFlags;
}

// Get property default value.
const IArgData* flat_metadata_prop_default(flat_metadata_t* pMetadata,
	size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Props.Size());

	return pMetadata->Props[index]->pDefValue;
}

// Find property index by name.
int flat_metadata_find_prop(flat_metadata_t* pMetadata, const char* name)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);

	for (size_t i = 0; i < pMetadata->Props.Size(); ++i)
	{
		if (pMetadata->Props[i]->sName == name)
		{
			return (int)i;
		}
	}

	return -1;
}

// Add event to metadata.
bool flat_metadata_add_event(flat_metadata_t* pMetadata, const char* name)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);

	meta_event_t* pEvent = K_NEW(meta_event_t);

	pEvent->sName = name;
	pMetadata->Events.PushBack(pEvent);
	return true;
}

// Get event count.
size_t flat_metadata_event_count(flat_metadata_t* pMetadata)
{
	Assert(pMetadata != NULL);

	return pMetadata->Events.Size();
}

// Get event name.
const char* flat_metadata_event_name(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Events.Size());

	return pMetadata->Events[index]->sName.CString();
}

// Add part to metadata.
bool flat_metadata_add_part(flat_metadata_t* pMetadata, const char* name,
	const char* widget_type)
{
	Assert(pMetadata != NULL);
	Assert(name != NULL);
	Assert(widget_type != NULL);

	meta_part_t* pPart = K_NEW(meta_part_t);

	pPart->sName = name;
	pPart->sWidgetType = widget_type;
	pMetadata->Parts.PushBack(pPart);
	return true;
}

// Get part widget count.
size_t flat_metadata_part_count(flat_metadata_t* pMetadata)
{
	Assert(pMetadata != NULL);

	return pMetadata->Parts.Size();
}

// Get part widget name.
const char* flat_metadata_part_name(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Parts.Size());

	return pMetadata->Parts[index]->sName.CString();
}

// Get part widget type.
const char* flat_metadata_part_type(flat_metadata_t* pMetadata, size_t index)
{
	Assert(pMetadata != NULL);
	Assert(index < pMetadata->Parts.Size());

	return pMetadata->Parts[index]->sWidgetType.CString();
}
