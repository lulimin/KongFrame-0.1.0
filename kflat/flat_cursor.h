// flat_cursor.h
// Created by lulimin on 2020/12/15.

#ifndef __FLAT_CURSOR_H
#define __FLAT_CURSOR_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"

// Show cursor.
bool flat_show_cursor();
// Hide cursor.
bool flat_hide_cursor();

// FlatCursor

struct flat_cursor_imp_t;

class FlatCursor : public IObject
{
public:
	FlatCursor();
	virtual ~FlatCursor();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Cursor name.
	void SetName(const char* value);
	const char* GetName() const;
	// Get hash value of name.
	unsigned int GetNameHash() const { return m_nNameHash; }

	// Cursor resource.
	void SetResource(const char* value);
	const char* GetResource() const;

	// Load cursor.
	bool Load();
	// Apply cursor.
	bool Apply();

private:
	FrameString m_sName;
	unsigned int m_nNameHash;
	FrameString m_sResource;
	flat_cursor_imp_t* m_pImp;
};

#endif // __FLAT_CURSOR_H
