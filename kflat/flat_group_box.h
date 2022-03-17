// flat_group_box.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_GROUP_BOX_H
#define __FLAT_GROUP_BOX_H

#include "flat_widget.h"

// FlatGroupBox

class FlatGroupBox : public FlatWidget
{
public:
	FlatGroupBox();
	virtual ~FlatGroupBox();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Clip children view.
	void SetClipView(bool value);
	bool GetClipView() const;
	
	// Add child widget.
	bool AddChild(const uniqid_t& id);
	// Remove child widget.
	bool RemoveChild(const uniqid_t& id);
	// Clear child widgets.
	bool ClearChildren();
	// Get child widget list.
	void GetChildList(const IArgList& args, IArgList* res);

private:
	FrameString m_sText;
	bool m_bClipView;
};

#endif // __FLAT_GROUP_BOX_H
