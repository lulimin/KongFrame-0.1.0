// flat_track_bar.cpp
// Created by lulimin on 2020/9/11.

#include "flat_track_bar.h"
#include "flat_button.h"
#include "flat_ui.h"

/// \file flat_track_bar.cpp
/// \brief User interface track bar.

/// object: FlatTrackBar
/// \brief User interface track bar object.
DEF_OBJECT(FlatTrackBar, FlatWidget);

/// readonly: object TrackButton
/// \brief Get track button object.
DEF_READ_F(uniqid_t, FlatTrackBar, TrackButton, GetTrackButtonID);

/// property: int Minimum
/// \brief Minimum horizontal value.
DEF_PROP(int, FlatTrackBar, Minimum);
/// property: int Maximum
/// \brief Maximum horizontal value.
DEF_PROP(int, FlatTrackBar, Maximum);
/// property: int Value
/// \brief Current horizontal value.
DEF_PROP(int, FlatTrackBar, Value);
/// property: int VerticalMinimum
/// \brief Minimum vertical value.
DEF_PROP(int, FlatTrackBar, VerticalMinimum);
/// property: int VerticalMaximum
/// \brief Maximum vertical value.
DEF_PROP(int, FlatTrackBar, VerticalMaximum);
/// property: int VerticalValue
/// \brief Current vertical value.
DEF_PROP(int, FlatTrackBar, VerticalValue);

// FlatTrackBar

FlatTrackBar::FlatTrackBar()
{
	m_pTrackButton = NULL;
	m_nMinimum = 0;
	m_nMaximum = 100;
	m_nValue = 0;
	m_nVerticalMinimum = 0;
	m_nVerticalMaximum = 0;
	m_nVerticalValue = 0;
}

FlatTrackBar::~FlatTrackBar()
{
}

bool FlatTrackBar::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();

	FlatUI* pFlatUI = this->GetFlatUI();

	m_pTrackButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pTrackButton->SetSize(20, 20);
	return true;
}

bool FlatTrackBar::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatTrackBar::Draw(flat_canvas_t* pCanvas)
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

	if (m_pTrackButton)
	{
		m_pTrackButton->Draw(pCanvas);
	}
}

void FlatTrackBar::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_maximum(DT_INT32, 100);

	flat_metadata_add_prop(pMD, "Minimum", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "Maximum", FLAT_METATYPE_INT32,
		&def_maximum);
	flat_metadata_add_prop(pMD, "Value", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "VerticalMinimum", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "VerticalMaximum", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "VerticalValue", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_event(pMD, "changed");
	flat_metadata_add_part(pMD, "TrackButton", "FlatButton");
}

FlatWidget* FlatTrackBar::GetRegionOf(int x, int y)
{
	if (!this->GetVisible())
	{
		return NULL;
	}

	if (this->GetDesignMode())
	{
		if (!this->InSelfRegion(x, y))
		{
			// Not in region of widget.
			return NULL;
		}

		// In design status.
		return this;
	}

	FlatWidget* pWidget = this->InPartRegion(x, y);

	if (pWidget)
	{
		return pWidget;
	}

	if (!this->InSelfRegion(x, y))
	{
		// Not in region of widget.
		return NULL;
	}

	if (this->OnTestTransparency(x, y))
	{
		// Is transparency part.
		return NULL;
	}

	return this;
}

int FlatTrackBar::OnLeftDown(int x, int y, unsigned int flags)
{
	bool track_horizontal = (m_nMaximum > m_nMinimum);
	bool track_vertical = (m_nVerticalMaximum > m_nMinimum);

	if (track_horizontal)
	{
		int half_track_width = m_pTrackButton->GetWidth() / 2;
		int beg_x = 0;
		int end_x = this->GetWidth();
		int track_x = x - this->GetGlobalLeft();

		if (track_x < beg_x)
		{
			track_x = beg_x;
		}

		if (track_x > end_x)
		{
			track_x = end_x;
		}

		m_pTrackButton->SetLeft(track_x - half_track_width);

		int stride = m_nMaximum - m_nMinimum;
		double rate = (double)(track_x - beg_x) / (double)(end_x - beg_x);
		int new_value = m_nMinimum + (int)(rate * (double)stride);
		int old_value = m_nValue;

		if (new_value != old_value)
		{
			m_nValue = new_value;
			this->ValueChanged(old_value);
			this->AdjustValue();
		}
	}

	if (track_vertical)
	{
		int half_track_height = m_pTrackButton->GetHeight() / 2;
		int beg_y = 0;
		int end_y = this->GetHeight();
		int track_y = y - this->GetGlobalTop();

		if (track_y < beg_y)
		{
			track_y = beg_y;
		}

		if (track_y > end_y)
		{
			track_y = end_y;
		}

		m_pTrackButton->SetTop(track_y - half_track_height);

		int stride = m_nVerticalMaximum - m_nVerticalMinimum;
		double rate = (double)(track_y - beg_y) / (double)(end_y - beg_y);
		int new_value = m_nVerticalMinimum + (int)(rate * (double)stride);
		int old_value = m_nVerticalValue;

		if (new_value != old_value)
		{
			m_nVerticalValue = new_value;
			this->VerticalValueChanged(old_value);
			this->AdjustVerticalValue();
		}
	}

	return 1;
}

int FlatTrackBar::OnSizeChanged()
{
	this->PerformLayout();
	return 1;
}

int FlatTrackBar::OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
	unsigned int flags)
{
	Assert(pPart != NULL);

	if (pPart != m_pTrackButton)
	{
		return 0;
	}

	bool track_horizontal = (m_nMaximum > m_nMinimum);
	bool track_vertical = (m_nVerticalMaximum > m_nMinimum);

	if (track_horizontal)
	{
		int half_track_width = m_pTrackButton->GetWidth() / 2;
		int beg_x = 0;
		int end_x = this->GetWidth();
		int track_x = x - this->GetGlobalLeft() - cx + half_track_width;

		if (track_x < beg_x)
		{
			track_x = beg_x;
		}

		if (track_x > end_x)
		{
			track_x = end_x;
		}

		m_pTrackButton->SetLeft(track_x - half_track_width);

		int stride = m_nMaximum - m_nMinimum;
		double rate = (double)(track_x - beg_x) / (double)(end_x - beg_x);
		int new_value = m_nMinimum + (int)(rate * (double)stride);
		int old_value = m_nValue;

		if (new_value != old_value)
		{
			m_nValue = new_value;
			this->ValueChanged(old_value);
			this->AdjustValue();
		}
	}

	if (track_vertical)
	{
		int half_track_height = m_pTrackButton->GetHeight() / 2;
		int beg_y = 0;
		int end_y = this->GetHeight();
		int track_y = y - this->GetGlobalTop() - cy + half_track_height;

		if (track_y < beg_y)
		{
			track_y = beg_y;
		}

		if (track_y > end_y)
		{
			track_y = end_y;
		}

		m_pTrackButton->SetTop(track_y - half_track_height);

		int stride = m_nVerticalMaximum - m_nVerticalMinimum;
		double rate = (double)(track_y - beg_y) / (double)(end_y - beg_y);
		int new_value = m_nVerticalMinimum + (int)(rate * (double)stride);
		int old_value = m_nVerticalValue;

		if (new_value != old_value)
		{
			m_nVerticalValue = new_value;
			this->VerticalValueChanged(old_value);
			this->AdjustVerticalValue();
		}
	}

	return 1;
}

void FlatTrackBar::SetMinimum(int value)
{
	m_nMinimum = value;
	this->AdjustValue();
}

int FlatTrackBar::GetMinimum() const
{
	return m_nMinimum;
}

void FlatTrackBar::SetMaximum(int value)
{
	m_nMaximum = value;
	this->AdjustValue();
}

int FlatTrackBar::GetMaximum() const
{
	return m_nMaximum;
}

void FlatTrackBar::SetValue(int value)
{
	int old_value = m_nValue;
	
	m_nValue = value;
	this->ValueChanged(old_value);
	this->AdjustValue();
	this->LocateTrackButton();
}

int FlatTrackBar::GetValue() const
{
	return m_nValue;
}

void FlatTrackBar::SetVerticalMinimum(int value)
{
	m_nVerticalMinimum = value;
	this->AdjustVerticalValue();
}

int FlatTrackBar::GetVerticalMinimum() const
{
	return m_nVerticalMinimum;
}

void FlatTrackBar::SetVerticalMaximum(int value)
{
	m_nVerticalMaximum = value;
	this->AdjustVerticalValue();
}

int FlatTrackBar::GetVerticalMaximum() const
{
	return m_nVerticalMaximum;
}

void FlatTrackBar::SetVerticalValue(int value)
{
	int old_value = m_nVerticalValue;
	
	m_nVerticalValue = value;
	this->ValueChanged(old_value);
	this->AdjustVerticalValue();
	this->LocateTrackButton();
}

int FlatTrackBar::GetVerticalValue() const
{
	return m_nVerticalValue;
}

uniqid_t FlatTrackBar::GetTrackButtonID() const
{
	if (NULL == m_pTrackButton)
	{
		return uniqid_t();
	}

	return m_pTrackButton->GetUID();
}

void FlatTrackBar::AdjustValue()
{
	if (m_nValue < m_nMinimum)
	{
		m_nValue = m_nMinimum;
	}

	if (m_nValue > m_nMaximum)
	{
		m_nValue = m_nMaximum;
	}
}

void FlatTrackBar::AdjustVerticalValue()
{
	if (m_nVerticalValue < m_nVerticalMinimum)
	{
		m_nVerticalValue = m_nVerticalMinimum;
	}

	if (m_nVerticalValue > m_nVerticalMaximum)
	{
		m_nVerticalValue = m_nVerticalMaximum;
	}
}

void FlatTrackBar::PerformLayout()
{
	this->LocateTrackButton();
}

void FlatTrackBar::ValueChanged(int old_value)
{
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}
}

void FlatTrackBar::VerticalValueChanged(int old_value)
{
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}
}

void FlatTrackBar::LocateTrackButton()
{
	bool track_horizontal = (m_nMaximum > m_nMinimum);
	bool track_vertical = (m_nVerticalMaximum > m_nVerticalMinimum);
	int width = this->GetWidth();
	int height = this->GetHeight();
	int half_track_width = m_pTrackButton->GetWidth() / 2;
	int half_track_height = m_pTrackButton->GetHeight() / 2;
	int cx;
	int cy;

	if (track_horizontal)
	{
		double rate = (double)(m_nValue - m_nMinimum) /
			(double)(m_nMaximum - m_nMinimum);

		cx = (int)(rate * width + 0.5);
	}
	else
	{
		cx = width / 2;
	}

	if (track_vertical)
	{
		double rate = (double)(m_nVerticalValue - m_nVerticalMinimum) / 
			(double)(m_nVerticalMaximum - m_nVerticalMinimum);

		cy = (int)(rate * height + 0.5);
	}
	else
	{
		cy = height / 2;
	}

	m_pTrackButton->SetLeft(cx - half_track_width);
	m_pTrackButton->SetTop(cy - half_track_height);
}
