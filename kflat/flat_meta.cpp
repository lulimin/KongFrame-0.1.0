// flat_meta.cpp
// Created by lulimin on 2021/1/14.

#include "flat_meta.h"
#include "flat_metadata.h"

/// \file flat_meta.cpp
/// \brief Widget metadata.

/// object: FlatMeta
/// \brief Widget metadata object.
DEF_OBJECT(FlatMeta, IObject);

/// function: int GetPropertyCount()
/// \brief Get property number.
DEF_FUNC_0(int, FlatMeta, GetPropertyCount);
/// function: string GetPropertyName(int index)
/// \brief Get property name by index.
/// \param index Property index.
DEF_FUNC_1(const char*, FlatMeta, GetPropertyName, int);
/// function: string GetPropertyType(int index)
/// \brief Get property type by index.
/// \param index Property index.
DEF_FUNC_1(const char*, FlatMeta, GetPropertyType, int);
/// function: string FindPropertyType(string name)
/// \brief Find property type by name.
/// \param name Property name.
DEF_FUNC_1(const char*, FlatMeta, FindPropertyType, const char*);
/// function: bool GetPropertyPart(int index)
/// \brief Get property saving as part widget.
/// \param index Property index.
DEF_FUNC_1(bool, FlatMeta, GetPropertyPart, int);
/// function: int GetEventCount()
/// \brief Get event number.
DEF_FUNC_0(int, FlatMeta, GetEventCount);
/// function: string GetEventName(int index)
/// \brief Get event name by index.
/// \param index Event index.
DEF_FUNC_1(const char*, FlatMeta, GetEventName, int);
/// function: int GetPartCount()
/// \brief Get part widget number.
DEF_FUNC_0(int, FlatMeta, GetPartCount);
/// function: string GetPartName(int index)
/// \brief Get part widget name by index.
/// \param index Part widget index.
DEF_FUNC_1(const char*, FlatMeta, GetPartName, int);
/// function: string GetPartType(int index)
/// \brief Get part widget type by index.
/// \param index Part widget index.
DEF_FUNC_1(const char*, FlatMeta, GetPartType, int);

// FlatMeta

FlatMeta::FlatMeta()
{
	m_pMetadata = NULL;
}

FlatMeta::~FlatMeta()
{
}

bool FlatMeta::Startup(const IArgList& args)
{
	return true;
}

bool FlatMeta::Shutdown()
{
	return true;
}

void FlatMeta::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	m_pMetadata = pMD;
}

int FlatMeta::GetPropertyCount()
{
	return (int)flat_metadata_prop_count(m_pMetadata);
}

const char* FlatMeta::GetPropertyName(int index)
{
	if ((size_t)index >= flat_metadata_prop_count(m_pMetadata))
	{
		return "";
	}

	return flat_metadata_prop_name(m_pMetadata, index);
}

const char* FlatMeta::GetPropertyType(int index)
{
	if ((size_t)index >= flat_metadata_prop_count(m_pMetadata))
	{
		return "";
	}

	int type = flat_metadata_prop_type(m_pMetadata, index);

	switch (type)
	{
	case FLAT_METATYPE_BOOLEAN:
		return "boolean";
	case FLAT_METATYPE_INT32:
		return "int32";
	case FLAT_METATYPE_INT64:
		return "int64";
	case FLAT_METATYPE_FLOAT:
		return "float";
	case FLAT_METATYPE_DOUBLE:
		return "double";
	case FLAT_METATYPE_STRING:
		return "string";
	case FLAT_METATYPE_COLOR:
		return "color";
	case FLAT_METATYPE_IMAGE:
		return "image";
	case FLAT_METATYPE_CURSOR:
		return "cursor";
	case FLAT_METATYPE_FONT:
		return "font";
	case FLAT_METATYPE_FILE:
		return "file";
	default:
		Assert(0);
		break;
	}

	return "";
}

const char* FlatMeta::FindPropertyType(const char* name)
{
	Assert(name != NULL);

	int index = flat_metadata_find_prop(m_pMetadata, name);

	if (index < 0)
	{
		return "";
	}

	return this->GetPropertyType(index);
}

bool FlatMeta::GetPropertyPart(int index)
{
	if ((size_t)index >= flat_metadata_prop_count(m_pMetadata))
	{
		return "";
	}

	int flags = flat_metadata_prop_flags(m_pMetadata, index);

	return (flags & FLAT_METAFLAG_PART_SAVE) != 0;
}

int FlatMeta::GetEventCount()
{
	return (int)flat_metadata_event_count(m_pMetadata);
}

const char* FlatMeta::GetEventName(int index)
{
	if ((size_t)index >= flat_metadata_event_count(m_pMetadata))
	{
		return "";
	}

	return flat_metadata_event_name(m_pMetadata, index);
}

int FlatMeta::GetPartCount()
{
	return (int)flat_metadata_part_count(m_pMetadata);
}

const char* FlatMeta::GetPartName(int index)
{
	if ((size_t)index >= flat_metadata_part_count(m_pMetadata))
	{
		return "";
	}

	return flat_metadata_part_name(m_pMetadata, index);
}

const char* FlatMeta::GetPartType(int index)
{
	if ((size_t)index >= flat_metadata_part_count(m_pMetadata))
	{
		return "";
	}

	return flat_metadata_part_type(m_pMetadata, index);
}
