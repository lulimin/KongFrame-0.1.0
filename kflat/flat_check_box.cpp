// flat_check_box.cpp
// Created by lulimin on 2020/9/11.

#include "flat_check_box.h"
#include "flat_ui.h"

/// \file flat_check_box.cpp
/// \brief User interface check box.

/// object: FlatCheckBox
/// \brief User interface check box object.
DEF_OBJECT(FlatCheckBox, FlatWidget);

/// property: string Text
/// \brief Show text.
DEF_PROP(const char*, FlatCheckBox, Text);
/// property: string CheckedImage
/// \brief Image in enable status.
DEF_PROP(const char*, FlatCheckBox, CheckedImage);
/// property: bool Checked
/// \brief Checked status.
DEF_PROP(bool, FlatCheckBox, Checked);

// FlatCheckBox

FlatCheckBox::FlatCheckBox()
{
	m_pCheckedImage = NULL;
	m_bChecked = false;
}

FlatCheckBox::~FlatCheckBox()
{
	if (m_pCheckedImage)
	{
		flat_image_delete(m_pCheckedImage);
	}
}

bool FlatCheckBox::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	return true;
}

bool FlatCheckBox::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatCheckBox::Draw(flat_canvas_t* pCanvas)
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

	if (this->ExistBackImage() && (m_pCheckedImage != NULL))
	{
		if (m_bChecked)
		{
			canvas_draw_image(pCanvas, x1, y1, x2, y2, m_pCheckedImage,
				this->GetImageLayoutValue());
		}
		else
		{
			this->DrawBackImage(pCanvas, x1, y1, x2, y2);
		}
	}
	else
	{
		int x3 = x1 + height / 4;
		int y3 = y1 + height / 4;
		int x4 = x3 + height / 2;
		int y4 = y3 + height / 2;

		if (m_bChecked)
		{
			canvas_draw_block(pCanvas, x3, y3, x4, y4, 
				this->GetForeColorValue());
		}
		else
		{
			canvas_draw_block(pCanvas, x3, y3, x4, y4, 
				this->GetBackColorValue());
			canvas_draw_frame(pCanvas, x3, y3, x4, y4,
				this->GetForeColorValue());
		}

		if (!m_sText.Empty())
		{
			this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
			canvas_draw_text_left(pCanvas, x1 + height, y1, x2, y2,
				this->GetForeColorValue(), m_sText.CString(), 0, 0);
		}
	}
}

void FlatCheckBox::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_empty_string(DT_STRING, "");

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "CheckedImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "Checked", FLAT_METATYPE_BOOLEAN,
		&def_false);
}

int FlatCheckBox::OnLeftDown(int x, int y, unsigned int flags)
{
	this->SetChecked(!m_bChecked);
	return 1;
}

void FlatCheckBox::SetText(const char* value)
{
	Assert(value != NULL);

	m_sText = value;
}

const char* FlatCheckBox::GetText() const
{
	return m_sText.CString();
}

void FlatCheckBox::SetCheckedImage(const char* value)
{
	Assert(value != NULL);

	m_sCheckedImage = value;
	m_pCheckedImage = flat_load_image(this->GetFlatUI(), m_pCheckedImage,
		value, false);
}

const char* FlatCheckBox::GetCheckedImage() const
{
	return m_sCheckedImage.CString();
}

void FlatCheckBox::SetChecked(bool value)
{
	if (value == m_bChecked)
	{
		return;
	}
	
	m_bChecked = value;
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}
}

bool FlatCheckBox::GetChecked() const
{
	return m_bChecked;
}
