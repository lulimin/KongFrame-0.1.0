// flat_list_view.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_LIST_VIEW_H
#define __FLAT_LIST_VIEW_H

#include "flat_widget.h"
#include "../inc/pod_array.h"

// FlatListView

class FlatListView : public FlatWidget
{
private:
	// Item in list view.
	struct list_item_t
	{
		FrameString sText;
		FrameString sTag;
		FrameString sGlyph;
		flat_image_t* pGlyphImage;
		uniqid_t WidgetID;
	};

public:
	FlatListView();
	virtual ~FlatListView();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Get minimum widget size.
	virtual int GetMinimumWidth() { return 16; }
	virtual int GetMinimumHeight() { return 16; }

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);
	// Get coordinate in which widget.
	virtual FlatWidget* GetRegionOf(int x, int y);

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
	virtual int OnClickReset();
	virtual int OnMouseMove(int x, int y, unsigned int flags);
	virtual int OnMouseWheel(int x, int y, double delta, unsigned int flags);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftUp(int x, int y, unsigned int flags);
	virtual int OnLeftDual(int x, int y, unsigned int flags);
	virtual int OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags);
	virtual int OnLeftDragUp(int x, int y, int gcx, int gcy, 
		unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// List item width.
	void SetItemWidth(int value);
	int GetItemWidth() const;

	// List item height.
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// List glyph width.
	void SetGlyphWidth(int value);
	int GetGlyphWidth() const;

	// List glyph height.
	void SetGlyphHeight(int value);
	int GetGlyphHeight() const;

	// Index of selected item.
	void SetSelectedIndex(int value);
	int GetSelectedIndex() const;

	// Scroll bar size.
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// Dynamic scroll slide bar size.
	void SetDynamicSlide(bool value);
	bool GetDynamicSlide() const;

	// Always show vertical scroll bar.
	void SetVScrollAlways(bool value);
	bool GetVScrollAlways() const;

	// Show horizontal scroll bar.
	void SetHScrollExists(bool value);
	bool GetHScrollExists() const;

	// Sort text item.
	void SetSorted(bool value);
	bool GetSorted() const;

	// Text in right side.
	void SetRightText(bool value);
	bool GetRightText() const;

	// Enable drag.
	void SetDragEnable(bool value);
	bool GetDragEnable() const;

	// Select fore color.
	void SetSelectForeColor(const char* value);
	ArgString GetSelectForeColor() const;

	// Select background color.
	void SetSelectBackColor(const char* value);
	ArgString GetSelectBackColor() const;

	// Background color of captured item.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;

	// Preset scroll style.
	//void SetScrollStyle(const char* value);
	//const char* GetScrollStyle() const;

	// Get scroll widget id.
	uniqid_t GetVScrollBarID() const;
	uniqid_t GetHScrollBarID() const;
	uniqid_t GetCrossLabelID() const;

	// Add list item.
	int AddItem(const char* text, const char* glyph);
	// Add widget item.
	int AddWidgetItem(const char* text, const uniqid_t& id);
	// Remove list item.
	bool RemoveItem(const char* text);
	// Remove item by index.
	bool RemoveByIndex(int index);
	// Clear all items.
	bool ClearItems();
	// Search item.
	int FindItem(const char* text);
	// Set tag of item.
	bool SetTag(int index, const char* tag);
	// Get tag of item.
	const char* GetTag(int index) const;
	// Get text by index.
	const char* GetText(int index);
	// Get glyph by index.
	const char* GetGlyph(int index);
	// Get widget by index.
	uniqid_t GetWidget(int index);
	// Get list item number.
	int GetItemCount();

	// Begin update.
	bool BeginUpdate();
	// End update.
	bool EndUpdate();

private:
	// Get item index under mouse.
	int GetInItemIndex(int x, int y);
	// Add sorted item.
	int AddSortedItem(list_item_t* pItem, int head, int tail);
	// Get item number in a row.
	int GetItemColumns();
	// Guarantee selected visible.
	void ShowSelected();
	// Perform part widget layout.
	void PerformLayout();
	// Update scroll status.
	void UpdateScroll();

private:
	int m_nItemHeight;
	int m_nItemWidth;
	int m_nGlyphHeight;
	int m_nGlyphWidth;
	int m_nCaptureIndex;
	int m_nSelectedIndex;
	int m_nVerticalBegin;
	int m_nHorizontalBegin;
	bool m_bSorted;
	bool m_bRightText;
	bool m_bDragEnable;
	bool m_bDraging;
	bool m_bUpdating;
	unsigned int m_nSelectForeColor;
	unsigned int m_nSelectBackColor;
	unsigned int m_nCaptureColor;
	//FrameString m_sScrollStyle;
	TPodArray<list_item_t*, 1, FrameAlloc> m_ListItems;
};

#endif // __FLAT_LIST_VIEW_H
