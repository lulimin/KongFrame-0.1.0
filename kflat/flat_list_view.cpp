// flat_list_view.cpp
// Created by lulimin on 2020/9/11.

#include "flat_list_view.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_list_view.cpp
/// \brief User interface list view.

/// object: FlatListView
/// \brief User interface list view object.
DEF_OBJECT(FlatListView, FlatWidget);

/// readonly: object VScrollBar
/// \brief Vertical scroll bar.
DEF_READ_F(uniqid_t, FlatListView, VScrollBar, GetVScrollBarID);
/// readonly: object HScrollBar
/// \brief Horizontal scroll bar.
DEF_READ_F(uniqid_t, FlatListView, HScrollBar, GetHScrollBarID);
/// readonly: object CrossLabel
/// \brief Cross label.
DEF_READ_F(uniqid_t, FlatListView, CrossLabel, GetCrossLabelID);

/// property: int ItemWidth
/// \brief Width of list item.
DEF_PROP(int, FlatListView, ItemWidth);
/// property: int ItemHeight
/// \brief Height of list item.
DEF_PROP(int, FlatListView, ItemHeight);
/// property: int GlyphWidth
/// \brief Width of glyph.
DEF_PROP(int, FlatListView, GlyphWidth);
/// property: int GlyphHeight
/// \brief Height of glyph.
DEF_PROP(int, FlatListView, GlyphHeight);
/// property: int SelectedIndex
/// \brief Index of selected item.
DEF_PROP(int, FlatListView, SelectedIndex);
/// property: int ScrollSize
/// \brief Scroll bar size.
DEF_PROP(int, FlatListView, ScrollSize);
/// property: bool DynamicSlide
/// \brief Dynamic scroll slide bar size.
DEF_PROP(bool, FlatListView, DynamicSlide);
/// property: bool VScrollAlways
/// \brief Always show vertical scroll bar.
DEF_PROP(bool, FlatListView, VScrollAlways);
/// property: bool HScrollExists
/// \brief Show horizontal scroll bar.
DEF_PROP(bool, FlatListView, HScrollExists);
/// property: bool Sorted
/// \brief Sort text item.
DEF_PROP(bool, FlatListView, Sorted);
/// property: bool RightText
/// \brief Text in right side.
DEF_PROP(bool, FlatListView, RightText);
/// property: bool DragEnable
/// \brief Enable draging.
DEF_PROP(bool, FlatListView, DragEnable);
/// property: string SelectForeColor
/// \brief Fore color of selected item.
DEF_PROP(ArgString, FlatListView, SelectForeColor);
/// property: string SelectBackColor
/// \brief Background color of selected item.
DEF_PROP(ArgString, FlatListView, SelectBackColor);
/// property: string CaptureColor
/// \brief Background color of captured item.
DEF_PROP(ArgString, FlatListView, CaptureColor);
/// property: string ScrollStyle
/// \brief Preset scroll style.
//DEF_PROP(const char*, FlatListView, ScrollStyle);

/// function: int AddItem(string text, string glyph)
/// \brief Add list item.
/// \param text Text of item.
/// \param glyph Glyph of item.
DEF_FUNC_2(int, FlatListView, AddItem, const char*, const char*);
/// function: int AddWidgetItem(string text, object id)
/// \brief Add widget item.
/// \param text Text of item.
/// \param id Widget object id.
DEF_FUNC_2(int, FlatListView, AddWidgetItem, const char*, const uniqid_t&);
/// function: bool RemoveItem(string text)
/// \brief Remove list item.
/// \param text Text value.
DEF_FUNC_1(bool, FlatListView, RemoveItem, const char*);
/// function: bool RemoveByIndex(int index)
/// \brief Remove text item by index.
/// \param index Item index.
DEF_FUNC_1(bool, FlatListView, RemoveByIndex, int);
/// function: bool ClearItems()
/// \brief Clear list items.
DEF_FUNC_0(bool, FlatListView, ClearItems);
/// function: int FindItem(string text)
/// \brief Find list item.
/// \param text Text value.
DEF_FUNC_1(int, FlatListView, FindItem, const char*);
/// function: bool SetTag(int index, string tag)
/// \brief Set tag of item.
/// \param index Item index.
/// \param tag Tag value.
DEF_FUNC_2(int, FlatListView, SetTag, int, const char*);
/// function: string GetTag(int index)
/// \brief Get tag of item.
/// \param index Item index.
DEF_FUNC_1(const char*, FlatListView, GetTag, int);
/// function: string GetText(int index)
/// \brief Get item text.
/// \param index Item index.
DEF_FUNC_1(const char*, FlatListView, GetText, int);
/// function: string GetGlyph(int index)
/// \brief Get item text.
/// \param index Item index.
DEF_FUNC_1(const char*, FlatListView, GetGlyph, int);
/// function: object GetWidget(int index)
/// \brief Get item widget.
/// \param index Item index.
DEF_FUNC_1(uniqid_t, FlatListView, GetWidget, int);
/// function: int GetItemCount()
/// \brief Get number of list item.
DEF_FUNC_0(int, FlatListView, GetItemCount);

/// function: bool BeginUdpate()
/// \brief Begin update.
DEF_FUNC_0(bool, FlatListView, BeginUpdate);
/// function: bool EndUpdate()
/// \brief End update.
DEF_FUNC_0(bool, FlatListView, EndUpdate);

#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000
#define INIT_CAPTURE_COLOR 0xFF800000

// FlatListView

FlatListView::FlatListView()
{
	m_nItemWidth = 64;
	m_nItemHeight = 64;
	m_nGlyphWidth = 32;
	m_nGlyphHeight = 32;
	m_nCaptureIndex = -1;
	m_nSelectedIndex = -1;
	m_nVerticalBegin = 0;
	m_nHorizontalBegin = 0;
	m_bSorted = false;
	m_bRightText = false;
	m_bDragEnable = false;
	m_bDraging = false;
	m_bUpdating = false;
	m_nSelectForeColor = INIT_SELECT_FORE_COLOR;
	m_nSelectBackColor = INIT_SELECT_BACK_COLOR;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
}

FlatListView::~FlatListView()
{
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		list_item_t* pItem = m_ListItems[i];

		if (pItem->pGlyphImage)
		{
			flat_image_delete(pItem->pGlyphImage);
		}

		K_DELETE(m_ListItems[i]);
	}
}

bool FlatListView::Startup(const IArgList& args)
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

bool FlatListView::Shutdown()
{
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		list_item_t* pItem = m_ListItems[i];

		if (uniqid_not_null(pItem->WidgetID))
		{
			FlatWidget* pWidget = flat_get_widget(pItem->WidgetID);

			if (pWidget)
			{
				pWidget->Release();
			}
		}
	}
	
	return FlatWidget::Shutdown();
}

void FlatListView::Draw(flat_canvas_t* pCanvas)
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

void FlatListView::SetMetadata(flat_metadata_t* pMD)
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

	flat_metadata_add_prop(pMD, "ItemWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "ItemHeight", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "GlyphWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "GlyphHeight", FLAT_METATYPE_INT32,
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
	flat_metadata_add_prop(pMD, "RightText", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "DragEnable", FLAT_METATYPE_BOOLEAN,
		&def_false);
	//flat_metadata_add_prop(pMD, "ScrollStyle", FLAT_METATYPE_STRING,
	//	&def_empty_string);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_event(pMD, "click");
	flat_metadata_add_part(pMD, "VScrollBar", "FlatVScrollBar");
	flat_metadata_add_part(pMD, "HScrollBar", "FlatHScrollBar");
	flat_metadata_add_part(pMD, "CrossLabel", "FlatLabel");
}

FlatWidget* FlatListView::GetRegionOf(int x, int y)
{
	if (!this->GetVisible())
	{
		return NULL;
	}

	if (!this->InSelfRegion(x, y))
	{
		// Not in region of widget.
		return NULL;
	}

	if (this->GetDesignMode())
	{
		// In design status.
		return this;
	}

	int index = this->GetInItemIndex(x, y);

	if ((size_t)index < m_ListItems.Size())
	{
		list_item_t* pItem = m_ListItems[index];

		if (uniqid_not_null(pItem->WidgetID))
		{
			FlatWidget* pItemWidget = flat_get_widget(pItem->WidgetID);

			if (pItemWidget)
			{
				FlatWidget* p = pItemWidget->GetRegionOf(x, y);

				if (p)
				{
					return p;
				}
			}
		}
	}

	FlatWidget* pWidget = this->InPartRegion(x, y);

	if (pWidget)
	{
		return pWidget;
	}

	if (this->OnTestTransparency(x, y))
	{
		// Is transparency part.
		return NULL;
	}

	return this;
}

void FlatListView::DrawScrollRegion(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	int column_num = this->GetItemColumns();
	int row_num = ((int)m_ListItems.Size() + column_num - 1) / column_num;
	int beg = m_nVerticalBegin / m_nItemHeight;
	int x0 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y0 = this->GetGlobalTop() + beg * m_nItemHeight - m_nVerticalBegin;
	int height = this->GetHeight();
	int num = height / m_nItemHeight + 1;
	int y1 = y0;

	this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
	
	for (int r = 0; r < num; ++r)
	{
		int x1 = x0;
		
		for (int c = 0; c < column_num; ++c)
		{
			int index = (r + beg) * column_num + c;

			if (index >= (int)m_ListItems.Size())
			{
				return;
			}

			int x2 = x1 + m_nItemWidth;
			int y2 = y1 + m_nItemHeight;
			list_item_t* pItem = m_ListItems[index];

			if (uniqid_not_null(pItem->WidgetID))
			{
				FlatWidget* pWidget = flat_get_widget(pItem->WidgetID);

				if (pWidget)
				{
					pWidget->SetGlobalLeftTop(x1, y1);
					pWidget->Draw(pCanvas);
					x1 += m_nItemWidth;
					continue;
				}
			}

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

			if (pItem->pGlyphImage)
			{
				int x3;
				int y3;

				if (m_bRightText)
				{
					x3 = x1;
					y3 = y1 + (m_nItemHeight - m_nGlyphHeight) / 2;
				}
				else
				{
					x3 = x1 + (m_nItemWidth - m_nGlyphWidth) / 2;
					y3 = y1;
				}

				int x4 = x3 + m_nGlyphWidth;
				int y4 = y3 + m_nGlyphHeight;

				canvas_draw_image(pCanvas, x3, y3, x4, y4, pItem->pGlyphImage,
					this->GetImageLayoutValue());
			}

			int x5;
			int y5;

			if (m_bRightText)
			{
				x5 = x1 + m_nGlyphWidth;
				y5 = y1;
			}
			else
			{
				x5 = x1;
				y5 = y1 + m_nGlyphHeight;
			}

			int x6 = x1 + m_nItemWidth;
			int y6 = y1 + m_nItemHeight;
			//unsigned int fore_color;
			//
			//if (m_nSelectedIndex == index)
			//{
			//	canvas_draw_block(pCanvas, x5, y5, x6, y6, m_nSelectBackColor);
			//	fore_color = m_nSelectForeColor;
			//}
			//else
			//{
			//	if (this->GetCapture() && (m_nCaptureIndex == index))
			//	{
			//		canvas_draw_block(pCanvas, x5, y5, x6, y6, m_nCaptureColor);
			//	}
			//
			//	fore_color = this->GetForeColorValue();
			//}
			
			if (m_bRightText)
			{
				canvas_draw_text_left(pCanvas, x5, y5, x6, y6, fore_color,
					pItem->sText.CString(), 0, 0);
			}
			else
			{
				canvas_draw_text_center(pCanvas, x5, y5, x6, y6, fore_color,
					pItem->sText.CString(), 0, 0);
			}

			x1 += m_nItemWidth;
		}

		y1 += m_nItemHeight;
	}
}

int FlatListView::GetVerticalWhole()
{
	int column_num = this->GetItemColumns();
	int row_num = ((int)m_ListItems.Size() + column_num - 1) / column_num;

	return row_num * m_nItemHeight;
}

int FlatListView::GetVerticalReveal()
{
	return flat_scroll_reveal_height(this->GetScroll());
}

int FlatListView::GetVerticalBegin()
{
	return m_nVerticalBegin;
}

void FlatListView::SetVerticalBegin(int value)
{
	m_nVerticalBegin = value;
}

int FlatListView::GetHorizontalWhole()
{
	int column_num = this->GetItemColumns();

	return column_num * m_nItemWidth;
}

int FlatListView::GetHorizontalReveal()
{
	return flat_scroll_reveal_width(this->GetScroll());
}

int FlatListView::GetHorizontalBegin()
{
	return m_nHorizontalBegin;
}

void FlatListView::SetHorizontalBegin(int value)
{
	m_nHorizontalBegin = value;
}

int FlatListView::GetInItemIndex(int x, int y)
{
	int column_num = this->GetItemColumns();
	int row_num = ((int)m_ListItems.Size() + column_num - 1) / column_num;
	int beg = m_nVerticalBegin / m_nItemHeight;
	int x0 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y0 = this->GetGlobalTop() + beg * m_nItemHeight - m_nVerticalBegin;
	int height = this->GetHeight();
	int num = height / m_nItemHeight + 1;
	int y1 = y0;

	for (int r = 0; r < num; ++r)
	{
		int x1 = x0;

		for (int c = 0; c < column_num; ++c)
		{
			int index = (r + beg) * column_num + c;

			if (index >= (int)m_ListItems.Size())
			{
				return -1;
			}

			int x2 = x1 + m_nItemWidth;
			int y2 = y1 + m_nItemHeight;
			
			if (flat_in_rectangle(x, y, x1, y1, x2, y2))
			{
				return index;
			}

			x1 += m_nItemWidth;
		}

		y1 += m_nItemHeight;
	}
	
	return -1;
}

int FlatListView::OnClickReset()
{
	if (m_bDragEnable && m_bDraging)
	{
		m_bDraging = false;
		
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "drag_end",
				FrameArgList() << 0 << 0 << uniqid_t(), NULL);
		}
	}

	return 1;
}

int FlatListView::OnMouseMove(int x, int y, unsigned int flags)
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

int FlatListView::OnMouseWheel(int x, int y, double delta, unsigned int flags)
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

int FlatListView::OnLeftDown(int x, int y, unsigned int flags)
{
	int index = this->GetInItemIndex(x, y);

	if (index < 0)
	{
		return 0;
	}

	this->SetSelectedIndex(index);
	return 1;
}

int FlatListView::OnLeftUp(int x, int y, unsigned int flags)
{
	return 1;
}

int FlatListView::OnLeftDual(int x, int y, unsigned int flags)
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

int FlatListView::OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags)
{
	if (m_bDragEnable)
	{
		if (!m_bDraging)
		{
			m_bDraging = true;
			
			if (this->GetEnabled())
			{
				frame_process_event(this, 0, "drag_begin",
					FrameArgList() << x << y, NULL);
			}
		}
		else
		{
			if (this->GetEnabled())
			{
				frame_process_event(this, 0, "drag_move",
					FrameArgList() << x << y, NULL);
			}
		}
	}
	
	return 1;
}

int FlatListView::OnLeftDragUp(int x, int y, int gcx, int gcy, 
	unsigned int flags)
{
	if (m_bDragEnable && m_bDraging)
	{
		uniqid_t target_id;
		FlatWidget* pTopDialog = this->GetFlatUI()->GetTopDialog();

		if (pTopDialog)
		{
			FlatWidget* pWidget = pTopDialog->GetRegionOf(x, y);

			if (pWidget)
			{
				target_id = pWidget->GetUID();
			}
		}

		m_bDraging = false;
		
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "drag_end",
				FrameArgList() << x << y << target_id, NULL);
		}
	}

	return 1;
}

int FlatListView::OnKeyDown(unsigned int key, unsigned int flags)
{
	switch (key)
	{
	case IInputService::KEY_LEFT:
	{
		if (m_nSelectedIndex <= 0)
		{
			break;
		}

		this->SetSelectedIndex(m_nSelectedIndex - 1);
		return 1;
	}
	case IInputService::KEY_RIGHT:
	{
		if ((m_nSelectedIndex + 1) >= (int)m_ListItems.Size())
		{
			break;
		}

		this->SetSelectedIndex(m_nSelectedIndex + 1);
		break;
	}
	case IInputService::KEY_UP:
	{
		if (m_nSelectedIndex <= 0)
		{
			break;
		}

		int column_num = this->GetItemColumns();
		int new_value = m_nSelectedIndex - column_num;

		if (column_num < 0)
		{
			break;
		}

		this->SetSelectedIndex(new_value);
		return 1;
	}
	case IInputService::KEY_DOWN:
	{
		if ((m_nSelectedIndex + 1) >= (int)m_ListItems.Size())
		{
			break;
		}
		
		int column_num = this->GetItemColumns();
		int new_value = m_nSelectedIndex + column_num;

		if (new_value >= (int)m_ListItems.Size())
		{
			break;
		}
		
		this->SetSelectedIndex(new_value);
		break;
	}
	case IInputService::KEY_PRIOR:
	{
		if (m_nSelectedIndex <= 0)
		{
			break;
		}
		
		int reveal_height = flat_scroll_reveal_height(this->GetScroll());
		int page_rows = reveal_height / m_nItemHeight;

		if (page_rows == 0)
		{
			page_rows = 1;
		}

		int column_num = this->GetItemColumns();
		int new_value = m_nSelectedIndex - page_rows * column_num;

		if (new_value < 0)
		{
			new_value = 0;
		}
		
		this->SetSelectedIndex(new_value);
		break;
	}
	case IInputService::KEY_NEXT:
	{
		if ((m_nSelectedIndex + 1) >= (int)m_ListItems.Size())
		{
			break;
		}

		int reveal_height = flat_scroll_reveal_height(this->GetScroll());
		int page_rows = reveal_height / m_nItemHeight;

		if (page_rows == 0)
		{
			page_rows = 1;
		}

		int column_num = this->GetItemColumns();
		int new_value = m_nSelectedIndex + page_rows * column_num;

		if (new_value >= (int)m_ListItems.Size())
		{
			new_value = (int)m_ListItems.Size() - 1;
		}

		this->SetSelectedIndex(new_value);
		break;
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

int FlatListView::OnSizeChanged()
{
	this->PerformLayout();
	this->UpdateScroll();
	return 1;
}

int FlatListView::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	flat_scroll_changed(this->GetScroll(), pPart, new_value, old_value);
	return 1;
}

void FlatListView::SetItemWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nItemWidth = value;
}

int FlatListView::GetItemWidth() const
{
	return m_nItemWidth;
}

void FlatListView::SetItemHeight(int value)
{
	if (value <= 0)
	{
		return;
	}
	
	m_nItemHeight = value;
}

int FlatListView::GetItemHeight() const
{
	return m_nItemHeight;
}

void FlatListView::SetGlyphWidth(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nGlyphWidth = value;
}

int FlatListView::GetGlyphWidth() const
{
	return m_nGlyphWidth;
}

void FlatListView::SetGlyphHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nGlyphHeight = value;
}

int FlatListView::GetGlyphHeight() const
{
	return m_nGlyphHeight;
}

void FlatListView::SetSelectedIndex(int value)
{
	if (value == m_nSelectedIndex)
	{
		return;
	}
	
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
	}
}

int FlatListView::GetSelectedIndex() const
{
	return m_nSelectedIndex;
}

void FlatListView::SetScrollSize(int value)
{
	if (value < 0)
	{
		return;
	}

	flat_scroll_set_bar_size(this->GetScroll(), value);
	this->PerformLayout();
}

int FlatListView::GetScrollSize() const
{
	return flat_scroll_get_bar_size(this->GetScroll());
}

void FlatListView::SetDynamicSlide(bool value)
{
	flat_scroll_set_dynamic_slide(this->GetScroll(), value);
}

bool FlatListView::GetDynamicSlide() const
{
	return flat_scroll_get_dynamic_slide(this->GetScroll());
}

void FlatListView::SetVScrollAlways(bool value)
{
	flat_scroll_set_vscroll_always(this->GetScroll(), value);
}

bool FlatListView::GetVScrollAlways() const
{
	return flat_scroll_get_vscroll_always(this->GetScroll());
}

void FlatListView::SetHScrollExists(bool value)
{
	flat_scroll_set_hscroll_exists(this->GetScroll(), this->GetFlatUI(), value);
}

bool FlatListView::GetHScrollExists() const
{
	return flat_scroll_get_hscroll_exists(this->GetScroll());
}

void FlatListView::SetSorted(bool value)
{
	m_bSorted = value;
}

bool FlatListView::GetSorted() const
{
	return m_bSorted;
}

void FlatListView::SetRightText(bool value)
{
	m_bRightText = value;
}

bool FlatListView::GetRightText() const
{
	return m_bRightText;
}

void FlatListView::SetDragEnable(bool value)
{
	m_bDragEnable = value;
}

bool FlatListView::GetDragEnable() const
{
	return m_bDragEnable;
}

void FlatListView::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatListView::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}

void FlatListView::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatListView::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatListView::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatListView::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

//void FlatListView::SetScrollStyle(const char* value)
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

//const char* FlatListView::GetScrollStyle() const
//{
//	return m_sScrollStyle.CString();
//}

uniqid_t FlatListView::GetVScrollBarID() const
{
	return flat_scroll_vscroll_id(this->GetScroll());
}

uniqid_t FlatListView::GetHScrollBarID() const
{
	return flat_scroll_hscroll_id(this->GetScroll());
}

uniqid_t FlatListView::GetCrossLabelID() const
{
	return flat_scroll_cross_label_id(this->GetScroll());
}

int FlatListView::AddItem(const char* text, const char* glyph)
{
	Assert(text != NULL);
	Assert(glyph != NULL);
	
	list_item_t* pItem = K_NEW(list_item_t);

	pItem->sText = text;
	pItem->sGlyph = glyph;
	pItem->pGlyphImage = NULL;

	if (!util_string_empty(glyph))
	{
		pItem->pGlyphImage = flat_load_image(this->GetFlatUI(),
			pItem->pGlyphImage, glyph, false);
	}

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
	
	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	return index;
}

int FlatListView::AddWidgetItem(const char* text, const uniqid_t& id)
{
	Assert(text != NULL);

	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}

	pWidget->SetSize(m_nItemWidth, m_nItemHeight);

	list_item_t* pItem = K_NEW(list_item_t);

	pItem->sText = text;
	pItem->pGlyphImage = NULL;
	pItem->WidgetID = id;

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

	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	return index;
}

bool FlatListView::RemoveItem(const char* text)
{
	Assert(text != NULL);

	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		list_item_t* pItem = m_ListItems[i];

		if (strcmp(pItem->sText.CString(), text) == 0)
		{
			if (uniqid_not_null(pItem->WidgetID))
			{
				FlatWidget* pWidget = flat_get_widget(pItem->WidgetID);

				if (pWidget)
				{
					pWidget->Release();
				}
			}
			
			m_ListItems.Remove(i);
			
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

bool FlatListView::RemoveByIndex(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return false;
	}

	list_item_t* pItem = m_ListItems[index];

	if (uniqid_not_null(pItem->WidgetID))
	{
		FlatWidget* pWidget = flat_get_widget(pItem->WidgetID);

		if (pWidget)
		{
			pWidget->Release();
		}
	}
	
	m_ListItems.Remove(index);

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

bool FlatListView::ClearItems()
{
	for (size_t i = 0; i < m_ListItems.Size(); ++i)
	{
		list_item_t* pItem = m_ListItems[i];

		if (uniqid_not_null(pItem->WidgetID))
		{
			FlatWidget* pWidget = flat_get_widget(pItem->WidgetID);

			if (pWidget)
			{
				pWidget->Release();
			}
		}
		
		if (pItem->pGlyphImage)
		{
			flat_image_delete(pItem->pGlyphImage);
		}
		
		K_DELETE(m_ListItems[i]);
	}

	m_ListItems.Clear();

	if (!m_bUpdating)
	{
		this->UpdateScroll();
	}

	this->SetSelectedIndex(-1);
	return true;
}

int FlatListView::FindItem(const char* text)
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

bool FlatListView::SetTag(int index, const char* tag)
{
	Assert(tag != NULL);

	if ((size_t)index >= m_ListItems.Size())
	{
		return false;
	}

	m_ListItems[index]->sTag = tag;
	return true;
}

const char* FlatListView::GetTag(int index) const
{
	if ((size_t)index >= m_ListItems.Size())
	{
		return "";
	}

	return m_ListItems[index]->sTag.CString();
}

const char* FlatListView::GetText(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return "";
	}

	return m_ListItems[index]->sText.CString();
}

const char* FlatListView::GetGlyph(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return "";
	}

	return m_ListItems[index]->sGlyph.CString();
}

uniqid_t FlatListView::GetWidget(int index)
{
	if ((index < 0) || (index >= m_ListItems.Size()))
	{
		return uniqid_t();
	}

	return m_ListItems[index]->WidgetID;
}

int FlatListView::GetItemCount()
{
	return (int)m_ListItems.Size();
}

bool FlatListView::BeginUpdate()
{
	m_bUpdating = true;
	return true;
}

bool FlatListView::EndUpdate()
{
	m_bUpdating = false;
	this->UpdateScroll();
	return true;
}

int FlatListView::AddSortedItem(list_item_t* pItem, int head, int tail)
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

int FlatListView::GetItemColumns()
{
	int reveal_width = flat_scroll_reveal_width(this->GetScroll());
	int column_num = reveal_width / m_nItemWidth;

	if (0 == column_num)
	{
		column_num = 1;
	}

	return column_num;
}

void FlatListView::ShowSelected()
{
	if (m_nSelectedIndex < 0)
	{
		return;
	}

	int column_num = this->GetItemColumns();
	int reveal_width = flat_scroll_reveal_width(this->GetScroll());
	int reveal_height = flat_scroll_reveal_height(this->GetScroll());
	int row = m_nSelectedIndex / column_num;
	//int column = m_nSelectedIndex % column_num;
	//int x1 = column * m_nItemWidth;
	int y1 = row * m_nItemHeight;

//	if (x1 < m_nHorizontalBegin)
//	{
//		this->SetHorizontalBegin(x1);
//		flat_scroll_adjust_hscroll(this->GetScroll());
//	}
//	else
//	{
//		int x2 = x1 + m_nItemWidth;
//		int reveal_width = flat_scroll_reveal_width(this->GetScroll());
//
//		if (x2 > (m_nHorizontalBegin + reveal_width))
//		{
//			this->SetHorizontalBegin(x2 - reveal_width);
//			flat_scroll_adjust_hscroll(this->GetScroll());
//		}
//	}
	
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

void FlatListView::PerformLayout()
{
	flat_scroll_perform_layout(this->GetScroll());
}

void FlatListView::UpdateScroll()
{
	flat_scroll_update(this->GetScroll());
}
