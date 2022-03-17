// flat_v_scroll_bar.cpp
// Created by lulimin on 2020/9/11.

#include "flat_v_scroll_bar.h"
#include "flat_ui.h"
#include "flat_button.h"

/// \file flat_v_scroll_bar.cpp
/// \brief User interface vertical scroll bar.

/// object: FlatVScrollBar
/// \brief User interface vertical scroll bar object.
DEF_OBJECT(FlatVScrollBar, FlatWidget);

/// readonly: object UpButton
/// \brief Up button.
DEF_READ_F(uniqid_t, FlatVScrollBar, UpButton, GetUpButtonID);
/// readonly: object DownButton
/// \brief Down button.
DEF_READ_F(uniqid_t, FlatVScrollBar, DownButton, GetDownButtonID);
/// readonly: object SlideButton
/// \brief Slide button.
DEF_READ_F(uniqid_t, FlatVScrollBar, SlideButton, GetSlideButtonID);

/// property: int Minimum
/// \brief Minimum value.
DEF_PROP(int, FlatVScrollBar, Minimum);
/// property: int Maximum
/// \brief Maximum value.
DEF_PROP(int, FlatVScrollBar, Maximum);
/// property: int Value
/// \brief Current value.
DEF_PROP(int, FlatVScrollBar, Value);
/// property: int SmallChange
/// \brief Small change step.
DEF_PROP(int, FlatVScrollBar, SmallChange);
/// property: int LargeChange
/// \brief Large change step.
DEF_PROP(int, FlatVScrollBar, LargeChange);
/// property: int StepButtonSize
/// \brief Step button size.
DEF_PROP(int, FlatVScrollBar, StepButtonSize);
/// property: int SlideButtonSize
/// \brief Slide button size.
DEF_PROP(int, FlatVScrollBar, SlideButtonSize);

// FlatVScrollBar

FlatVScrollBar::FlatVScrollBar()
{
	m_pUpButton = NULL;
	m_pDownButton = NULL;
	m_pSlideButton = NULL;
	m_nMinimum = 0;
	m_nMaximum = 100;
	m_nValue = 0;
	m_nSmallChange = 1;
	m_nLargeChange = 10;
	m_nStepButtonSize = 32;
	m_nSlideButtonSize = 32;
}

FlatVScrollBar::~FlatVScrollBar()
{
}

bool FlatVScrollBar::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->CreateParts();
	
	FlatUI* pFlatUI = this->GetFlatUI();

	m_pUpButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pUpButton->SetArrowType(FlatButton::ARROW_UP);
	m_pDownButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pDownButton->SetArrowType(FlatButton::ARROW_DOWN);
	m_pSlideButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pUpButton->SetPushClick(true);
	m_pUpButton->SetHoverClick(true);
	m_pDownButton->SetPushClick(true);
	m_pDownButton->SetHoverClick(true);
	this->SetSize(32, 200);
	return true;
}

bool FlatVScrollBar::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatVScrollBar::Draw(flat_canvas_t* pCanvas)
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
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}

	m_pUpButton->Draw(pCanvas);
	m_pDownButton->Draw(pCanvas);
	m_pSlideButton->Draw(pCanvas);
}

void FlatVScrollBar::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_maximum(DT_INT32, 100);
	FrameArgData def_small_change(DT_INT32, 1);
	FrameArgData def_large_change(DT_INT32, 10);

	flat_metadata_add_prop(pMD, "Minimum", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "Maximum", FLAT_METATYPE_INT32,
		&def_maximum);
	flat_metadata_add_prop(pMD, "Value", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "SmallChange", FLAT_METATYPE_INT32,
		&def_small_change);
	flat_metadata_add_prop(pMD, "LargeChange", FLAT_METATYPE_INT32,
		&def_large_change);
	flat_metadata_add_prop(pMD, "StepButtonSize", FLAT_METATYPE_INT32,
		NULL);
	flat_metadata_add_prop(pMD, "SlideButtonSize", FLAT_METATYPE_INT32,
		NULL);
	flat_metadata_part_prop(pMD, "BackImage");
	flat_metadata_part_prop(pMD, "ImageLayout");
	flat_metadata_add_event(pMD, "changed");
	flat_metadata_add_part(pMD, "UpButton", "FlatButton");
	flat_metadata_add_part(pMD, "DownButton", "FlatButton");
	flat_metadata_add_part(pMD, "SlideButton", "FlatButton");
}

int FlatVScrollBar::OnLeftDown(int x, int y, unsigned int flags)
{
	int slide_y1 = m_pSlideButton->GetGlobalTop();
	int slide_y2 = slide_y1 + m_pSlideButton->GetHeight();

	if (y < slide_y1)
	{
		this->IncreaseValue(-m_nLargeChange);
	}
	else if (y > slide_y2)
	{
		this->IncreaseValue(m_nLargeChange);
	}

	return 0;
}

int FlatVScrollBar::OnSizeChanged()
{
	this->PerformLayout();
	return 1;
}

int FlatVScrollBar::OnPartClick(FlatWidget* pPart, int x, int y)
{
	Assert(pPart != NULL);

	if (pPart == m_pUpButton)
	{
		this->IncreaseValue(-m_nSmallChange);
	}
	else if (pPart == m_pDownButton)
	{
		this->IncreaseValue(m_nSmallChange);
	}

	return 1;
}

int FlatVScrollBar::OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
	unsigned int flags)
{
	Assert(pPart != NULL);

	if (pPart != m_pSlideButton)
	{
		return 0;
	}

	int up_button_size = m_pUpButton->GetHeight();
	int slide_button_size = m_pSlideButton->GetHeight();
	int beg_y = up_button_size;
	int end_y = this->GetHeight() - up_button_size - slide_button_size;
	int slide_y = y - this->GetGlobalTop() - cy;

	if (slide_y < beg_y)
	{
		slide_y = beg_y;
	}

	if (slide_y > end_y)
	{
		slide_y = end_y;
	}

	m_pSlideButton->SetTop(slide_y);

	if (m_nMaximum > m_nMinimum)
	{
		int stride = m_nMaximum - m_nMinimum;
		double rate = (double)(slide_y - beg_y) / (double)(end_y - beg_y);
		int new_value = m_nMinimum + (int)(rate * (double)stride);
		int old_value = m_nValue;

		if (new_value != old_value)
		{
			m_nValue = new_value;
			this->ValueChanged(old_value);
		}
	}

	return 1;
}

void FlatVScrollBar::SetMinimum(int value)
{
	if (value == m_nMinimum)
	{
		return;
	}

	m_nMinimum = value;
	this->PerformLayout();
}

int FlatVScrollBar::GetMinimum() const
{
	return m_nMinimum;
}

void FlatVScrollBar::SetMaximum(int value)
{
	if (value == m_nMaximum)
	{
		return;
	}

	m_nMaximum = value;
	this->PerformLayout();
}

int FlatVScrollBar::GetMaximum() const
{
	return m_nMaximum;
}

void FlatVScrollBar::SetValue(int value)
{
	int old_value = m_nValue;

	if (value == old_value)
	{
		return;
	}

	if (value < m_nMinimum)
	{
		return;
	}

	if (value > m_nMaximum)
	{
		return;
	}

	m_nValue = value;
	this->LocateSlideButton();
	this->ValueChanged(old_value);
}

int FlatVScrollBar::GetValue() const
{
	return m_nValue;
}

void FlatVScrollBar::SetSmallChange(int value)
{
	m_nSmallChange = value;
}

int FlatVScrollBar::GetSmallChange() const
{
	return m_nSmallChange;
}

void FlatVScrollBar::SetLargeChange(int value)
{
	m_nLargeChange = value;
}

int FlatVScrollBar::GetLargeChange() const
{
	return m_nLargeChange;
}

void FlatVScrollBar::SetStepButtonSize(int value)
{
	m_nStepButtonSize = value;
}

int FlatVScrollBar::GetStepButtonSize() const
{
	return m_nStepButtonSize;
}

void FlatVScrollBar::SetSlideButtonSize(int value)
{
	m_nSlideButtonSize = value;
}

int FlatVScrollBar::GetSlideButtonSize() const
{
	return m_nSlideButtonSize;
}

uniqid_t FlatVScrollBar::GetUpButtonID() const\
{
	return m_pUpButton->GetUID();
}

uniqid_t FlatVScrollBar::GetDownButtonID() const
{
	return m_pDownButton->GetUID();
}

uniqid_t FlatVScrollBar::GetSlideButtonID() const
{
	return m_pSlideButton->GetUID();
}

bool FlatVScrollBar::AdjustValue(int new_value)
{
	int old_value = m_nValue;

	if (new_value == old_value)
	{
		return false;
	}

	if (new_value < m_nMinimum)
	{
		return false;
	}

	if (new_value > m_nMaximum)
	{
		return false;
	}

	m_nValue = new_value;
	this->LocateSlideButton();
	return true;
}

bool FlatVScrollBar::AdjustSlideSize(float rate, int min_size)
{
	Assert((rate >= 0.0F) && (rate <= 1.0F));
	Assert(min_size > 0);

	int h = this->GetHeight() - m_pUpButton->GetHeight() - 
		m_pDownButton->GetHeight();
	int slide_size = (int)((float)h * rate);

	if (slide_size < min_size)
	{
		slide_size = min_size;
	}

	if (slide_size > h)
	{
		slide_size = h;
	}
	
	m_pSlideButton->SetHeight(slide_size);
	return true;
}

bool FlatVScrollBar::IncreaseValue(int delta)
{
	int new_value = m_nValue + delta;

	if (new_value < m_nMinimum)
	{
		new_value = m_nMinimum;
	}

	if (new_value > m_nMaximum)
	{
		new_value = m_nMaximum;
	}

	this->SetValue(new_value);
	return true;
}

void FlatVScrollBar::PerformLayout()
{
	if (m_nStepButtonSize > 0)
	{
		m_pUpButton->SetVisible(true);
		m_pDownButton->SetVisible(true);
	}
	else
	{
		m_pUpButton->SetVisible(false);
		m_pDownButton->SetVisible(false);
	}

	if ((m_nMaximum - m_nMinimum) > 0)
	{
		m_pSlideButton->SetVisible(true);
	}
	else
	{
		m_pSlideButton->SetVisible(false);
	}

	m_pUpButton->SetLeft(0);
	m_pUpButton->SetTop(0);
	m_pUpButton->SetSize(this->GetWidth(), m_nStepButtonSize);
	m_pDownButton->SetLeft(0);
	m_pDownButton->SetTop(this->GetHeight() - m_nStepButtonSize);
	m_pDownButton->SetSize(this->GetWidth(), m_nStepButtonSize);
	m_pSlideButton->SetLeft(0);
	m_pSlideButton->SetTop(m_nStepButtonSize);
	m_pSlideButton->SetSize(this->GetWidth(), m_nSlideButtonSize);
	this->LocateSlideButton();
}

bool FlatVScrollBar::LocateSlideButton()
{
	int stride = m_nMaximum - m_nMinimum;

	if (stride <= 0)
	{
		return false;
	}

	double rate = (double)m_nValue / (double)stride;
	int up_button_size = m_pUpButton->GetHeight();
	int slide_button_size = m_pSlideButton->GetHeight();
	int beg_y = up_button_size;
	int end_y = this->GetHeight() - up_button_size - slide_button_size;
	int height = end_y - beg_y;

	if (height <= 0)
	{
		return false;
	}

	int slide_y = beg_y + (int)(rate * (double)height);

	if (slide_y < beg_y)
	{
		slide_y = beg_y;
	}

	if (slide_y > end_y)
	{
		slide_y = end_y;
	}

	m_pSlideButton->SetTop(slide_y);
	return true;
}

void FlatVScrollBar::ValueChanged(int old_value)
{
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList() << old_value,
			NULL);
	}

	FlatWidget* pDelegator = this->GetDelegator();

	if (pDelegator)
	{
		pDelegator->OnPartSelect(this, m_nValue, old_value);
	}
}
