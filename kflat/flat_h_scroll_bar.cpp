// flat_h_scroll_bar.cpp
// Created by lulimin on 2020/9/11.

#include "flat_h_scroll_bar.h"
#include "flat_ui.h"
#include "flat_button.h"

/// \file flat_h_scroll_bar.cpp
/// \brief User interface horizontal scroll bar.

/// object: FlatHScrollBar
/// \brief User interface horizontal scroll bar object.
DEF_OBJECT(FlatHScrollBar, FlatWidget);

/// readonly: object LeftButton
/// \brief Up button.
DEF_READ_F(uniqid_t, FlatHScrollBar, LeftButton, GetLeftButtonID);
/// readonly: object RightButton
/// \brief Down button.
DEF_READ_F(uniqid_t, FlatHScrollBar, RightButton, GetRightButtonID);
/// readonly: object SlideButton
/// \brief Slide button.
DEF_READ_F(uniqid_t, FlatHScrollBar, SlideButton, GetSlideButtonID);

/// property: int Minimum
/// \brief Minimum value.
DEF_PROP(int, FlatHScrollBar, Minimum);
/// property: int Maximum
/// \brief Maximum value.
DEF_PROP(int, FlatHScrollBar, Maximum);
/// property: int Value
/// \brief Current value.
DEF_PROP(int, FlatHScrollBar, Value);
/// property: int SmallChange
/// \brief Small change step.
DEF_PROP(int, FlatHScrollBar, SmallChange);
/// property: int LargeChange
/// \brief Large change step.
DEF_PROP(int, FlatHScrollBar, LargeChange);
/// property: int StepButtonSize
/// \brief Step button size.
DEF_PROP(int, FlatHScrollBar, StepButtonSize);
/// property: int SlideButtonSize
/// \brief Slide button size.
DEF_PROP(int, FlatHScrollBar, SlideButtonSize);

// FlatHScrollBar

FlatHScrollBar::FlatHScrollBar()
{
	m_pLeftButton = NULL;
	m_pRightButton = NULL;
	m_pSlideButton = NULL;
	m_nMinimum = 0;
	m_nMaximum = 100;
	m_nValue = 0;
	m_nSmallChange = 1;
	m_nLargeChange = 10;
	m_nStepButtonSize = 32;
	m_nSlideButtonSize = 32;
}

FlatHScrollBar::~FlatHScrollBar()
{
}

bool FlatHScrollBar::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->CreateParts();
	
	FlatUI* pFlatUI = this->GetFlatUI();

	m_pLeftButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pLeftButton->SetArrowType(FlatButton::ARROW_LEFT);
	m_pRightButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pRightButton->SetArrowType(FlatButton::ARROW_RIGHT);
	m_pSlideButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pLeftButton->SetPushClick(true);
	m_pLeftButton->SetHoverClick(true);
	m_pRightButton->SetPushClick(true);
	m_pRightButton->SetHoverClick(true);
	this->SetSize(200, 32);
	return true;
}

bool FlatHScrollBar::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatHScrollBar::Draw(flat_canvas_t* pCanvas)
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

	m_pLeftButton->Draw(pCanvas);
	m_pRightButton->Draw(pCanvas);
	m_pSlideButton->Draw(pCanvas);
}

void FlatHScrollBar::SetMetadata(flat_metadata_t* pMD)
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
	flat_metadata_add_part(pMD, "LeftButton", "FlatButton");
	flat_metadata_add_part(pMD, "RightButton", "FlatButton");
	flat_metadata_add_part(pMD, "SlideButton", "FlatButton");
}

int FlatHScrollBar::OnLeftDown(int x, int y, unsigned int flags)
{
	int slide_x1 = m_pSlideButton->GetGlobalLeft();
	int slide_x2 = slide_x1 + m_pSlideButton->GetWidth();

	if (x < slide_x1)
	{
		this->IncreaseValue(-m_nLargeChange);
	}
	else if (x > slide_x2)
	{
		this->IncreaseValue(m_nLargeChange);
	}
	
	return 0;
}

int FlatHScrollBar::OnSizeChanged()
{
	this->PerformLayout();
	return 1;
}

int FlatHScrollBar::OnPartClick(FlatWidget* pPart, int x, int y)
{
	Assert(pPart != NULL);
	
	if (pPart == m_pLeftButton)
	{
		this->IncreaseValue(-m_nSmallChange);
	}
	else if (pPart == m_pRightButton)
	{
		this->IncreaseValue(m_nSmallChange);
	}
	
	return 1;
}

int FlatHScrollBar::OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
	unsigned int flags)
{
	Assert(pPart != NULL);

	if (pPart != m_pSlideButton)
	{
		return 0;
	}

	int left_button_size = m_pLeftButton->GetWidth();
	int slide_button_size = m_pSlideButton->GetWidth();
	int beg_x = left_button_size;
	int end_x = this->GetWidth() - left_button_size - slide_button_size;
	int slide_x = x - this->GetGlobalLeft() - cx;

	if (slide_x < beg_x)
	{
		slide_x = beg_x;
	}

	if (slide_x > end_x)
	{
		slide_x = end_x;
	}

	m_pSlideButton->SetLeft(slide_x);

	if (m_nMaximum > m_nMinimum)
	{
		int stride = m_nMaximum - m_nMinimum;
		double rate = (double)(slide_x - beg_x) / (double)(end_x - beg_x);
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

void FlatHScrollBar::SetMinimum(int value)
{ 
	if (value == m_nMinimum)
	{
		return;
	}
	
	m_nMinimum = value;
	this->PerformLayout();
}

int FlatHScrollBar::GetMinimum() const
{ 
	return m_nMinimum;
}

void FlatHScrollBar::SetMaximum(int value)
{ 
	if (value == m_nMaximum)
	{
		return;
	}
	
	m_nMaximum = value;
	this->PerformLayout();
}

int FlatHScrollBar::GetMaximum() const
{ 
	return m_nMaximum;
}

void FlatHScrollBar::SetValue(int value)
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

int FlatHScrollBar::GetValue() const
{ 
	return m_nValue;
}

void FlatHScrollBar::SetSmallChange(int value)
{
	m_nSmallChange = value;
}

int FlatHScrollBar::GetSmallChange() const
{
	return m_nSmallChange;
}

void FlatHScrollBar::SetLargeChange(int value)
{
	m_nLargeChange = value;
}

int FlatHScrollBar::GetLargeChange() const
{
	return m_nLargeChange;
}

void FlatHScrollBar::SetStepButtonSize(int value)
{
	if (value == m_nStepButtonSize)
	{
		return;
	}
	
	m_nStepButtonSize = value;
	this->PerformLayout();
}

int FlatHScrollBar::GetStepButtonSize() const
{
	return m_nStepButtonSize;
}

void FlatHScrollBar::SetSlideButtonSize(int value)
{
	if (value == m_nSlideButtonSize)
	{
		return;
	}

	m_nSlideButtonSize = value;
	this->PerformLayout();
}

int FlatHScrollBar::GetSlideButtonSize() const
{
	return m_nSlideButtonSize;
}

uniqid_t FlatHScrollBar::GetLeftButtonID() const\
{
	return m_pLeftButton->GetUID();
}

uniqid_t FlatHScrollBar::GetRightButtonID() const
{
	return m_pRightButton->GetUID();
}

uniqid_t FlatHScrollBar::GetSlideButtonID() const
{
	return m_pSlideButton->GetUID();
}

bool FlatHScrollBar::AdjustValue(int new_value)
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

bool FlatHScrollBar::AdjustSlideSize(float rate, int min_size)
{
	Assert((rate >= 0.0F) && (rate <= 1.0F));
	Assert(min_size > 0);

	int w = this->GetWidth() - m_pLeftButton->GetWidth() -
		m_pRightButton->GetWidth();
	int slide_size = (int)((float)w * rate);

	if (slide_size < min_size)
	{
		slide_size = min_size;
	}

	if (slide_size > w)
	{
		slide_size = w;
	}

	m_pSlideButton->SetWidth(slide_size);
	return true;
}

bool FlatHScrollBar::IncreaseValue(int delta)
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

void FlatHScrollBar::PerformLayout()
{
	if (m_nStepButtonSize > 0)
	{
		m_pLeftButton->SetVisible(true);
		m_pRightButton->SetVisible(true);
	}
	else
	{
		m_pLeftButton->SetVisible(false);
		m_pRightButton->SetVisible(false);
	}

	if ((m_nMaximum - m_nMinimum) > 0)
	{
		m_pSlideButton->SetVisible(true);
	}
	else
	{
		m_pSlideButton->SetVisible(false);
	}
	
	m_pLeftButton->SetLeft(0);
	m_pLeftButton->SetTop(0);
	m_pLeftButton->SetSize(m_nStepButtonSize, this->GetHeight());
	m_pRightButton->SetLeft(this->GetWidth() - m_nStepButtonSize);
	m_pRightButton->SetTop(0);
	m_pRightButton->SetSize(m_nStepButtonSize, this->GetHeight());
	m_pSlideButton->SetLeft(m_nStepButtonSize);
	m_pSlideButton->SetTop(0);
	m_pSlideButton->SetSize(m_nSlideButtonSize, this->GetHeight());
	this->LocateSlideButton();
}

bool FlatHScrollBar::LocateSlideButton()
{
	int stride = m_nMaximum - m_nMinimum;
	
	if (stride <= 0)
	{
		return false;
	}

	double rate = (double)m_nValue / (double)stride;
	int left_button_size = m_pLeftButton->GetWidth();
	int slide_button_size = m_pSlideButton->GetWidth();
	int beg_x = left_button_size;
	int end_x = this->GetWidth() - left_button_size - slide_button_size;
	int width = end_x - beg_x;

	if (width <= 0)
	{
		return false;
	}

	int slide_x = beg_x + (int)(rate * (double)width);

	if (slide_x < beg_x)
	{
		slide_x = beg_x;
	}

	if (slide_x > end_x)
	{
		slide_x = end_x;
	}

	m_pSlideButton->SetLeft(slide_x);
	return true;
}

void FlatHScrollBar::ValueChanged(int old_value)
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
