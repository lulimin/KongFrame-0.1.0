// flat_progress_bar.cpp
// Created by lulimin on 2020/9/11.

#include "flat_progress_bar.h"

/// \file flat_progress_bar.cpp
/// \brief User interface progress bar.

/// object: FlatProgressBar
/// \brief User interface progress bar object.
DEF_OBJECT(FlatProgressBar, FlatWidget);

/// property: int Minimum
/// \brief Minimum value.
DEF_PROP(int, FlatProgressBar, Minimum);
/// property: int Maximum
/// \brief Maximum value.
DEF_PROP(int, FlatProgressBar, Maximum);
/// property: int Value
/// \brief Current value.
DEF_PROP(int, FlatProgressBar, Value);

// FlatProgressBar

FlatProgressBar::FlatProgressBar()
{
	m_nMinimum = 0;
	m_nMaximum = 100;
	m_nValue = 0;
}

FlatProgressBar::~FlatProgressBar()
{
}

bool FlatProgressBar::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	return true;
}

bool FlatProgressBar::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatProgressBar::Draw(flat_canvas_t* pCanvas)
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

	if ((m_nMaximum > m_nMinimum) && (m_nValue >= m_nMinimum) &&
		(m_nValue <= m_nMaximum))
	{
		float percent = (float)(m_nValue - m_nMinimum) / 
			(float)(m_nMaximum - m_nMinimum);

		if (m_nValue == m_nMaximum)
		{
			percent = 1.0F;
		}

		int x3 = x1 + (int)(percent * width);

		if (m_nValue == m_nMaximum)
		{
			x3 = x2;
		}

		canvas_draw_block(pCanvas, x1, y1, x3, y2, this->GetForeColorValue());
	}
}

void FlatProgressBar::SetMetadata(flat_metadata_t* pMD)
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
}

void FlatProgressBar::SetMinimum(int value)
{
	m_nMinimum = value;
	this->AdjustValue();
}

int FlatProgressBar::GetMinimum() const
{
	return m_nMinimum;
}

void FlatProgressBar::SetMaximum(int value)
{
	m_nMaximum = value;
	this->AdjustValue();
}

int FlatProgressBar::GetMaximum() const
{
	return m_nMaximum;
}

void FlatProgressBar::SetValue(int value)
{
	m_nValue = value;
	this->AdjustValue();
}

int FlatProgressBar::GetValue() const
{
	return m_nValue;
}

void FlatProgressBar::AdjustValue()
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
