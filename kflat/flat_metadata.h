// flat_metadata.h
// Created by lulimin on 2020/12/30.

#ifndef __FLAT_METADATA_H
#define __FLAT_METADATA_H

#include "../inc/common.h"
#include "../inc/uniqid.h"

// Metadata types.
#define FLAT_METATYPE_BOOLEAN 1
#define FLAT_METATYPE_INT32 2
#define FLAT_METATYPE_INT64 3
#define FLAT_METATYPE_FLOAT 4
#define FLAT_METATYPE_DOUBLE 5
#define FLAT_METATYPE_STRING 6
#define FLAT_METATYPE_COLOR 7
#define FLAT_METATYPE_IMAGE 8
#define FLAT_METATYPE_CURSOR 9
#define FLAT_METATYPE_FONT 10
#define FLAT_METATYPE_FILE 11

// Metadata flags.
#define FLAT_METAFLAG_SAVE 0x1
#define FLAT_METAFLAG_PART_SAVE 0x2

// User interface metadata.

class IArgData;
struct flat_metadata_t;

// Create metadata.
flat_metadata_t* flat_metadata_create();
// Delete metadata.
void flat_metadata_delete(flat_metadata_t* pMetadata);

// Set relate object id.
void flat_metadata_set_object(flat_metadata_t* pMetadata, const uniqid_t& id);
// Get relate object id.
uniqid_t flat_metadata_get_object(flat_metadata_t* pMetadata);

// Add property to metadata.
bool flat_metadata_add_prop(flat_metadata_t* pMetadata, const char* name,
	int type, const IArgData* pDefValue);
// Remove property from metadata.
bool flat_metadata_remove_prop(flat_metadata_t* pMetadata, const char* name);
// Set property saving as part widget.
bool flat_metadata_part_prop(flat_metadata_t* pMetadata, const char* name);
// Get property count.
size_t flat_metadata_prop_count(flat_metadata_t* pMetadata);
// Get property name.
const char* flat_metadata_prop_name(flat_metadata_t* pMetadata, size_t index);
// Get property type.
int flat_metadata_prop_type(flat_metadata_t* pMetadata, size_t index);
// Get property flags.
int flat_metadata_prop_flags(flat_metadata_t* pMetadata, size_t index);
// Get property default value.
const IArgData* flat_metadata_prop_default(flat_metadata_t* pMetadata,
	size_t index);
// Find property index by name.
int flat_metadata_find_prop(flat_metadata_t* pMetadata, const char* name);

// Add event to metadata.
bool flat_metadata_add_event(flat_metadata_t* pMetadata, const char* name);
// Get event count.
size_t flat_metadata_event_count(flat_metadata_t* pMetadata);
// Get event name.
const char* flat_metadata_event_name(flat_metadata_t* pMetadata, size_t index);

// Add part to metadata.
bool flat_metadata_add_part(flat_metadata_t* pMetadata, const char* name,
	const char* widget_type);
// Get part widget count.
size_t flat_metadata_part_count(flat_metadata_t* pMetadata);
// Get part widget name.
const char* flat_metadata_part_name(flat_metadata_t* pMetadata, size_t index);
// Get part widget type.
const char* flat_metadata_part_type(flat_metadata_t* pMetadata, size_t index);

#endif // __FLAT_METADATA_H
