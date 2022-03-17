// flat_tree_view.cpp
// Created by lulimin on 2020/9/11.

#include "flat_tree_view.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_tree_view.cpp
/// \brief User interface tree view.

/// object: FlatTreeView
/// \brief User interface tree view object.
DEF_OBJECT(FlatTreeView, FlatWidget);

/// readonly: object VScrollBar
/// \brief Vertical scroll bar.
DEF_READ_F(uniqid_t, FlatTreeView, VScrollBar, GetVScrollBarID);
/// readonly: object HScrollBar
/// \brief Horizontal scroll bar.
DEF_READ_F(uniqid_t, FlatTreeView, HScrollBar, GetHScrollBarID);
/// readonly: object CrossLabel
/// \brief Cross label.
DEF_READ_F(uniqid_t, FlatTreeView, CrossLabel, GetCrossLabelID);

/// property: int ItemHeight
/// \brief Height of tree node item.
DEF_PROP(int, FlatTreeView, ItemHeight);
/// property: int IndentWidth
/// \brief Indent width between level.
DEF_PROP(int, FlatTreeView, IndentWidth);
/// property: int ScrollSize
/// \brief Scroll bar size.
DEF_PROP(int, FlatTreeView, ScrollSize);
/// property: bool DynamicSlide
/// \brief Dynamic scroll slide bar size.
DEF_PROP(bool, FlatTreeView, DynamicSlide);
/// property: string ScrollStyle
/// \brief Preset scroll style.
//DEF_PROP(const char*, FlatTreeView, ScrollStyle);
/// property: bool VScrollAlways
/// \brief Always show vertical scroll bar.
DEF_PROP(bool, FlatTreeView, VScrollAlways);
/// property: bool HScrollAlways
/// \brief Always show horizontal scroll bar.
DEF_PROP(bool, FlatTreeView, HScrollAlways);
/// property: bool ShowPlusMinus
/// \brief Show plus and minus symbol.
DEF_PROP(bool, FlatTreeView, ShowPlusMinus);
/// property: bool Sorted
/// \brief Sort order of node.
DEF_PROP(bool, FlatTreeView, Sorted);
/// property: bool HideRoot
/// \brief Hide root node.
DEF_PROP(bool, FlatTreeView, HideRoot);
/// property: string SelectForeColor
/// \brief Fore color of selected item.
DEF_PROP(ArgString, FlatTreeView, SelectForeColor);
/// property: string SelectBackColor
/// \brief Background color of selected item.
DEF_PROP(ArgString, FlatTreeView, SelectBackColor);
/// property: string CaptureColor
/// \brief Background color of captured item.
DEF_PROP(ArgString, FlatTreeView, CaptureColor);
/// property: string PlusImage
/// \brief Image name of plus symbol.
DEF_PROP(const char*, FlatTreeView, PlusImage);
/// property: string MinusImage
/// \brief Image name of minus symbol.
DEF_PROP(const char*, FlatTreeView, MinusImage);

/// function: bool CreateRoot(string text, bool expand)
/// \brief Create root node.
/// \param text Node text.
/// \param expand Expanded status.
DEF_FUNC_2(bool, FlatTreeView, CreateRoot, const char*, bool);
/// function: int AddText(string text, bool expand)
/// \brief Add child node to current node.
/// \param text Node text.
/// \param expand Expanded status.
DEF_FUNC_2(int, FlatTreeView, AddText, const char*, bool);
/// function: bool RemoveText(string text)
/// \brief Remove child node from current node by text.
/// \param text Node text.
DEF_FUNC_1(bool, FlatTreeView, RemoveText, const char*);
/// function: bool RemoveIndex(int index)
/// \brief Remove child node from current node by index.
/// \param index Node index.
DEF_FUNC_1(bool, FlatTreeView, RemoveIndex, int);
/// function: bool RemoveIdentity(int identity)
/// \brief Remove node by identity.
/// \param identity Node identity.
DEF_FUNC_1(bool, FlatTreeView, RemoveIdentity, int);
/// function: bool Clear()
/// \brief Clear all nodes.
DEF_FUNC_0(bool, FlatTreeView, Clear);

/// function: bool SelectRoot()
/// \brief Select root node.
DEF_FUNC_0(bool, FlatTreeView, SelectRoot);
/// function: bool SelectFirstChild()
/// \brief Select first child node of current node.
DEF_FUNC_0(bool, FlatTreeView, SelectFirstChild);
/// function: bool SelectNextSibling()
/// \brief Select next sibling node of current node.
DEF_FUNC_0(bool, FlatTreeView, SelectNextSibling);
/// function: bool SelectText(string text)
/// \brief Select child node of current node by text.
/// \param text Node text.
DEF_FUNC_1(bool, FlatTreeView, SelectText, const char*);
/// function: bool SelectIndex(int index)
/// \brief Select child node of current node by index.
DEF_FUNC_1(bool, FlatTreeView, SelectIndex, int);
/// function: bool SelectIdentity(int identity)
/// \brief Select node by identity.
/// \param identity Node identity.
DEF_FUNC_1(bool, FlatTreeView, SelectIdentity, int);
/// function: bool SelectParent()
/// \brief Select parent node of current node.
DEF_FUNC_0(bool, FlatTreeView, SelectParent);
/// function: bool SelectNull()
/// \brief Clear select node.
DEF_FUNC_0(bool, FlatTreeView, SelectNull);

/// function: string GetText()
/// \brief Get text of current node.
DEF_FUNC_0(const char*, FlatTreeView, GetText);
/// function: string GetTag()
/// \brief Get tag of current node.
DEF_FUNC_0(const char*, FlatTreeView, GetTag);
/// function: bool SetTag(string tag)
/// \brief Set tag of current node.
/// \param tag Tag value.
DEF_FUNC_1(bool, FlatTreeView, SetTag, const char*);
/// function: bool FindTag(string tag)
/// \brief Find node identity by tag.
/// \param tag Tag value.
DEF_FUNC_1(bool, FlatTreeView, FindTag, const char*);
/// function: int GetLevel()
/// \brief Get level of current node.
DEF_FUNC_0(int, FlatTreeView, GetLevel);
/// function: int GetIndex()
/// \brief Get index of current node.
DEF_FUNC_0(int, FlatTreeView, GetIndex);
/// function: int GetIdentity()
/// \brief Get identity of current node.
DEF_FUNC_0(int, FlatTreeView, GetIdentity);
/// function: bool GetExpanded()
/// \brief Get expanded status of current node.
DEF_FUNC_0(bool, FlatTreeView, GetExpanded);
/// function: bool SetExpanded(bool value)
/// \brief Set expanded status of current node.
/// \param value Expanded status.
DEF_FUNC_1(bool, FlatTreeView, SetExpanded, bool);
/// function: int GetChildCount()
/// \brief Get child number of current node.
DEF_FUNC_0(int, FlatTreeView, GetChildCount);
/// function: [child identities] GetChildList()
/// \brief Get child identity list of current node.
DEF_FUNC_A(FlatTreeView, GetChildList);

/// function: bool BeginUdpate()
/// \brief Begin update.
DEF_FUNC_0(bool, FlatTreeView, BeginUpdate);
/// function: bool EndUpdate()
/// \brief End update.
DEF_FUNC_0(bool, FlatTreeView, EndUpdate);

#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000
#define INIT_CAPTURE_COLOR 0xFF800000

// FlatTreeView

FlatTreeView::FlatTreeView()
{
	m_pRoot = NULL;
	m_pCurrent = NULL;
	m_nCaptureIdentity = -1;
	m_nVerticalBegin = 0;
	m_nHorizontalBegin = 0;
	m_nMaxItemWidth = 0;
	m_nVisibleRowCount = 0;
	m_nItemHeight = 16;
	m_nIndentWidth = 16;
	m_bShowPlusMinus = false;
	m_bSorted = false;
	m_bHideRoot = false;
	m_bUpdating = false;
	m_nSelectForeColor = INIT_SELECT_FORE_COLOR;
	m_nSelectBackColor = INIT_SELECT_BACK_COLOR;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
	m_pPlusImage = NULL;
	m_pMinusImage = NULL;
}

FlatTreeView::~FlatTreeView()
{
	for (size_t i = 0; i < m_NodeBuffer.Size(); ++i)
	{
		tree_node_t* pNode = m_NodeBuffer[i];

		if (pNode->pChildren)
		{
			K_FREE(pNode->pChildren);
		}
		
		K_DELETE(pNode);
	}

	if (m_pPlusImage)
	{
		flat_image_delete(m_pPlusImage);
	}

	if (m_pMinusImage)
	{
		flat_image_delete(m_pMinusImage);
	}
}

bool FlatTreeView::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();
	this->CreateScroll();
	flat_scroll_initialize(this->GetScroll(), this->GetFlatUI(), true, true);
	return true;
}

bool FlatTreeView::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatTreeView::Draw(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	if (!this->GetVisible())
	{
		return;
	}

	int width = this->GetWidth();
	int height = this->GetHeight();
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int x2 = x1 + width;
	int y2 = y1 + height;
	bool exist_back_image = this->ExistBackImage();

	if (exist_back_image)
	{
		this->DrawBackImage(pCanvas, x1, y1, x2, y2);
	}
	else
	{
		canvas_draw_block(pCanvas, x1, y1, x2, y2, this->GetBackColorValue());
	}

	flat_scroll_draw(this->GetScroll(), pCanvas);

	if (!exist_back_image)
	{
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}
}

void FlatTreeView::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_empty_string(DT_STRING, "");
	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_false(DT_BOOLEAN, false);
	char buffer[32];
	FrameArgData def_select_fore_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_FORE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_select_back_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_BACK_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_capture_color(DT_STRING,
		flat_rgba_to_string(INIT_CAPTURE_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "ItemHeight", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "IndentWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "ScrollSize", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DynamicSlide", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "VScrollAlways", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "HScrollAlways", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "ShowPlusMinus", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "Sorted", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "HideRoot", FLAT_METATYPE_BOOLEAN,
		&def_false);
	//flat_metadata_add_prop(pMD, "ScrollStyle", FLAT_METATYPE_STRING,
	//	&def_empty_string);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_add_prop(pMD, "PlusImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "MinusImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_event(pMD, "click");
	flat_metadata_add_event(pMD, "expand");
	flat_metadata_add_part(pMD, "VScrollBar", "FlatVScrollBar");
	flat_metadata_add_part(pMD, "HScrollBar", "FlatHScrollBar");
	flat_metadata_add_part(pMD, "CrossLabel", "FlatLabel");
}

void FlatTreeView::DrawScrollRegion(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	int beg = m_nVerticalBegin / m_nItemHeight;
	int x1 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y1 = this->GetGlobalTop() + beg * m_nItemHeight - m_nVerticalBegin;
	int width = this->GetWidth();
	int height = this->GetHeight();
	int num = height / m_nItemHeight + 1;

	if (width < m_nMaxItemWidth)
	{
		width = m_nMaxItemWidth;
	}

	this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());

	for (int i = 0; i < num; ++i)
	{
		if ((size_t)i >= m_DisplayNodes.Size())
		{
			break;
		}

		tree_node_t* pNode = m_DisplayNodes[i];
		int x2 = x1 + width;
		int y2 = y1 + m_nItemHeight;
		unsigned int fore_color;

		if (m_pCurrent == pNode)
		{
			canvas_draw_block(pCanvas, x1, y1, x2, y2, m_nSelectBackColor);
			fore_color = m_nSelectForeColor;
		}
		else
		{
			if (this->GetCapture() &&
				(m_nCaptureIdentity == (int)pNode->nIdentity))
			{
				canvas_draw_block(pCanvas, x1, y1, x2, y2, m_nCaptureColor);
			}

			fore_color = this->GetForeColorValue();
		}

		int level = pNode->nLevel;

		if (m_bHideRoot)
		{
			--level;
		}

		int space_size = level * m_nIndentWidth;
		int symbol_size = 0;

		//if (m_bShowPlusMinus && (pNode->nChildCount > 0))
		if (m_bShowPlusMinus && 
			((!pNode->bFirstExpand) || (pNode->nChildCount > 0)))
		{
			if (pNode->bExpanded)
			{
				if (m_pMinusImage)
				{
					int image_w = flat_image_get_width(m_pMinusImage);
					int image_h = flat_image_get_height(m_pMinusImage);
					int image_x = x1 + space_size;
					int image_y = y1 + (m_nItemHeight - image_h) / 2;

					canvas_draw_image(pCanvas, image_x, image_y, 
						image_x + image_w, image_y + image_h, m_pMinusImage,
						FLAT_LAYOUT_FIT);
					symbol_size = image_w;
				}
				else
				{
					symbol_size = canvas_draw_text_left(pCanvas,
						x1 + space_size, y1, x2, y2, fore_color, "-", 0, 0);
				}
			}
			else
			{
				if (m_pPlusImage)
				{
					int image_w = flat_image_get_width(m_pPlusImage);
					int image_h = flat_image_get_height(m_pPlusImage);
					int image_x = x1 + space_size;
					int image_y = y1 + (m_nItemHeight - image_h) / 2;

					canvas_draw_image(pCanvas, image_x, image_y, 
						image_x + image_w, image_y + image_h, m_pPlusImage,
						FLAT_LAYOUT_FIT);
					symbol_size = image_w;
				}
				else
				{
					symbol_size = canvas_draw_text_left(pCanvas,
						x1 + space_size, y1, x2, y2, fore_color, "+", 0, 0);
				}
			}
		}

		canvas_draw_text_left(pCanvas, x1 + space_size + symbol_size, y1, x2,
			y2, fore_color, pNode->sText.CString(), 0, 0);
		y1 += m_nItemHeight;
	}
}

int FlatTreeView::GetVerticalWhole()
{
	return m_nVisibleRowCount * m_nItemHeight;
}

int FlatTreeView::GetVerticalReveal()
{
	return flat_scroll_reveal_height(this->GetScroll());
}

int FlatTreeView::GetVerticalBegin()
{
	return m_nVerticalBegin;
}

void FlatTreeView::SetVerticalBegin(int value)
{
	m_nVerticalBegin = value;
	this->UpdateDisplayNodes();
}

int FlatTreeView::GetHorizontalWhole()
{
	return m_nMaxItemWidth;
}

int FlatTreeView::GetHorizontalReveal()
{
	return flat_scroll_reveal_width(this->GetScroll());
}

int FlatTreeView::GetHorizontalBegin()
{
	return m_nHorizontalBegin;
}

void FlatTreeView::SetHorizontalBegin(int value)
{
	m_nHorizontalBegin = value;
}

FlatTreeView::tree_node_t* FlatTreeView::GetInNode(int x, int y)
{
	int beg = m_nVerticalBegin / m_nItemHeight;
	int x1 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y1 = this->GetGlobalTop() + beg * m_nItemHeight - m_nVerticalBegin;
	int width = this->GetWidth();
	int height = this->GetHeight();
	int num = height / m_nItemHeight + 1;

	if (width < m_nMaxItemWidth)
	{
		width = m_nMaxItemWidth;
	}

	for (int i = 0; i < num; ++i)
	{
		int index = i + beg;

		if (index >= m_nVisibleRowCount)
		{
			break;
		}

		int x2 = x1 + width;
		int y2 = y1 + m_nItemHeight;

		if (flat_in_rectangle(x, y, x1, y1, x2, y2))
		{
			if ((size_t)i >= m_DisplayNodes.Size())
			{
				break;
			}

			return m_DisplayNodes[i];
		}

		y1 += m_nItemHeight;
	}

	return NULL;
}

int FlatTreeView::OnMouseMove(int x, int y, unsigned int flags)
{
	if (m_nCaptureColor != 0)
	{
		tree_node_t* pNode = this->GetInNode(x, y);

		if (pNode)
		{
			m_nCaptureIdentity = (int)pNode->nIdentity;
		}
		else
		{
			m_nCaptureIdentity = -1;
		}
	}
	
	return 1;
}

int FlatTreeView::OnMouseWheel(int x, int y, double delta, unsigned int flags)
{
	if (flags & IInputService::FLAG_CONTROL)
	{
		int reveal_width = this->GetHorizontalReveal();
		int whole_width = this->GetHorizontalWhole();

		if (reveal_width >= whole_width)
		{
			return 0;
		}

		int new_value = m_nHorizontalBegin + (int)((-delta) * 4.0);

		if (new_value < 0)
		{
			new_value = 0;
		}
		else if ((new_value + reveal_width) >= whole_width)
		{
			new_value = whole_width - reveal_width;
		}

		this->SetHorizontalBegin(new_value);
		flat_scroll_adjust_hscroll(this->GetScroll());
	}
	else
	{
		int reveal_height = this->GetVerticalReveal();
		int whole_height = this->GetVerticalWhole();

		if (reveal_height >= whole_height)
		{
			return 0;
		}

		int new_value = m_nVerticalBegin + (int)((-delta) * 8.0);

		if (new_value < 0)
		{
			new_value = 0;
		}
		else if ((new_value + reveal_height) >= whole_height)
		{
			new_value = whole_height - reveal_height;
		}

		this->SetVerticalBegin(new_value);
		flat_scroll_adjust_vscroll(this->GetScroll());
	}

	return 1;
}

int FlatTreeView::OnLeftDown(int x, int y, unsigned int flags)
{
	tree_node_t* pNode = this->GetInNode(x, y);

	if (NULL == pNode)
	{
		return 0;
	}

	this->SetCurrentNode(pNode);

	//if (m_bShowPlusMinus && (pNode->nChildCount > 0))
	if (m_bShowPlusMinus && 
		((!pNode->bFirstExpand) || (pNode->nChildCount > 0)))
	{
		int level = pNode->nLevel;

		if (m_bHideRoot)
		{
			--level;
		}

		int space_size = level * m_nIndentWidth;
		int symbol_size = 0;

		if (pNode->bExpanded)
		{
			if (m_pMinusImage)
			{
				symbol_size = flat_image_get_width(m_pMinusImage);
			}
			else
			{
				symbol_size = this->GetFlatUI()->GetTextWidth(
					this->GetAvailableFont(), "-", 0, 0);
			}
		}
		else
		{
			if (m_pPlusImage)
			{
				symbol_size = flat_image_get_width(m_pPlusImage);
			}
			else
			{
				symbol_size = this->GetFlatUI()->GetTextWidth(
					this->GetAvailableFont(), "+", 0, 0);
			}
		}

		int x1 = this->GetGlobalLeft() - m_nHorizontalBegin;
		int x2 = x1 + space_size;
		int x3 = x2 + symbol_size;

		if ((x >= x2) && (x < x3))
		{
			this->SetExpanded(!pNode->bExpanded);
		}
	}

	return 1;
}

int FlatTreeView::OnLeftDual(int x, int y, unsigned int flags)
{
	tree_node_t* pNode = this->GetInNode(x, y);

	if (NULL == pNode)
	{
		return 0;
	}

	this->SetCurrentNode(pNode);
	
	if (pNode->nChildCount > 0)
	{
		this->SetExpanded(!pNode->bExpanded);
	}
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "click",
			FrameArgList() << (int)pNode->nIdentity, NULL);
	}

	return 1;
}

bool FlatTreeView::MoveToPrev()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	tree_node_t* pParent = m_pCurrent->pParent;

	if (NULL == pParent)
	{
		return false;
	}

	int node_index = -1;

	for (size_t i = 0; i < pParent->nChildCount; ++i)
	{
		if (pParent->pChildren[i] == m_pCurrent)
		{
			node_index = (int)i;
			break;
		}
	}

	if (node_index < 0)
	{
		return false;
	}

	if (node_index > 0)
	{
		this->SetCurrentNode(pParent->pChildren[node_index - 1]);
	}
	else if (node_index == 0)
	{
		if (m_bHideRoot && (pParent == m_pRoot))
		{
			return false;
		}
		
		this->SetCurrentNode(pParent);
	}

	return true;
}

bool FlatTreeView::MoveToNext()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	tree_node_t* pParent = m_pCurrent->pParent;

	if (NULL == pParent)
	{
		if (!m_pCurrent->bExpanded)
		{
			return false;
		}

		if (0 == m_pCurrent->nChildCount)
		{
			return false;
		}

		this->SetCurrentNode(m_pCurrent->pChildren[0]);
		return true;
	}

	int node_index = -1;

	for (size_t i = 0; i < pParent->nChildCount; ++i)
	{
		if (pParent->pChildren[i] == m_pCurrent)
		{
			node_index = (int)i;
			break;
		}
	}

	if (node_index < 0)
	{
		return false;
	}

	if (node_index == (int)(pParent->nChildCount - 1))
	{
		if (!m_pCurrent->bExpanded)
		{
			return false;
		}

		if (0 == m_pCurrent->nChildCount)
		{
			return false;
		}

		this->SetCurrentNode(m_pCurrent->pChildren[0]);
	}
	else
	{
		this->SetCurrentNode(pParent->pChildren[node_index + 1]);
	}

	return true;
}

FlatTreeView::tree_node_t* FlatTreeView::GetLastVisibleNode(tree_node_t* pNode)
{
	Assert(pNode != NULL);

	if (pNode->bExpanded && (pNode->nChildCount > 0))
	{
		return this->GetLastVisibleNode(
			pNode->pChildren[pNode->nChildCount - 1]);
	}

	return pNode;
}

int FlatTreeView::OnKeyDown(unsigned int key, unsigned int flags)
{
	switch (key)
	{
	case IInputService::KEY_UP:
	{
		if (!this->MoveToPrev())
		{
			break;
		}
		
		return 1;
	}
	case IInputService::KEY_DOWN:
	{
		if (!this->MoveToNext())
		{
			break;
		}

		return 1;
	}
	case IInputService::KEY_LEFT:
	{
		if (m_nHorizontalBegin == 0)
		{
			break;
		}

		int new_value = m_nHorizontalBegin - 4;

		if (new_value < 0)
		{
			new_value = 0;
		}

		this->SetHorizontalBegin(new_value);
		flat_scroll_adjust_hscroll(this->GetScroll());
		return 1;
	}
	case IInputService::KEY_RIGHT:
	{
		int reveal_width = flat_scroll_reveal_width(this->GetScroll());

		if ((m_nHorizontalBegin + reveal_width) >= m_nMaxItemWidth)
		{
			break;
		}

		int new_value = m_nHorizontalBegin + 4;

		if ((new_value + reveal_width) >= m_nMaxItemWidth)
		{
			new_value = m_nMaxItemWidth - reveal_width;
		}

		this->SetHorizontalBegin(new_value);
		flat_scroll_adjust_hscroll(this->GetScroll());
		return 1;
	}
	case IInputService::KEY_PRIOR:
	{
		if (NULL == m_pRoot)
		{
			break;
		}

		if (m_pCurrent == m_pRoot)
		{
			break;
		}
		
		int page_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nItemHeight - 1;

		if (page_num < 1)
		{
			page_num = 1;
		}

		this->BeginUpdate();

		for (int i = 0; i < page_num; ++i)
		{
			if (!this->MoveToPrev())
			{
				break;
			}
		}

		this->EndUpdate();
		return 1;
	}
	case IInputService::KEY_NEXT:
	{
		if (NULL == m_pRoot)
		{
			break;
		}

		int page_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nItemHeight - 1;

		if (page_num < 1)
		{
			page_num = 1;
		}

		this->BeginUpdate();

		int i = 0; 

		for (; i < page_num; ++i)
		{
			if (!this->MoveToNext())
			{
				break;
			}
		}

		this->EndUpdate();

		if (i == 0)
		{
			break;
		}

		return 1;
	}
	case IInputService::KEY_HOME:
	{
		if (NULL == m_pRoot)
		{
			break;
		}

		if (m_bHideRoot)
		{
			if (!m_pRoot->bExpanded)
			{
				break;
			}

			if (m_pRoot->nChildCount == 0)
			{
				break;
			}

			this->SetCurrentNode(m_pRoot->pChildren[0]);
		}
		else
		{
			this->SetCurrentNode(m_pRoot);
		}

		return 1;
	}
	case IInputService::KEY_END:
	{
		if (NULL == m_pRoot)
		{
			break;
		}

		tree_node_t* pNode = this->GetLastVisibleNode(m_pRoot);

		if (m_bHideRoot && (pNode == m_pRoot))
		{
			break;
		}

		this->SetCurrentNode(pNode);
		break;
	}
	case IInputService::KEY_RETURN:
	{
		if (NULL == m_pCurrent)
		{
			break;
		}

		if (m_pCurrent->nChildCount > 0)
		{
			this->SetExpanded(!m_pCurrent->bExpanded);
		}

		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click",
				FrameArgList() << (int)m_pCurrent->nIdentity, NULL);
		}

		break;
	}
	default:
		break;
	}

	return 0;
}

int FlatTreeView::OnSizeChanged()
{
	this->PerformLayout();
	this->UpdateRowCount();
	this->UpdateScroll();
	return 1;
}

int FlatTreeView::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	flat_scroll_changed(this->GetScroll(), pPart, new_value, old_value);
	return 1;
}

void FlatTreeView::SetItemHeight(int value)
{
	if (value <= 0)
	{
		return;
	}

	m_nItemHeight = value;
}

int FlatTreeView::GetItemHeight() const
{
	return m_nItemHeight;
}

void FlatTreeView::SetIndentWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nIndentWidth = value;
}

int FlatTreeView::GetIndentWidth() const
{
	return m_nIndentWidth;
}

void FlatTreeView::SetScrollSize(int value)
{
	if (value < 0)
	{
		return;
	}

	flat_scroll_set_bar_size(this->GetScroll(), value);
	this->PerformLayout();
}

int FlatTreeView::GetScrollSize() const
{
	return flat_scroll_get_bar_size(this->GetScroll());
}

void FlatTreeView::SetDynamicSlide(bool value)
{
	flat_scroll_set_dynamic_slide(this->GetScroll(), value);
}

bool FlatTreeView::GetDynamicSlide() const
{
	return flat_scroll_get_dynamic_slide(this->GetScroll());
}

//void FlatTreeView::SetScrollStyle(const char* value)
//{
//	Assert(value != NULL);
//
//	m_sScrollStyle = value;
//
//	if (!util_string_empty(value))
//	{
//		FlatWidget* pWidget = this->GetFlatUI()->GetScrollStyle(value);
//
//		if (pWidget)
//		{
//			flat_scroll_t* pSrcScroll = pWidget->GetScroll();
//
//			Assert(pSrcScroll != NULL);
//
//			flat_scroll_clone_style(this->GetScroll(), pSrcScroll);
//		}
//	}
//}

//const char* FlatTreeView::GetScrollStyle() const
//{
//	return m_sScrollStyle.CString();
//}

void FlatTreeView::SetVScrollAlways(bool value)
{
	flat_scroll_set_vscroll_always(this->GetScroll(), value);
}

bool FlatTreeView::GetVScrollAlways() const
{
	return flat_scroll_get_vscroll_always(this->GetScroll());
}

void FlatTreeView::SetHScrollAlways(bool value)
{
	flat_scroll_set_hscroll_always(this->GetScroll(), value);
}

bool FlatTreeView::GetHScrollAlways() const
{
	return flat_scroll_get_hscroll_always(this->GetScroll());
}

void FlatTreeView::SetShowPlusMinus(bool value)
{
	m_bShowPlusMinus = value;

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();
	}
}

bool FlatTreeView::GetShowPlusMinus() const
{
	return m_bShowPlusMinus;
}

void FlatTreeView::SetSorted(bool value)
{
	m_bSorted = value;
}

bool FlatTreeView::GetSorted() const
{
	return m_bSorted;
}

void FlatTreeView::SetHideRoot(bool value)
{
	m_bHideRoot = value;
}

bool FlatTreeView::GetHideRoot() const
{
	return m_bHideRoot;
}

void FlatTreeView::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatTreeView::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}

void FlatTreeView::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatTreeView::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatTreeView::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatTreeView::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

void FlatTreeView::SetPlusImage(const char* value)
{
	Assert(value != NULL);

	m_sPlusImage = value;
	m_pPlusImage = flat_load_image(this->GetFlatUI(), m_pPlusImage, value,
		false);
}

const char* FlatTreeView::GetPlusImage() const
{
	return m_sPlusImage.CString();
}

void FlatTreeView::SetMinusImage(const char* value)
{
	Assert(value != NULL);

	m_sMinusImage = value;
	m_pMinusImage = flat_load_image(this->GetFlatUI(), m_pMinusImage, value,
		false);
}

const char* FlatTreeView::GetMinusImage() const
{
	return m_sMinusImage.CString();
}

uniqid_t FlatTreeView::GetVScrollBarID() const
{
	return flat_scroll_vscroll_id(this->GetScroll());
}

uniqid_t FlatTreeView::GetHScrollBarID() const
{
	return flat_scroll_hscroll_id(this->GetScroll());
}

uniqid_t FlatTreeView::GetCrossLabelID() const
{
	return flat_scroll_cross_label_id(this->GetScroll());
}

bool FlatTreeView::CreateRoot(const char* text, bool expand)
{
	Assert(text != NULL);

	if (m_pRoot != NULL)
	{
		return false;
	}

	tree_node_t* pNode = this->NewNode();

	pNode->sText = text;
	pNode->nLevel = 0;
	pNode->nIndex = 0;
	pNode->bExpanded = expand;
	pNode->bFirstExpand = expand;
	pNode->pParent = NULL;
	pNode->nChildCount = 0;
	m_pRoot = pNode;
	m_pCurrent = pNode;

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();
	}

	return true;
}

int FlatTreeView::AddText(const char* text, bool expand)
{
	Assert(text != NULL);

	if (NULL == m_pCurrent)
	{
		return -1;
	}

	tree_node_t* pNode = this->NewNode();

	pNode->sText = text;
	pNode->nLevel = m_pCurrent->nLevel + 1;
	pNode->bExpanded = expand;
	pNode->bFirstExpand = expand;
	pNode->pParent = m_pCurrent;
	pNode->nChildCount = 0;

	if (m_bSorted)
	{
		this->AddSortedNode(m_pCurrent, pNode, 0, (int)m_pCurrent->nChildCount);
	}
	else
	{
		this->AddNode(m_pCurrent, (int)m_pCurrent->nChildCount, pNode);
	}

	m_pCurrent = pNode;

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();
	}

	return pNode->nIndex;
}

bool FlatTreeView::RemoveText(const char* text)
{
	Assert(text != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	for (size_t i = 0; i < m_pCurrent->nChildCount; ++i)
	{
		tree_node_t* pNode = m_pCurrent->pChildren[i];

		if (strcmp(pNode->sText.CString(), text) == 0)
		{
			this->RemoveNode(pNode);
			return true;
		}
	}
	
	return false;
}

bool FlatTreeView::RemoveIndex(int index)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if ((size_t)index >= m_pCurrent->nChildCount)
	{
		return false;
	}

	this->RemoveNode(m_pCurrent->pChildren[index]);
	return true;
}

bool FlatTreeView::RemoveIdentity(int identity)
{
	if (NULL == m_pRoot)
	{
		return false;
	}

	size_t id = (size_t)identity;
	
	if (id >= m_NodeBuffer.Size())
	{
		return false;
	}

	tree_node_t* pNode = m_NodeBuffer[id];

	if (pNode->nIdentity != id)
	{
		return false;
	}

	this->RemoveNode(pNode);
	return true;
}

bool FlatTreeView::Clear()
{
	for (size_t i = 0; i < m_NodeBuffer.Size(); ++i)
	{
		tree_node_t* pNode = m_NodeBuffer[i];

		if (pNode->pChildren)
		{
			K_FREE(pNode->pChildren);
		}

		K_DELETE(pNode);
	}

	m_DisplayNodes.Clear();
	m_NodeBuffer.Clear();
	m_NodeFree.Clear();
	m_pRoot = NULL;
	m_pCurrent = NULL;
	m_nVisibleRowCount = 0;

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();
	}

	return true;
}

bool FlatTreeView::SelectRoot()
{
	if (NULL == m_pRoot)
	{
		return false;
	}
	
	this->SetCurrentNode(m_pRoot);
	return true;
}

bool FlatTreeView::SelectFirstChild()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (0 == m_pCurrent->nChildCount)
	{
		return false;
	}

	this->SetCurrentNode(m_pCurrent->pChildren[0]);
	return true;
}

bool FlatTreeView::SelectNextSibling()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	tree_node_t* pParent = m_pCurrent->pParent;

	if (NULL == pParent)
	{
		return false;
	}

	size_t next_index = m_pCurrent->nIndex + 1;

	if (next_index >= pParent->nChildCount)
	{
		return false;
	}

	this->SetCurrentNode(pParent->pChildren[next_index]);
	return true;
}

bool FlatTreeView::SelectText(const char* text)
{
	Assert(text != NULL);
	
	if (NULL == m_pCurrent)
	{
		return false;
	}
	
	for (size_t i = 0; i < m_pCurrent->nChildCount; ++i)
	{
		tree_node_t* pNode = m_pCurrent->pChildren[i];

		if (strcmp(pNode->sText.CString(), text) == 0)
		{
			this->SetCurrentNode(pNode);
			return true;
		}
	}

	return false;
}

bool FlatTreeView::SelectIndex(int index)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if ((size_t)index >= m_pCurrent->nChildCount)
	{
		return false;
	}

	this->SetCurrentNode(m_pCurrent->pChildren[index]);
	return true;
}

bool FlatTreeView::SelectIdentity(int identity)
{
	size_t id = (size_t)identity;

	if (id >= m_NodeBuffer.Size())
	{
		return false;
	}

	tree_node_t* pNode = m_NodeBuffer[id];

	if (pNode->nIdentity != id)
	{
		return false;
	}
	
	this->SetCurrentNode(pNode);
	return true;
}

bool FlatTreeView::SelectParent()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	tree_node_t* pParent = m_pCurrent->pParent;

	if (NULL == pParent)
	{
		return false;
	}
	
	this->SetCurrentNode(pParent);
	return true;
}

bool FlatTreeView::SelectNull()
{
	this->SetCurrentNode(NULL);
	return true;
}

const char* FlatTreeView::GetText()
{
	if (NULL == m_pCurrent)
	{
		return "";
	}

	return m_pCurrent->sText.CString();
}

const char* FlatTreeView::GetTag()
{
	if (NULL == m_pCurrent)
	{
		return "";
	}

	return m_pCurrent->sTag.CString();
}

bool FlatTreeView::SetTag(const char* tag)
{
	Assert(tag != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	m_pCurrent->sTag = tag;
	return true;
}

int FlatTreeView::SearchTag(tree_node_t* pNode, const char* tag)
{
	Assert(pNode != NULL);
	Assert(tag != NULL);

	if (pNode->sTag == tag)
	{
		return (int)pNode->nIdentity;
	}

	for (int i = 0; i < pNode->nChildCount; ++i)
	{
		int identity = this->SearchTag(pNode->pChildren[i], tag);

		if (identity >= 0)
		{
			return identity;
		}
	}

	return -1;
}

int FlatTreeView::FindTag(const char* tag)
{
	Assert(tag != NULL);

	if (NULL == m_pRoot)
	{
		return -1;
	}

	return this->SearchTag(m_pRoot, tag);
}

int FlatTreeView::GetLevel()
{
	if (NULL == m_pCurrent)
	{
		return -1;
	}

	return m_pCurrent->nLevel;
}

int FlatTreeView::GetIndex()
{
	if (NULL == m_pCurrent)
	{
		return -1;
	}

	return m_pCurrent->nIndex;
}

int FlatTreeView::GetIdentity()
{
	if (NULL == m_pCurrent)
	{
		return -1;
	}

	return (int)m_pCurrent->nIdentity;
}

bool FlatTreeView::GetExpanded()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	return m_pCurrent->bExpanded;
}

bool FlatTreeView::SetExpanded(bool value)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (value == m_pCurrent->bExpanded)
	{
		return false;
	}

	m_pCurrent->bExpanded = value;

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();

		if (m_pCurrent->bExpanded && (!m_pCurrent->bFirstExpand))
		{
			m_pCurrent->bFirstExpand = true;

			if (this->GetEnabled())
			{
				frame_process_event(this, 0, "expand", FrameArgList(), NULL);
			}
		}
	}

	return true;
}

int FlatTreeView::GetChildCount()
{
	if (NULL == m_pCurrent)
	{
		return 0;
	}

	return (int)m_pCurrent->nChildCount;
}

void FlatTreeView::GetChildList(const IArgList& args, IArgList* res)
{
	res->Clear();

	if (m_pCurrent)
	{
		for (size_t i = 0; i < m_pCurrent->nChildCount; ++i)
		{
			res->AddInt32((int)m_pCurrent->pChildren[i]->nIdentity);
		}
	}
}

bool FlatTreeView::BeginUpdate()
{
	m_bUpdating = true;
	return true;
}

bool FlatTreeView::EndUpdate()
{
	m_bUpdating = false;
	this->UpdateRowCount();
	this->UpdateScroll();
	return true;
}

int FlatTreeView::AddSortedNode(tree_node_t* pParent, tree_node_t* pNode,
	int head, int tail)
{
	Assert(pParent != NULL);
	Assert(pNode != NULL);
	Assert((size_t)head <= pParent->nChildCount);
	Assert((size_t)tail <= pParent->nChildCount);

	if (head == tail)
	{
		this->AddNode(pParent, head, pNode);
		return head;
	}

	int index = head + (tail - head) / 2;
	tree_node_t* p = pParent->pChildren[index];

	if (stricmp(p->sText.CString(), pNode->sText.CString()) >= 0)
	{
		return this->AddSortedNode(pParent, pNode, head, index);
	}
	else
	{
		return this->AddSortedNode(pParent, pNode, index + 1, tail);
	}
}

void FlatTreeView::AddNode(tree_node_t* pParent, int index, tree_node_t* pNode)
{
	Assert(pParent != NULL);
	Assert((size_t)index <= pParent->nChildCount);
	Assert(pNode != NULL);

	if (pParent->nChildCount == pParent->nChildSize)
	{
		size_t new_size = pParent->nChildSize * 2;

		if (0 == new_size)
		{
			new_size = 8;
		}

		tree_node_t** pNewNodes = (tree_node_t**)K_ALLOC(
			sizeof(tree_node_t*) * new_size);

		if (pParent->pChildren)
		{
			memcpy(pNewNodes, pParent->pChildren,
				sizeof(tree_node_t*) * pParent->nChildCount);
			K_FREE(pParent->pChildren);
		}

		pParent->pChildren = pNewNodes;
		pParent->nChildSize = new_size;
	}

	if ((size_t)index < pParent->nChildCount)
	{
		for (size_t i = pParent->nChildCount; i > index; --i)
		{
			tree_node_t* p = pParent->pChildren[i - 1];

			pParent->pChildren[i] = p;
			p->nIndex++;
		}

		pNode->nIndex = index;
		pParent->pChildren[index] = pNode;
	}
	else
	{
		pNode->nIndex = (int)pParent->nChildCount;
		pParent->pChildren[pParent->nChildCount] = pNode;
	}

	pParent->nChildCount++;
}

void FlatTreeView::RemoveNode(tree_node_t* pNode)
{
	Assert(pNode != NULL);

	tree_node_t* pParent = pNode->pParent;

	if (pParent)
	{
		Assert((size_t)pNode->nIndex < pParent->nChildCount);

		for (size_t i = pNode->nIndex + 1; i < pParent->nChildCount; ++i)
		{
			tree_node_t* p = pParent->pChildren[i];

			pParent->pChildren[i - 1] = p;
			p->nIndex--;
		}

		pParent->nChildCount--;
	}

	this->DeleteNode(pNode);

	if (m_pCurrent == pNode)
	{
		m_pCurrent = NULL;
	}

	if (m_pRoot == pNode)
	{
		m_pRoot = NULL;
	}

	if (!m_bUpdating)
	{
		this->UpdateRowCount();
		this->UpdateScroll();
	}
}

FlatTreeView::tree_node_t* FlatTreeView::NewNode()
{
	if (m_NodeFree.Empty())
	{
		tree_node_t* pNode = K_NEW(tree_node_t);

		pNode->nIdentity = m_NodeBuffer.Size();
		pNode->nLevel = 0;
		pNode->nIndex = 0;
		pNode->bExpanded = false;
		pNode->pParent = NULL;
		pNode->pChildren = NULL;
		pNode->nChildSize = 0;
		pNode->nChildCount = 0;
		m_NodeBuffer.PushBack(pNode);
		return pNode;
	}
	else
	{
		size_t index = m_NodeFree.Back();

		Assert(index < m_NodeBuffer.Size());

		m_NodeFree.PopBack();

		tree_node_t* pNode = m_NodeBuffer[index];

		pNode->nIdentity = index;
		return pNode;
	}
}

void FlatTreeView::DeleteNode(tree_node_t* pNode)
{
	Assert(pNode != NULL);

	if (pNode->nChildCount > 0)
	{
		for (size_t i = 0; i < pNode->nChildCount; ++i)
		{
			this->DeleteNode(pNode->pChildren[i]);
		}

		pNode->nChildCount = 0;
	}

	pNode->nIdentity = (size_t)(-1);
	m_NodeFree.PushBack((size_t)pNode->nIdentity);
}

void FlatTreeView::SetCurrentNode(tree_node_t* pNode)
{
	if (pNode == m_pCurrent)
	{
		return;
	}

	int old_identity = this->GetIdentity();

	m_pCurrent = pNode;
	
	if ((!m_bHideRoot) || (pNode != m_pRoot))
	{
		this->ShowSelected();
	}

	if (!m_bUpdating)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select",
				FrameArgList() << old_identity, NULL);
		}
	}
}

int FlatTreeView::GetNodeMaxItemWidth(tree_node_t* pNode)
{
	Assert(pNode != NULL);

	int level = pNode->nLevel;

	if (m_bHideRoot)
	{
		--level;
	}
	
	int item_width = level * m_nIndentWidth;
	
	item_width += this->GetFlatUI()->GetTextWidth(this->GetAvailableFont(),
		pNode->sText.CString(), 0, 0);

	if (pNode->bExpanded)
	{
		if (m_bShowPlusMinus && (pNode->nChildCount > 0))
		{
			if (m_pMinusImage)
			{
				item_width += flat_image_get_width(m_pMinusImage);
			}
			else
			{
				item_width += this->GetFlatUI()->GetTextWidth(
					this->GetAvailableFont(), "-", 0, 0);
			}
		}

		for (size_t i = 0; i < pNode->nChildCount; ++i)
		{
			int child_width = this->GetNodeMaxItemWidth(pNode->pChildren[i]);

			if (child_width > item_width)
			{
				item_width = child_width;
			}
		}
	}
	else
	{
		if (m_bShowPlusMinus && (pNode->nChildCount > 0))
		{
			if (m_pPlusImage)
			{
				item_width += flat_image_get_width(m_pPlusImage);
			}
			else
			{
				item_width += this->GetFlatUI()->GetTextWidth(
					this->GetAvailableFont(), "+", 0, 0);
			}
		}
	}

	return item_width;
}

void FlatTreeView::UpdateMaxItemWidth()
{
	if (m_pRoot)
	{
		m_nMaxItemWidth = this->GetNodeMaxItemWidth(m_pRoot);
	}
	else
	{
		m_nMaxItemWidth = 0;
	}
}

int FlatTreeView::GetNodeVerticalBegin(tree_node_t* pNode, 
	tree_node_t* pCurrent)
{
	Assert(pNode != NULL);
	Assert(pCurrent != NULL);

	int height = 0;
	
	if (pNode == pCurrent)
	{
		return height;
	}

	if ((!m_bHideRoot) || (pNode != m_pRoot))
	{
		height += m_nItemHeight;
	}

	for (size_t i = 0; i < pNode->nChildCount; ++i)
	{
		tree_node_t* pChild = pNode->pChildren[i];
		
		if (pChild == pCurrent)
		{
			break;
		}
		
		if (pNode->bExpanded)
		{
			height += this->GetNodeVerticalBegin(pChild, pCurrent);
		}
	}
	
	return height;
}

void FlatTreeView::ShowSelected()
{
	if (NULL == m_pCurrent)
	{
		return;
	}

	int reveal_height = flat_scroll_reveal_height(this->GetScroll());
	int y1 = this->GetNodeVerticalBegin(m_pRoot, m_pCurrent);

	if (y1 < m_nVerticalBegin)
	{
		this->SetVerticalBegin(y1);
		flat_scroll_adjust_vscroll(this->GetScroll());
	}
	else
	{
		int y2 = y1 + m_nItemHeight;
		int reveal_height = flat_scroll_reveal_height(this->GetScroll());

		if (y2 >(m_nVerticalBegin + reveal_height))
		{
			this->SetVerticalBegin(y2 - reveal_height);
			flat_scroll_adjust_vscroll(this->GetScroll());
		}
	}
}

void FlatTreeView::PerformLayout()
{
	flat_scroll_perform_layout(this->GetScroll());
}

void FlatTreeView::UpdateScroll()
{
	this->UpdateMaxItemWidth();
	flat_scroll_update(this->GetScroll());
}

int FlatTreeView::GetNodeRowCount(tree_node_t* pNode)
{
	Assert(pNode != NULL);

	int count = 1;

	if (m_bHideRoot && (pNode == m_pRoot))
	{
		count = 0;
	}

	if (pNode->bExpanded)
	{
		for (size_t i = 0; i < pNode->nChildCount; ++i)
		{
			count += this->GetNodeRowCount(pNode->pChildren[i]);
		}
	}

	return count;
}

void FlatTreeView::UpdateRowCount()
{
	if (m_pRoot)
	{
		m_nVisibleRowCount = this->GetNodeRowCount(m_pRoot);
	}
	else
	{
		m_nVisibleRowCount = 0;
	}

	this->UpdateDisplayNodes();
}

void FlatTreeView::GetDiplayNodes(tree_node_t* pNode, int* pRowCount)
{
	Assert(pNode != NULL);
	Assert(pRowCount != NULL);

	int y1 = (*pRowCount) * m_nItemHeight - m_nVerticalBegin;
	
	if ((!m_bHideRoot) || (pNode != m_pRoot))
	{
		int y2 = y1 + m_nItemHeight;

		if ((y2 > 0) && (y1 < this->GetHeight()))
		{
			m_DisplayNodes.PushBack(pNode);
		}

		(*pRowCount)++;
	}

	if (y1 < this->GetHeight())
	{
		if (pNode->bExpanded)
		{
			for (size_t i = 0; i < pNode->nChildCount; ++i)
			{
				this->GetDiplayNodes(pNode->pChildren[i], pRowCount);
			}
		}
	}
}

void FlatTreeView::UpdateDisplayNodes()
{
	m_DisplayNodes.Clear();
	
	if (m_pRoot)
	{
		int row_count = 0;

		this->GetDiplayNodes(m_pRoot, &row_count);
	}
}
