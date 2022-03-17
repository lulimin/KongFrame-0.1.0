// flat_panel.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_PANEL_H
#define __FLAT_PANEL_H

#include "flat_widget.h"

// FlatPanel

class FlatGroupBox;

class FlatPanel : public FlatWidget
{
public:
	FlatPanel();
	virtual ~FlatPanel();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Is contianer.
	virtual bool IsContainer() { return true; }
	// Test has children.
	virtual bool HasChildren();

	// Get minimum widget size.
	virtual int GetMinimumWidth() { return 16; }
	virtual int GetMinimumHeight() { return 16; }
	
	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Scroll related.
	virtual void DrawScrollRegion(flat_canvas_t* pCanvas);
	virtual int GetVerticalWhole();
	virtual int GetVerticalReveal();
	virtual int GetVerticalBegin();
	virtual void SetVerticalBegin(int value);
	virtual int GetHorizontalWhole();
	virtual int GetHorizontalReveal();
	virtual int GetHorizontalBegin();
	virtual void SetHorizontalBegin(int value);

	// Callbacks.
	virtual int OnMouseWheel(int x, int y, double delta, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Scroll bar size.
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// Dynamic scroll slide bar size.
	void SetDynamicSlide(bool value);
	bool GetDynamicSlide() const;

	// Preset scroll style.
	//void SetScrollStyle(const char* value);
	//const char* GetScrollStyle() const;

	// Get scroll widget id.
	uniqid_t GetVScrollBarID() const;
	uniqid_t GetHScrollBarID() const;
	uniqid_t GetCrossLabelID() const;

	// Set content size.
	bool SetContentSize(int width, int height);
	// Get content size.
	int GetContentWidth();
	int GetContentHeight();

	// Add child widget.
	bool AddChild(const uniqid_t& id);
	// Remove child widget.
	bool RemoveChild(const uniqid_t& id);
	// Clear child widgets.
	bool ClearChildren();
	// Get child widget list.
	void GetChildList(const IArgList& args, IArgList* res);

private:
	// Perform part widget layout.
	void PerformLayout();
	// Update scroll status.
	void UpdateScroll();

private:
	int m_nVerticalBegin;
	int m_nHorizontalBegin;
	FlatGroupBox* m_pGroupBox;
	//FrameString m_sScrollStyle;
};

#endif // __FLAT_PANEL_H
