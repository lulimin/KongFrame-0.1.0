// flat_radio_button.cpp
// Created by lulimin on 2020/9/11.

#include "flat_radio_button.h"
#include "flat_ui.h"

/// \file flat_radio_button.cpp
/// \brief User interface radio button.

/// object: FlatRadioButton
/// \brief User interface radio button object.
DEF_OBJECT(FlatRadioButton, FlatWidget);

/// readonly: bool Checked
/// \brief Checked status.
DEF_READ(bool, FlatRadioButton, Checked);

/// property: string Text
/// \brief Show text.
DEF_PROP(const char*, FlatRadioButton, Text);
/// property: string CheckedImage
/// \brief Image in cheched status.
DEF_PROP(const char*, FlatRadioButton, CheckedImage);

/// function: bool SetCheckedValue()
/// \brief Set checked status.
DEF_FUNC_0(bool, FlatRadioButton, SetCheckedValue);
/// function: bool ClearCheckedValue()
/// \brief Clear checked status.
DEF_FUNC_0(bool, FlatRadioButton, ClearCheckedValue);

// FlatRadioButton

FlatRadioButton::FlatRadioButton()
{
	m_pCheckedImage = NULL;
	m_bChecked = false;
}

FlatRadioButton::~FlatRadioButton()
{
	if (m_pCheckedImage)
	{
		flat_image_delete(m_pCheckedImage);
	}
}

bool FlatRadioButton::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	return true;
}

bool FlatRadioButton::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatRadioButton::Draw(flat_canvas_t* pCanvas)
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

void FlatRadioButton::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_empty_string(DT_STRING, "");

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "CheckedImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
}

int FlatRadioButton::OnLeftDown(int x, int y, unsigned int flags)
{
	this->SetCheckedValue();
	return 1;
}

void FlatRadioButton::SetText(const char* value)
{
	Assert(value != NULL);

	m_sText = value;
}

const char* FlatRadioButton::GetText() const
{
	return m_sText.CString();
}

void FlatRadioButton::SetCheckedImage(const char* value)
{
	Assert(value != NULL);

	m_sCheckedImage = value;
	m_pCheckedImage = flat_load_image(this->GetFlatUI(), m_pCheckedImage,
		value, false);
}

const char* FlatRadioButton::GetCheckedImage() const
{
	return m_sCheckedImage.CString();
}

bool FlatRadioButton::GetChecked() const
{
	return m_bChecked;
}

bool FlatRadioButton::SetCheckedValue()
{
	if (m_bChecked)
	{
		return false;
	}

	m_bChecked = true;

	FlatWidget* pParent = this->GetParent();

	if (pParent)
	{
		flat_container_t* pContainer = pParent->GetContainer();

		if (pContainer)
		{
			// Clear checked status of other children in same container.
			size_t size = flat_container_get_size(pContainer);
			FlatWidget** data = flat_container_get_data(pContainer);

			for (size_t i = 0; i < size; ++i)
			{
				FlatWidget* pChild = data[i];

				if (NULL == pChild)
				{
					continue;
				}

				if (pChild == this)
				{
					continue;
				}

				if (!pChild->GetDesc()->DeriveFrom("FlatRadioButton"))
				{
					continue;
				}

				FlatRadioButton* pRB = (FlatRadioButton*)pChild;

				pRB->ClearCheckedValue();
			}
		}
	}
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}

	return true;
}

bool FlatRadioButton::ClearCheckedValue()
{
	if (!m_bChecked)
	{
		return false;
	}
	
	m_bChecked = false;
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}

	return true;
}
