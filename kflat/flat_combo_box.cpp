// flat_combo_box.cpp
// Created by lulimin on 2020/9/11.

#include "flat_combo_box.h"
#include "flat_button.h"
#include "flat_text_box.h"
#include "flat_list_box.h"
#include "flat_ui.h"

/// \file flat_combo_box.cpp
/// \brief User interface combo box.

/// object: FlatComboBox
/// \brief User interface combo box object.
DEF_OBJECT(FlatComboBox, FlatWidget);

/// readonly: object InputBox
/// \brief Input text box.
DEF_READ_F(uniqid_t, FlatComboBox, InputBox, GetInputBoxID);
/// readonly: object DropButton
/// \brief Drop down button.
DEF_READ_F(uniqid_t, FlatComboBox, DropButton, GetDropButtonID);
/// readonly: object DropList
/// \brief Drop down list box.
DEF_READ_F(uniqid_t, FlatComboBox, DropList, GetDropListID);

/// property: string Text
/// \brief Text of input box.
DEF_PROP(const char*, FlatComboBox, Text);
/// property: int DropButtonWidth
/// \brief Width of drop button.
DEF_PROP(int, FlatComboBox, DropButtonWidth);
/// property: int DropDownWidth
/// \brief Width of drop down list box.
DEF_PROP(int, FlatComboBox, DropDownWidth);
/// property: int DropDownHeight
/// \brief Height of drop down list box.
DEF_PROP(int, FlatComboBox, DropDownHeight);
/// property: bool DroppedDown
/// \brief List dropped status.
DEF_PROP(bool, FlatComboBox, DroppedDown);

// FlatComboBox

FlatComboBox::FlatComboBox()
{
	m_pInputBox = NULL;
	m_pDropButton = NULL;
	m_pDropList = NULL;
	m_nDropButtonWidth = 0;
	m_nDropDownWidth = 0;
	m_nDropDownHeight = 0;
}

FlatComboBox::~FlatComboBox()
{
}

bool FlatComboBox::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();

	FlatUI* pFlatUI = this->GetFlatUI();

	m_pInputBox = (FlatTextBox*)pFlatUI->CreatePartWidget(this, "FlatTextBox");
	m_pDropButton = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pDropButton->SetArrowType(FlatButton::ARROW_DOWN);
	m_pDropList = (FlatListBox*)pFlatUI->CreateWidget("FlatListBox");
	m_pDropList->SetDelegator(this);
	return true;
}

bool FlatComboBox::Shutdown()
{
	SAFE_RELEASE(m_pDropList);
	return FlatWidget::Shutdown();
}

void FlatComboBox::Draw(flat_canvas_t* pCanvas)
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

	m_pInputBox->Draw(pCanvas);
	m_pDropButton->Draw(pCanvas);
}

void FlatComboBox::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_empty_string(DT_STRING, "");

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "DropButtonWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DropDownWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DropDownHeight", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_part(pMD, "InputBox", "FlatTextBox");
	flat_metadata_add_part(pMD, "DropButton", "FlatButton");
	flat_metadata_add_part(pMD, "DropList", "FlatListBox");
}

int FlatComboBox::OnKeyDown(unsigned int key, unsigned int flags)
{
	return m_pInputBox->OnKeyDown(key, flags);
}

int FlatComboBox::OnKeyUp(unsigned int key, unsigned int flags)
{
	return m_pInputBox->OnKeyUp(key, flags);
}

int FlatComboBox::OnChar(unsigned int ch, unsigned int flags)
{
	return m_pInputBox->OnChar(ch, flags);
}

int FlatComboBox::OnGotFocus()
{
	return m_pInputBox->OnGotFocus();
}

int FlatComboBox::OnLostFocus()
{
	return m_pInputBox->OnLostFocus();
}

int FlatComboBox::OnSizeChanged()
{
	this->PerformLayout();
	return 1;
}

int FlatComboBox::OnPartClick(FlatWidget* pPart, int x, int y)
{
	Assert(pPart != NULL);

	if (pPart == m_pDropButton)
	{
		bool dropped = this->GetDroppedDown();

		this->SetDroppedDown(!dropped);
		return 1;
	}
	
	return 0;
}

int FlatComboBox::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	if (pPart == m_pDropList)
	{
		m_pInputBox->SetText(m_pDropList->GetSelectedText());
		this->SetDroppedDown(false);
		
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select", FrameArgList(), NULL);
		}

		return 1;
	}
	
	return 0;
}

void FlatComboBox::SetText(const char* value)
{
	Assert(value != NULL);

	m_pInputBox->SetText(value);
}

const char* FlatComboBox::GetText() const
{
	return m_pInputBox->GetText();
}

void FlatComboBox::SetDropButtonWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nDropButtonWidth = value;
}

int FlatComboBox::GetDropButtonWidth() const
{
	return m_nDropButtonWidth;
}

void FlatComboBox::SetDropDownWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nDropDownWidth = value;
}

int FlatComboBox::GetDropDownWidth() const
{
	return m_nDropDownWidth;
}

void FlatComboBox::SetDropDownHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nDropDownHeight = value;
}

int FlatComboBox::GetDropDownHeight() const
{
	return m_nDropDownHeight;
}

uniqid_t FlatComboBox::GetInputBoxID() const
{
	if (NULL == m_pInputBox)
	{
		return uniqid_t();
	}

	return m_pInputBox->GetUID();
}

uniqid_t FlatComboBox::GetDropButtonID() const
{
	if (NULL == m_pDropButton)
	{
		return uniqid_t();
	}

	return m_pDropButton->GetUID();
}

uniqid_t FlatComboBox::GetDropListID() const
{
	if (NULL == m_pDropList)
	{
		return uniqid_t();
	}
	
	return m_pDropList->GetUID();
}

void FlatComboBox::SetDroppedDown(bool value)
{
	FlatUI* pFlatUI = this->GetFlatUI();
	uniqid_t drop_list_id = m_pDropList->GetUID();
	
	if (value)
	{
		if (!pFlatUI->FindFloating(drop_list_id))
		{
			int height = m_nDropDownHeight;

			if (0 == height)
			{
				int item_height = m_pDropList->GetItemHeight();
				int item_count = m_pDropList->GetTextCount();

				height = item_height * item_count + 8;
			}

			FlatWidget* pRoot = this->GetRootWidget();
			int y1 = this->GetGlobalTop() + this->GetHeight() + height;
			int y2 = pRoot->GetGlobalTop() + pRoot->GetHeight();
			int top;

			if (y1 > y2)
			{
				top = this->GetGlobalTop() - height;
			}
			else
			{
				top = this->GetGlobalTop() + this->GetHeight();
			}

			int width = m_nDropDownWidth;

			if (0 == width)
			{
				width = this->GetWidth();
			}

			m_pDropList->SetDynamicSlide(true);
			m_pDropList->SetGlobalLeftTop(this->GetGlobalLeft(), top);
			m_pDropList->SetSize(width, height);
			m_pDropList->SetSelectedIndex(-1);
			// Update scroll status.
			//m_pDropList->EndUpdate();
			pFlatUI->AddFloating(drop_list_id);
		}
	}
	else
	{
		pFlatUI->RemoveFloating(drop_list_id);
	}
}

bool FlatComboBox::GetDroppedDown() const
{
	return this->GetFlatUI()->FindFloating(m_pDropList->GetUID());
}

void FlatComboBox::PerformLayout()
{
	int btn_width = this->GetHeight();

	if (m_nDropButtonWidth > 0)
	{
		btn_width = m_nDropButtonWidth;
	}
	
	m_pInputBox->SetLeft(0);
	m_pInputBox->SetTop(0);
	m_pInputBox->SetSize(this->GetWidth() - btn_width, this->GetHeight());
	m_pDropButton->SetLeft(this->GetWidth() - btn_width);
	m_pDropButton->SetTop(0);
	m_pDropButton->SetSize(btn_width, this->GetHeight());

	if (m_nDropDownWidth > 0)
	{
		m_pDropList->SetWidth(m_nDropDownWidth);
	}

	if (m_nDropDownHeight > 0)
	{
		m_pDropList->SetHeight(m_nDropDownHeight);
	}
}
