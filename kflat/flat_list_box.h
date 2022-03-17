// flat_list_box.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_LIST_BOX_H
#define __FLAT_LIST_BOX_H

#include "flat_widget.h"
#include "../inc/pod_array.h"

// FlatListBox

class FlatListBox : public FlatWidget
{
private:
	// Item in list box.
	struct list_item_t
	{
		FrameString sText;
		int nWidth;
	};

public:
	FlatListBox();
	virtual ~FlatListBox();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

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
	virtual int OnMouseMove(int x, int y, unsigned int flags);
	virtual int OnMouseWheel(int x, int y, double delta, unsigned int flags);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftDual(int x, int y, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// List item height.
	void SetItemHeight(int value);
	int GetItemHeight() const;

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

	// Selected text.
	void SetSelectedText(const char* text);
	const char* GetSelectedText() const;

	// Get scroll widget id.
	uniqid_t GetVScrollBarID() const;
	uniqid_t GetHScrollBarID() const;
	uniqid_t GetCrossLabelID() const;

	// Add text item.
	int AddText(const char* text);
	// Remove text item.
	bool RemoveText(const char* text);
	// Remove text by index.
	bool RemoveByIndex(int index);
	// Clear all text.
	bool ClearTexts();
	// Search text.
	int FindText(const char* text);
	// Get text by index.
	const char* GetText(int index);
	// Get text item number.
	int GetTextCount();

	// Set top visible list item.
	bool SetTopIndex(int item_index);

	// Begin update.
	bool BeginUpdate();
	// End update.
	bool EndUpdate();

private:
	// Get item index under mouse.
	int GetInItemIndex(int x, int y);
	// Add sorted item.
	int AddSortedItem(list_item_t* pItem, int head, int tail);
	// Update maximum width of all list item.
	void UpdateMaxItemWidth();
	// Guarantee selected visible.
	void ShowSelected();
	// Perform part widget layout.
	void PerformLayout();
	// Update scroll status.
	void UpdateScroll();

private:
	int m_nItemHeight;
	int m_nCaptureIndex;
	int m_nSelectedIndex;
	int m_nVerticalBegin;
	int m_nHorizontalBegin;
	int m_nMaxItemWidth;
	bool m_bSorted;
	bool m_bUpdating;
	unsigned int m_nSelectForeColor;
	unsigned int m_nSelectBackColor;
	unsigned int m_nCaptureColor;
	//FrameString m_sScrollStyle;
	TPodArray<list_item_t*, 1, FrameAlloc> m_ListItems;
};

#endif // __FLAT_LIST_BOX_H
