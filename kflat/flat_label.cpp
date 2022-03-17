// flat_label.cpp
// Created by lulimin on 2020/9/11.

#include "flat_label.h"
#include "flat_ui.h"

/// \file flat_label.cpp
/// \brief User interface label.

/// object: FlatLabel
/// \brief User interface label object.
DEF_OBJECT(FlatLabel, FlatWidget);

/// property: string Text
/// \brief Text string.
DEF_PROP(const char*, FlatLabel, Text);
/// property: string TextAlign
/// \brief Text align mode.
DEF_PROP(const char*, FlatLabel, TextAlign);
/// property: bool Background
/// \brief Need draw background.
DEF_PROP(bool, FlatLabel, Background);
/// property: bool Border
/// \brief Need draw border.
DEF_PROP(bool, FlatLabel, Border);

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

// FlatLabel

FlatLabel::FlatLabel()
{
	m_nTextAlign = 0;
	m_bBackground = false;
	m_bBorder = false;
}

FlatLabel::~FlatLabel()
{
}

bool FlatLabel::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	//this->SetCanSelect(false);
	return true;
}

bool FlatLabel::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatLabel::Draw(flat_canvas_t* pCanvas)
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

	if (m_bBackground)
	{
		if (this->ExistBackImage())
		{
			this->DrawBackImage(pCanvas, x1, y1, x2, y2);
		}
		else
		{
			canvas_draw_block(pCanvas, x1, y1, x2, y2,
				this->GetBackColorValue());
		}
	}

	if (!m_sText.Empty())
	{
		this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
		
		if (m_nTextAlign == ALIGN_LEFT)
		{
			canvas_draw_text_left(pCanvas, x1, y1, x2, y2,
				this->GetForeColorValue(), m_sText.CString(), 0, 0);
		}
		else if (m_nTextAlign == ALIGN_CENTER)
		{
			canvas_draw_text_center(pCanvas, x1, y1, x2, y2,
				this->GetForeColorValue(), m_sText.CString(), 0, 0);
		}
		else if (m_nTextAlign == ALIGN_RIGHT)
		{
			canvas_draw_text_right(pCanvas, x1, y1, x2, y2,
				this->GetForeColorValue(), m_sText.CString(), 0, 0);
		}
	}

	if (m_bBorder)
	{
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}
}

void FlatLabel::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_empty_string(DT_STRING, "");
	FrameArgData def_text_align(DT_STRING, "Left");

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "TextAlign", FLAT_METATYPE_STRING,
		&def_text_align);
	flat_metadata_add_prop(pMD, "Background", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "Border", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_part_prop(pMD, "BackImage");
	flat_metadata_part_prop(pMD, "ImageLayout");
}

void FlatLabel::SetText(const char* value)
{
	Assert(value != NULL);

	m_sText = value;
}

const char* FlatLabel::GetText() const
{
	return m_sText.CString();
}

void FlatLabel::SetTextAlign(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Left") == 0)
	{
		m_nTextAlign = ALIGN_LEFT;
	}
	else if (stricmp(value, "Center") == 0)
	{
		m_nTextAlign = ALIGN_CENTER;
	}
	else if (stricmp(value, "Right") == 0)
	{
		m_nTextAlign = ALIGN_RIGHT;
	}
}

const char* FlatLabel::GetTextAlign() const
{
	switch (m_nTextAlign)
	{
	case ALIGN_LEFT:
		return "Left";
	case ALIGN_CENTER:
		return "Center";
	case ALIGN_RIGHT:
		return "Right";
	default:
		Assert(0);
		break;
	}

	return "";
}

void FlatLabel::SetBackground(bool value)
{ 
	m_bBackground = value;
}

bool FlatLabel::GetBackground() const
{ 
	return m_bBackground; 
}

void FlatLabel::SetBorder(bool value)
{ 
	m_bBorder = value;
}

bool FlatLabel::GetBorder() const
{ 
	return m_bBorder;
}
