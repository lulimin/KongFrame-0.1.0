// flat_panel.cpp
// Created by lulimin on 2020/9/11.

#include "flat_panel.h"
#include "flat_group_box.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_panel.cpp
/// \brief User interface panel.

/// object: FlatPanel
/// \brief User interface panel object.
DEF_OBJECT(FlatPanel, FlatWidget);

/// readonly: object VScrollBar
/// \brief Vertical scroll bar.
DEF_READ_F(uniqid_t, FlatPanel, VScrollBar, GetVScrollBarID);
/// readonly: object HScrollBar
/// \brief Horizontal scroll bar.
DEF_READ_F(uniqid_t, FlatPanel, HScrollBar, GetHScrollBarID);
/// readonly: object CrossLabel
/// \brief Cross label.
DEF_READ_F(uniqid_t, FlatPanel, CrossLabel, GetCrossLabelID);
/// readonly: int ContentWidth
/// \brief Width of content.
DEF_READ(int, FlatPanel, ContentWidth);
/// readonly: int ContentHeight
/// \brief Height of content.
DEF_READ(int, FlatPanel, ContentHeight);

/// property: string Text
/// \brief Text of title.
DEF_PROP(const char*, FlatPanel, Text);
/// property: int ScrollSize
/// \brief Scroll bar size.
DEF_PROP(int, FlatPanel, ScrollSize);
/// property: bool DynamicSlide
/// \brief Dynamic scroll slide bar size.
DEF_PROP(bool, FlatPanel, DynamicSlide);
/// property: string ScrollStyle
/// \brief Preset scroll style.
//DEF_PROP(const char*, FlatPanel, ScrollStyle);

/// function: bool SetContentSize(int width, int height)
/// \brief Set content size.
/// \param width,height Size value.
DEF_FUNC_2(bool, FlatPanel, SetContentSize, int, int);
/// function: bool AddChild(object id)
/// \brief Add child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatPanel, AddChild, const uniqid_t&);
/// function: bool RemoveChild(object id)
/// \brief Remove child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatPanel, RemoveChild, const uniqid_t&);
/// function: bool ClearChildren()
/// \brief Clear child widgets.
DEF_FUNC_0(bool, FlatPanel, ClearChildren);
/// function [children table] GetChildList()
/// \brief Get children widget id list.
DEF_FUNC_A(FlatPanel, GetChildList);

// FlatPanel

FlatPanel::FlatPanel()
{
	m_nVerticalBegin = 0;
	m_nHorizontalBegin = 0;
	m_pGroupBox = NULL;
}

FlatPanel::~FlatPanel()
{
}

bool FlatPanel::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();
	this->CreateScroll();

	FlatUI* pFlatUI = this->GetFlatUI();

	// Group box must in background.
	m_pGroupBox = (FlatGroupBox*)pFlatUI->CreatePartWidget(this,
		"FlatGroupBox");
	flat_scroll_initialize(this->GetScroll(), pFlatUI, true, true);
	return true;
}

bool FlatPanel::Shutdown()
{
	return FlatWidget::Shutdown();
}

bool FlatPanel::HasChildren()
{
	return m_pGroupBox->HasChildren();
}

void FlatPanel::Draw(flat_canvas_t* pCanvas)
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

	if (this->ExistBackImage())
	{
		this->DrawBackImage(pCanvas, x1, y1, x2, y2);
	}
	else
	{
		canvas_draw_block(pCanvas, x1, y1, x2, y2, this->GetBackColorValue());
	}

	flat_scroll_draw(this->GetScroll(), pCanvas);

	if (!this->ExistBackImage())
	{
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}
}

void FlatPanel::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_empty_string(DT_STRING, "");
	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_false(DT_BOOLEAN, false);

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "ScrollSize", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DynamicSlide", FLAT_METATYPE_BOOLEAN,
		&def_false);
	//flat_metadata_add_prop(pMD, "ScrollStyle", FLAT_METATYPE_STRING,
	//	&def_empty_string);
	flat_metadata_add_part(pMD, "VScrollBar", "FlatVScrollBar");
	flat_metadata_add_part(pMD, "HScrollBar", "FlatHScrollBar");
	flat_metadata_add_part(pMD, "CrossLabel", "FlatLabel");
}

void FlatPanel::DrawScrollRegion(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	m_pGroupBox->Draw(pCanvas);
}

int FlatPanel::GetVerticalWhole()
{
	return m_pGroupBox->GetHeight();
}

int FlatPanel::GetVerticalReveal()
{
	return flat_scroll_reveal_height(this->GetScroll());
}

int FlatPanel::GetVerticalBegin()
{
	//return 0 - m_pGroupBox->GetTop();
	return m_nVerticalBegin;
}

void FlatPanel::SetVerticalBegin(int value)
{
	m_nVerticalBegin = value;
	m_pGroupBox->SetTop(0 - value);
}

int FlatPanel::GetHorizontalWhole()
{
	return m_pGroupBox->GetWidth();
}

int FlatPanel::GetHorizontalReveal()
{
	return flat_scroll_reveal_width(this->GetScroll());
}

int FlatPanel::GetHorizontalBegin()
{
	//return 0 - m_pGroupBox->GetLeft();
	return m_nHorizontalBegin;
}

void FlatPanel::SetHorizontalBegin(int value)
{
	m_nHorizontalBegin = value;
	m_pGroupBox->SetLeft(0 - value);
}

int FlatPanel::OnMouseWheel(int x, int y, double delta, unsigned int flags)
{
	if (flags & IInputService::FLAG_CONTROL)
	{
		int reveal_width = this->GetHorizontalReveal();
		int whole_width = this->GetHorizontalWhole();
		
		if (reveal_width >= whole_width)
		{
			return 0;
		}

		int new_value = m_nHorizontalBegin + (int)((-delta) * 10.0);

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

		int new_value = m_nVerticalBegin + (int)((-delta) * 10.0);

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

int FlatPanel::OnSizeChanged()
{
	this->PerformLayout();
	this->UpdateScroll();
	return 1;
}

int FlatPanel::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	flat_scroll_changed(this->GetScroll(), pPart, new_value, old_value);
	return 1;
}

void FlatPanel::SetText(const char* value)
{
	Assert(value != NULL);

	m_pGroupBox->SetText(value);
}

const char* FlatPanel::GetText() const
{
	return m_pGroupBox->GetText();
}

void FlatPanel::SetScrollSize(int value)
{
	if (value < 0)
	{
		return;
	}
	
	flat_scroll_set_bar_size(this->GetScroll(), value);
	this->PerformLayout();
}

int FlatPanel::GetScrollSize() const
{
	return flat_scroll_get_bar_size(this->GetScroll());
}

void FlatPanel::SetDynamicSlide(bool value)
{
	flat_scroll_set_dynamic_slide(this->GetScroll(), value);
}

bool FlatPanel::GetDynamicSlide() const
{
	return flat_scroll_get_dynamic_slide(this->GetScroll());
}

//void FlatPanel::SetScrollStyle(const char* value)
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

//const char* FlatPanel::GetScrollStyle() const
//{
//	return m_sScrollStyle.CString();
//}

uniqid_t FlatPanel::GetVScrollBarID() const
{
	return flat_scroll_vscroll_id(this->GetScroll());
}

uniqid_t FlatPanel::GetHScrollBarID() const
{
	return flat_scroll_hscroll_id(this->GetScroll());
}

uniqid_t FlatPanel::GetCrossLabelID() const
{
	return flat_scroll_cross_label_id(this->GetScroll());
}

bool FlatPanel::SetContentSize(int width, int height)
{
	m_pGroupBox->SetSize(width, height);
	this->UpdateScroll();
	return true;
}

int FlatPanel::GetContentWidth()
{
	return m_pGroupBox->GetWidth();
}

int FlatPanel::GetContentHeight()
{
	return m_pGroupBox->GetHeight();
}

bool FlatPanel::AddChild(const uniqid_t& id)
{
	return m_pGroupBox->AddChild(id);
}

bool FlatPanel::RemoveChild(const uniqid_t& id)
{
	return m_pGroupBox->RemoveChild(id);
}

bool FlatPanel::ClearChildren()
{
	return m_pGroupBox->ClearChildren();
}

void FlatPanel::GetChildList(const IArgList& args, IArgList* res)
{
	return m_pGroupBox->GetChildList(args, res);
}

void FlatPanel::PerformLayout()
{
	flat_scroll_perform_layout(this->GetScroll());
}

void FlatPanel::UpdateScroll()
{
	flat_scroll_update(this->GetScroll());
}
