// flat_list_box.cpp
// Created by lulimin on 2020/9/11.

#include "flat_list_box.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_list_box.cpp
/// \brief User interface list box.

/// object: FlatListBox
/// \brief User interface list box object.
DEF_OBJECT(FlatListBox, FlatWidget);

/// readonly: object VScrollBar
/// \brief Vertical scroll bar.
DEF_READ_F(uniqid_t, FlatListBox, VScrollBar, GetVScrollBarID);
/// readonly: object HScrollBar
/// \brief Horizontal scroll bar.
DEF_READ_F(uniqid_t, FlatListBox, HScrollBar, GetHScrollBarID);
/// readonly: object CrossLabel
/// \brief Cross label.
DEF_READ_F(uniqid_t, FlatListBox, CrossLabel, GetCrossLabelID);

/// property: int ItemHeight
/// \brief Height of list item.
DEF_PROP(int, FlatListBox, ItemHeight);
/// property: int SelectedIndex
/// \brief Index of selected item.
DEF_PROP(int, FlatListBox, SelectedIndex);
/// property: int ScrollSize
/// \brief Scroll bar size.
DEF_PROP(int, FlatListBox, ScrollSize);
/// property: bool DynamicSlide
/// \brief Dynamic scroll slide bar size.
DEF_PROP(bool, FlatListBox, DynamicSlide);
/// property: bool VScrollAlways
/// \brief Always show vertical scroll bar.
DEF_PROP(bool, FlatListBox, VScrollAlways);
/// property: bool HScrollExists
/// \brief Show horizontal scroll bar.
DEF_PROP(bool, FlatListBox, HScrollExists);
/// property: bool Sorted
/// \brief Sort text item.
DEF_PROP(bool, FlatListBox, Sorted);
/// property: string SelectForeColor
/// \brief Fore color of selected item.
DEF_PROP(ArgString, FlatListBox, SelectForeColor);
/// property: string SelectBackColor
/// \brief Background color of selected item.
DEF_PROP(ArgString, FlatListBox, SelectBackColor);
/// property: string CaptureColor
/// \brief Background color of captured item.
DEF_PROP(ArgString, FlatListBox, CaptureColor);
/// property: string ScrollStyle
/// \brief Preset scroll style.
//DEF_PROP(const char*, FlatListBox, ScrollStyle);
/// property: string SelectedText
/// \brief Text of selected item.
DEF_PROP(const char*, FlatListBox, SelectedText);

/// function: int AddText(string text)
/// \brief Add text item.
/// \param text Text value.
DEF_FUNC_1(int, FlatListBox, AddText, const char*);
/// function: bool RemoveText(string text)
/// \brief Remove text item.
/// \param text Text value.
DEF_FUNC_1(bool, FlatListBox, RemoveText, const char*);
/// function: bool RemoveByIndex(int index)
/// \brief Remove text item by index.
/// \param index Item index.
DEF_FUNC_1(bool, FlatListBox, RemoveByIndex, int);
/// function: bool ClearTexts()
/// \brief Clear item texts.
DEF_FUNC_0(bool, FlatListBox, ClearTexts);
/// function: int FindText(string text)
/// \brief Find text item.
/// \param text Text value.
DEF_FUNC_1(int, FlatListBox, FindText, const char*);
/// function: string GetText(int index)
/// \brief Get item text.
/// \param index Item index.
DEF_FUNC_1(const char*, FlatListBox, GetText, int);
/// function: int GetTextCount()
/// \brief Get number of text item.
DEF_FUNC_0(int, FlatListBox, GetTextCount);

/// function: bool SetTopIndex(int item_index)
/// \brief Set top visible list item.
/// \param item_index Index of list item.
DEF_FUNC_1(bool, FlatListBox, SetTopIndex, int);

/// function: bool BeginUdpate()
/// \brief Begin update.
DEF_FUNC_0(bool, FlatListBox, BeginUpdate);
/// function: bool EndUpdate()
/// \brief End update.
DEF_FUNC_0(bool, FlatListBox, EndUpdate);

#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000
#define INIT_CAPTURE_COLOR 0xFF800000

// FlatListBox

FlatListBox::FlatListBox()
{
	m_nItemHeight = 16;
	m_nCaptureIndex = -1;
	m_nSelectedIndex = -1;
	m_nVerticalBegin = 0;
	m_nHorizontalBegin = 0;
	m_nMaxItemWidth = 0;
	m_bSorted = false;
	m_bUpdating = false;
	m_nSelectForeColor = INIT_SELECT_FORE_COLOR;
	m_nSelectBackColor = INIT_SELECT_BACK_COLOR;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
}

FlatListBox::~FlatListBox()
{
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		K_DELETE(m_ListItems[i]);
	}
}

bool FlatListBox::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();
	this->CreateScroll();
	flat_scroll_initialize(this->GetScroll(), this->GetFlatUI(), true, false);
	return true;
}

bool FlatListBox::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatListBox::Draw(flat_canvas_t* pCanvas)
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

void FlatListBox::SetMetadata(flat_metadata_t* pMD)
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
	flat_metadata_add_prop(pMD, "ScrollSize", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DynamicSlide", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "VScrollAlways", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "HScrollExists", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "Sorted", FLAT_METATYPE_BOOLEAN,
		&def_false);
	//flat_metadata_add_prop(pMD, "ScrollStyle", FLAT_METATYPE_STRING,
	//	&def_empty_string);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_part_prop(pMD, "BackImage");
	flat_metadata_part_prop(pMD, "ImageLayout");
	flat_metadata_part_prop(pMD, "ItemHeight");
	flat_metadata_part_prop(pMD, "Sorted");
	//flat_metadata_part_prop(pMD, "ScrollStyle");
	flat_metadata_part_prop(pMD, "SelectForeColor");
	flat_metadata_part_prop(pMD, "SelectBackColor");
	flat_metadata_part_prop(pMD, "CaptureColor");
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_event(pMD, "click");
	flat_metadata_add_part(pMD, "VScrollBar", "FlatVScrollBar");
	flat_metadata_add_part(pMD, "HScrollBar", "FlatHScrollBar");
	flat_metadata_add_part(pMD, "CrossLabel", "FlatLabel");
}

void FlatListBox::DrawScrollRegion(flat_canvas_t* pCanvas)
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
		int index = i + beg;
		
		if (index >= (int)m_ListItems.Size())
		{
			break;
		}

		int x2 = x1 + width;
		int y2 = y1 + m_nItemHeight;
		unsigned int fore_color;

		if (m_nSelectedIndex == index)
		{
			canvas_draw_block(pCanvas, x1, y1, x2, y2, m_nSelectBackColor);
			fore_color = m_nSelectForeColor;
		}
		else
		{
			if (this->GetCapture() && (m_nCaptureIndex == index))
			{
				canvas_draw_block(pCanvas, x1, y1, x2, y2, m_nCaptureColor);
			}
			
			fore_color = this->GetForeColorValue();
		}

		canvas_draw_text_left(pCanvas, x1, y1, x2, y2, fore_color,
			m_ListItems[index]->sText.CString(), 0, 0);
		y1 += m_nItemHeight;
	}
}

int FlatListBox::GetVerticalWhole()
{
	return (int)m_ListItems.Size() * m_nItemHeight;
}

int FlatListBox::GetVerticalReveal()
{
	return flat_scroll_reveal_height(this->GetScroll());
}

int FlatListBox::GetVerticalBegin()
{
	return m_nVerticalBegin;
}

void FlatListBox::SetVerticalBegin(int value)
{
	m_nVerticalBegin = value;
}

int FlatListBox::GetHorizontalWhole()
{
	//return this->GetWidth();
	return m_nMaxItemWidth;
}

int FlatListBox::GetHorizontalReveal()
{
	return flat_scroll_reveal_width(this->GetScroll());
}

int FlatListBox::GetHorizontalBegin()
{
	return m_nHorizontalBegin;
}

void FlatListBox::SetHorizontalBegin(int value)
{
	m_nHorizontalBegin = value;
}

int FlatListBox::GetInItemIndex(int x, int y)
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

		if (index >= (int)m_ListItems.Size())
		{
			break;
		}

		int x2 = x1 + width;
		int y2 = y1 + m_nItemHeight;

		if (flat_in_rectangle(x, y, x1, y1, x2, y2))
		{
			return index;
		}
		
		y1 += m_nItemHeight;
	}
	
	return -1;
}

int FlatListBox::OnMouseMove(int x, int y, unsigned int flags)
{
	if (m_nCaptureColor != 0)
	{
		int index = this->GetInItemIndex(x, y);

		if (index >= 0)
		{
			m_nCaptureIndex = index;
		}
		else
		{
			m_nCaptureIndex = -1;
		}
	}

	return 1;
}

int FlatListBox::OnMouseWheel(int x, int y, double delta, unsigned int flags)
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

int FlatListBox::OnLeftDown(int x, int y, unsigned int flags)
{
	int index = this->GetInItemIndex(x, y);

	if (index < 0)
	{
		return 0;
	}

	this->SetSelectedIndex(index);
	return 1;
}

int FlatListBox::OnLeftDual(int x, int y, unsigned int flags)
{
	int index = this->GetInItemIndex(x, y);

	if (index < 0)
	{
		return 0;
	}
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "click", FrameArgList() << index, NULL);
	}

	return 1;
}

int FlatListBox::OnKeyDown(unsigned int key, unsigned int flags)
{
	switch (key)
	{
	case IInputService::KEY_UP:
	{
		if (m_nSelectedIndex <= 0)
		{
			break;
		}

		this->SetSelectedIndex(m_nSelectedIndex - 1);
		return 1;
	}
	case IInputService::KEY_DOWN:
	{
		if ((m_nSelectedIndex + 1) >= (int)m_ListItems.Size())
		{
			break;
		}
		
		this->SetSelectedIndex(m_nSelectedIndex + 1);
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
		if (m_ListItems.Size() == 0)
		{
			break;
		}
		
		if (m_nSelectedIndex < 0)
		{
			break;
		}

		if (0 == m_nSelectedIndex)
		{
			break;
		}
		
		int page_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nItemHeight - 1;
		int new_value = m_nSelectedIndex - page_num;

		if (new_value)
		{
			new_value = 0;
		}

		this->SetSelectedIndex(new_value);
		return 1;
	}
	case IInputService::KEY_NEXT:
	{
		if (m_ListItems.Size() == 0)
		{
			break;
		}

		if (m_nSelectedIndex < 0)
		{
			break;
		}

		if (m_nSelectedIndex == (int)(m_ListItems.Size() - 1))
		{
			break;
		}

		int page_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nItemHeight - 1;
		int new_value = m_nSelectedIndex + page_num;

		if (new_value >= (int)(m_ListItems.Size() - 1))
		{
			new_value = (int)(m_ListItems.Size() - 1);
		}

		this->SetSelectedIndex(new_value);
		return 1;
	}
	case IInputService::KEY_HOME:
	{
		if (m_ListItems.Size() == 0)
		{
			break;
		}
		
		this->SetSelectedIndex(0);
		break;
	}
	case IInputService::KEY_END:
	{
		if (m_ListItems.Size() == 0)
		{
			break;
		}
		
		this->SetSelectedIndex((int)m_ListItems.Size() - 1);
		break;
	}
	case IInputService::KEY_RETURN:
	{
		if (m_nSelectedIndex < 0)
		{
			break;
		}
		
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click",
				FrameArgList() << m_nSelectedIndex, NULL);
		}

		break;
	}
	default:
		break;
	}
	
	return 0;
}

int FlatListBox::OnSizeChanged()
{
	this->PerformLayout();
	this->UpdateScroll();
	return 1;
}

int FlatListBox::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	flat_scroll_changed(this->GetScroll(), pPart, new_value, old_value);
	return 1;
}

void FlatListBox::SetItemHeight(int value)
{
	if (value <= 0)
	{
		return;
	}
	
	m_nItemHeight = value;
}

int FlatListBox::GetItemHeight() const
{
	return m_nItemHeight;
}

void FlatListBox::SetSelectedIndex(int value)
{
	if (value == m_nSelectedIndex)
	{
		return;
	}

	int old_value = m_nSelectedIndex;
	
	m_nSelectedIndex = value;
	this->ShowSelected();
	
	if (value >= 0)
	{
		if (!m_bUpdating)
		{
			if (this->GetEnabled())
			{
				frame_process_event(this, 0, "select",
					FrameArgList() << value, NULL);
			}
		}

		FlatWidget* pDelegator = this->GetDelegator();

		if (pDelegator)
		{
			pDelegator->OnPartSelect(this, value, old_value);
		}
	}
}

int FlatListBox::GetSelectedIndex() const
{
	return m_nSelectedIndex;
}

void FlatListBox::SetScrollSize(int value)
{
	if (value < 0)
	{
		return;
	}

	flat_scroll_set_bar_size(this->GetScroll(), value);
	this->PerformLayout();
}

int FlatListBox::GetScrollSize() const
{
	return flat_scroll_get_bar_size(this->GetScroll());
}

void FlatListBox::SetDynamicSlide(bool value)
{
	flat_scroll_set_dynamic_slide(this->GetScroll(), value);
}

bool FlatListBox::GetDynamicSlide() const
{
	return flat_scroll_get_dynamic_slide(this->GetScroll());
}

void FlatListBox::SetVScrollAlways(bool value)
{
	flat_scroll_set_vscroll_always(this->GetScroll(), value);
}

bool FlatListBox::GetVScrollAlways() const
{
	return flat_scroll_get_vscroll_always(this->GetScroll());
}

void FlatListBox::SetHScrollExists(bool value)
{
	flat_scroll_set_hscroll_exists(this->GetScroll(), this->GetFlatUI(), value);
}

bool FlatListBox::GetHScrollExists() const
{
	return flat_scroll_get_hscroll_exists(this->GetScroll());
}

void FlatListBox::SetSorted(bool value)
{
	m_bSorted = value;
}

bool FlatListBox::GetSorted() const
{
	return m_bSorted;
}

void FlatListBox::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatListBox::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}

void FlatListBox::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatListBox::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatListBox::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatListBox::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

//void FlatListBox::SetScrollStyle(const char* value)
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

//const char* FlatListBox::GetScrollStyle() const
//{
//	return m_sScrollStyle.CString();
//}

void FlatListBox::SetSelectedText(const char* text)
{
	Assert(text != NULL);

	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		if (strcmp(m_ListItems[i]->sText.CString(), text) == 0)
		{
			m_nSelectedIndex = (int)i;
			return;
		}
	}
}

const char* FlatListBox::GetSelectedText() const
{
	if ((m_nSelectedIndex < 0) || (m_nSelectedIndex >= m_ListItems.Size()))
	{
		return "";
	}

	return m_ListItems[m_nSelectedIndex]->sText.CString();
}

uniqid_t FlatListBox::GetVScrollBarID() const
{
	return flat_scroll_vscroll_id(this->GetScroll());
}

uniqid_t FlatListBox::GetHScrollBarID() const
{
	return flat_scroll_hscroll_id(this->GetScroll());
}

uniqid_t FlatListBox::GetCrossLabelID() const
{
	return flat_scroll_cross_label_id(this->GetScroll());
}

int FlatListBox::AddText(const char* text)
{
	Assert(text != NULL);

	int width = this->GetFlatUI()->GetTextWidth(this->GetAvailableFont(),
		text, 0, 0);
	list_item_t* pItem = K_NEW(list_item_t);

	pItem->sText = text;
	pItem->nWidth = width;

	int index;
	
	if (m_bSorted)
	{
		index = this->AddSortedItem(pItem, 0, (int)m_ListItems.Size());
	}
	else
	{
		index = (int)m_ListItems.Size();
		m_ListItems.PushBack(pItem);
	}

	if (width > m_nMaxItemWidth)
	{
		m_nMaxItemWidth = width;
	}
	
	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	return index;
}

bool FlatListBox::RemoveText(const char* text)
{
	Assert(text != NULL);

	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		if (strcmp(m_ListItems[i]->sText.CString(), text) == 0)
		{
			m_ListItems.Remove(i);
			this->UpdateMaxItemWidth();
			
			if (!m_bUpdating)
			{
				this->UpdateScroll();
			}

			if (m_nSelectedIndex >= m_ListItems.Size())
			{
				this->SetSelectedIndex(-1);
			}

			return true;
		}
	}

	return false;
}

bool FlatListBox::RemoveByIndex(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return false;
	}
	
	m_ListItems.Remove(index);
	this->UpdateMaxItemWidth();

	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	if (m_nSelectedIndex >= m_ListItems.Size())
	{
		this->SetSelectedIndex(-1);
	}

	return true;
}

bool FlatListBox::ClearTexts()
{
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		K_DELETE(m_ListItems[i]);
	}

	m_ListItems.Clear();
	m_nMaxItemWidth = 0;
	
	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	this->SetSelectedIndex(-1);
	return true;
}

int FlatListBox::FindText(const char* text)
{
	Assert(text != NULL);

	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		if (strcmp(m_ListItems[i]->sText.CString(), text) == 0)
		{
			return (int)i;
		}
	}

	return -1;
}

const char* FlatListBox::GetText(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return "";
	}

	return m_ListItems[index]->sText.CString();
}

int FlatListBox::GetTextCount()
{
	return (int)m_ListItems.Size();
}

bool FlatListBox::SetTopIndex(int item_index)
{
	if ((item_index < 0) || (item_index >= m_ListItems.Size()))
	{
		return true;
	}

	int y1 = item_index * m_nItemHeight;

	if (y1 < m_nVerticalBegin)
	{
		this->SetVerticalBegin(y1);
		flat_scroll_adjust_vscroll(this->GetScroll());
	}
	
	return true;
}

bool FlatListBox::BeginUpdate()
{
	m_bUpdating = true;
	return true;
}

bool FlatListBox::EndUpdate()
{
	m_bUpdating = false;
	this->UpdateScroll();
	return true;
}

int FlatListBox::AddSortedItem(list_item_t* pItem, int head, int tail)
{
	Assert(pItem != NULL);
	Assert((size_t)head <= m_ListItems.Size());
	Assert((size_t)tail <= m_ListItems.Size());

	if (head == tail)
	{
		m_ListItems.Insert(head, pItem);
		return head;
	}

	int index = head + (tail - head) / 2;
	list_item_t* p = m_ListItems[index];

	if (stricmp(p->sText.CString(), pItem->sText.CString()) >= 0)
	{
		return this->AddSortedItem(pItem, head, index);
	}
	else
	{
		return this->AddSortedItem(pItem, index + 1, tail);
	}
}

void FlatListBox::UpdateMaxItemWidth()
{
	int max_width = 0;
	
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		if (m_ListItems[i]->nWidth > max_width)
		{
			max_width = m_ListItems[i]->nWidth;
		}
	}

	m_nMaxItemWidth = max_width;
}

void FlatListBox::ShowSelected()
{
	if (m_nSelectedIndex < 0)
	{
		return;
	}

	int reveal_height = flat_scroll_reveal_height(this->GetScroll());
	int y1 = m_nItemHeight * m_nSelectedIndex;

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

void FlatListBox::PerformLayout()
{
	flat_scroll_perform_layout(this->GetScroll());
}

void FlatListBox::UpdateScroll()
{
	flat_scroll_update(this->GetScroll());
}
