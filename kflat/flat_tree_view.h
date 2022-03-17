// flat_tree_view.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_TREE_VIEW_H
#define __FLAT_TREE_VIEW_H

#include "flat_widget.h"
#include "../inc/pod_array.h"

// FlatTreeView

class FlatTreeView : public FlatWidget
{
private:
	// Tree node.
	struct tree_node_t
	{
		size_t nIdentity;
		FrameString sText;
		FrameString sTag;
		int nLevel;
		int nIndex;
		bool bExpanded;
		bool bFirstExpand;
		tree_node_t* pParent;
		tree_node_t** pChildren;
		size_t nChildSize;
		size_t nChildCount;
	};

public:
	FlatTreeView();
	virtual ~FlatTreeView();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

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

	// Tree node item height.
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// Indent width between level.
	void SetIndentWidth(int value);
	int GetIndentWidth() const;

	// Scroll bar size.
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// Dynamic scroll slide bar size.
	void SetDynamicSlide(bool value);
	bool GetDynamicSlide() const;

	// Preset scroll style.
	//void SetScrollStyle(const char* value);
	//const char* GetScrollStyle() const;

	// Always show vertical scroll bar.
	void SetVScrollAlways(bool value);
	bool GetVScrollAlways() const;

	// Always show horizontal scroll bar.
	void SetHScrollAlways(bool value);
	bool GetHScrollAlways() const;

	// Show plus minus symbol.
	void SetShowPlusMinus(bool value);
	bool GetShowPlusMinus() const;

	// Sort node.
	void SetSorted(bool value);
	bool GetSorted() const;

	// Hide root node.
	void SetHideRoot(bool value);
	bool GetHideRoot() const;

	// Select fore color.
	void SetSelectForeColor(const char* value);
	ArgString GetSelectForeColor() const;

	// Select background color.
	void SetSelectBackColor(const char* value);
	ArgString GetSelectBackColor() const;

	// Background color of captured item.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;

	// Plus image name.
	void SetPlusImage(const char* value);
	const char* GetPlusImage() const;

	// Minus image name.
	void SetMinusImage(const char* value);
	const char* GetMinusImage() const;

	// Get scroll widget id.
	uniqid_t GetVScrollBarID() const;
	uniqid_t GetHScrollBarID() const;
	uniqid_t GetCrossLabelID() const;

	// Create root node.
	bool CreateRoot(const char* text, bool expand);
	// Add child node and set current node to it.
	int AddText(const char* text, bool expand);
	// Remove child node by text.
	bool RemoveText(const char* text);
	// Remove child node by index.
	bool RemoveIndex(int index);
	// Remove node by level and index.
	bool RemoveIdentity(int identity);
	// Clear all nodes.
	bool Clear();

	// Select root node.
	bool SelectRoot();
	// Select first child node.
	bool SelectFirstChild();
	// Select next child node.
	bool SelectNextSibling();
	// Select child node by text.
	bool SelectText(const char* text);
	// Select child node by index.
	bool SelectIndex(int index);
	// Select node by identity.
	bool SelectIdentity(int identity);
	// Select parent node.
	bool SelectParent();
	// Clear select.
	bool SelectNull();

	// Get text of current node.
	const char* GetText();
	// Get tag of current node.
	const char* GetTag();
	// Set tag of current node.
	bool SetTag(const char* tag);
	// Find node identity by tag.
	int FindTag(const char* tag);
	// Get level of current node.
	int GetLevel();
	// Get index of current node.
	int GetIndex();
	// Get identity of current node.
	int GetIdentity();
	// Get expanded of current node.
	bool GetExpanded();
	// Set expanded of current node.
	bool SetExpanded(bool value);
	// Get child number of current node.
	int GetChildCount();
	// Get child identity list of current node.
	void GetChildList(const IArgList& args, IArgList* res);

	// Begin update.
	bool BeginUpdate();
	// End update.
	bool EndUpdate();

private:
	// Search node identity by tag.
	int SearchTag(tree_node_t* pNode, const char* tag);
	// Get mouse in which node.
	tree_node_t* GetInNode(int x, int y);
	// Move current node to previus.
	bool MoveToPrev();
	// Move current node to next.
	bool MoveToNext();
	// Get last visible node.
	tree_node_t* GetLastVisibleNode(tree_node_t* pNode);
	// Add sorted node.
	int AddSortedNode(tree_node_t* pParent, tree_node_t* pNode, int head,
		int tail);
	// Add child node.
	void AddNode(tree_node_t* pParent, int index, tree_node_t* pNode);
	// Remove child node.
	void RemoveNode(tree_node_t* pNode);
	// Create new node.
	tree_node_t* NewNode();
	// Delete tree node.
	void DeleteNode(tree_node_t* pNode);
	// Set current select node.
	void SetCurrentNode(tree_node_t* pNode);
	// Get node maximum item width.
	int GetNodeMaxItemWidth(tree_node_t* pNode);
	// Update maximum width of all list item.
	void UpdateMaxItemWidth();
	// Get node vertical begin.
	int GetNodeVerticalBegin(tree_node_t* pNode, tree_node_t* pCurrent);
	// Guarantee selected visible.
	void ShowSelected();
	// Perform part widget layout.
	void PerformLayout();
	// Update scroll status.
	void UpdateScroll();
	// Get row number of node.
	int GetNodeRowCount(tree_node_t* pNode);
	// Update visible row count.
	void UpdateRowCount();
	// Get display nodes.
	void GetDiplayNodes(tree_node_t* pNode, int* pRowCount);
	// Update display nodes.
	void UpdateDisplayNodes();

private:
	tree_node_t* m_pRoot;
	tree_node_t* m_pCurrent;
	int m_nCaptureIdentity;
	int m_nVerticalBegin;
	int m_nHorizontalBegin;
	int m_nMaxItemWidth;
	int m_nVisibleRowCount;
	int m_nItemHeight;
	int m_nIndentWidth;
	bool m_bShowPlusMinus;
	bool m_bSorted;
	bool m_bHideRoot;
	bool m_bUpdating;
	unsigned int m_nSelectForeColor;
	unsigned int m_nSelectBackColor;
	unsigned int m_nCaptureColor;
	flat_image_t* m_pPlusImage;
	flat_image_t* m_pMinusImage;
	FrameString m_sPlusImage;
	FrameString m_sMinusImage;
	//FrameString m_sScrollStyle;
	TPodArray<tree_node_t*, 1, FrameAlloc> m_DisplayNodes;
	TPodArray<tree_node_t*, 1, FrameAlloc> m_NodeBuffer;
	TPodArray<size_t, 1, FrameAlloc> m_NodeFree;
};

#endif // __FLAT_TREE_VIEW_H
