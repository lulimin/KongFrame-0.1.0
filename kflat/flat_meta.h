// flat_meta.h
// Created by lulimin on 2021/1/14.

#ifndef __FLAT_META_H
#define __FLAT_META_H

#include "../inc/frame_def.h"

// FlatMeta

struct flat_metadata_t;

class FlatMeta : public IObject
{
public:
	FlatMeta();
	virtual ~FlatMeta();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Set metadata.
	void SetMetadata(flat_metadata_t* pMD);

	// Get property number.
	int GetPropertyCount();
	// Get property name by index.
	const char* GetPropertyName(int index);
	// Get property type by index.
	const char* GetPropertyType(int index);
	// Search property type by name.
	const char* FindPropertyType(const char* name);
	// Get property saving as part widget.
	bool GetPropertyPart(int index);
	// Get event number.
	int GetEventCount();
	// Get event name by index.
	const char* GetEventName(int index);
	// Get part number.
	int GetPartCount();
	// Get part widget name by index.
	const char* GetPartName(int index);
	// Get part widget type by index.
	const char* GetPartType(int index);

private:
	flat_metadata_t* m_pMetadata;
};

#endif // __FLAT_META_H
