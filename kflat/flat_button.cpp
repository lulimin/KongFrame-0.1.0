// flat_button.cpp
// Created by lulimin on 2020/9/11.

#include "flat_button.h"
#include "flat_utility.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_button.cpp
/// \brief User interface button.

/// object: FlatButton
/// \brief User interface button object.
DEF_OBJECT(FlatButton, FlatWidget);

/// property: string Text
/// \brief Text string on button.
DEF_PROP(const char*, FlatButton, Text);
/// property: string CaptureImage
/// \brief Image name in captured status.
DEF_PROP(const char*, FlatButton, CaptureImage);
/// property: string FocusedImage
/// \brief Image name in focused status.
DEF_PROP(const char*, FlatButton, FocusedImage);
/// property: string PressImage
/// \brief Image name in pressed status.
DEF_PROP(const char*, FlatButton, PressImage);
/// property: string GreyImage
/// \brief Image name in disabled status.
DEF_PROP(const char*, FlatButton, GreyImage);
/// property: string CaptureColor
/// \brief Color in captured.
DEF_PROP(ArgString, FlatButton, CaptureColor);
/// property: string FocusedColor
/// \brief Color in focused.
DEF_PROP(ArgString, FlatButton, FocusedColor);
/// property: string PressColor
/// \brief Color in pressed.
DEF_PROP(ArgString, FlatButton, PressColor);
/// property: string GreyColor
/// \brief Color in grey.
DEF_PROP(ArgString, FlatButton, GreyColor);
/// property: bool PushClick
/// \brief Generate click event when mouse down.
DEF_PROP(bool, FlatButton, PushClick);
/// property: bool HoverClick
/// \brief Generate click event when mouse hover.
DEF_PROP(bool, FlatButton, HoverClick);

#define INIT_CAPTURE_COLOR 0xFF202020
#define INIT_FOCUSED_COLOR 0xFF404040
#define INIT_PRESS_COLOR 0xFF606060
#define INIT_GREY_COLOR 0xFF808080

// FlatButton

FlatButton::FlatButton()
{
	m_pCaptureImage = NULL;
	m_pFocusedImage = NULL;
	m_pPressImage = NULL;
	m_pGreyImage = NULL;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
	m_nFocusedColor = INIT_FOCUSED_COLOR;
	m_nPressColor = INIT_PRESS_COLOR;
	m_nGreyColor = INIT_GREY_COLOR;
	m_bPushClick = false;
	m_bHoverClick = false;
	m_bPressed = false;
	m_nArrowType = 0;
	m_nClickX = -1;
	m_nClickY = -1;
}

FlatButton::~FlatButton()
{
	if (m_pCaptureImage)
	{
		flat_image_delete(m_pCaptureImage);
	}

	if (m_pFocusedImage)
	{
		flat_image_delete(m_pFocusedImage);
	}

	if (m_pPressImage)
	{
		flat_image_delete(m_pPressImage);
	}

	if (m_pGreyImage)
	{
		flat_image_delete(m_pGreyImage);
	}
}

bool FlatButton::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	this->SetCanFocus(true);
	return true;
}

bool FlatButton::Shutdown()
{
	return FlatWidget::Shutdown();
}

static void draw_arrow(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color, int arrow_type)
{
	int w = x2 - x1;
	int h = y2 - y1;

	if ((w < 8) || (h < 8))
	{
		return;
	}
	
	switch (arrow_type)
	{
	case FlatButton::ARROW_LEFT:
	{
		int edge = h / 2 - 2;
		int px1 = x1 + w / 3;
		int py1 = y1 + h / 2 - 1;
		int px2 = px1 + edge;
		int py2 = py1 - edge;
		int px3 = x1 + w / 3;
		int py3 = y1 + h / 2;
		int px4 = px3 + edge;
		int py4 = py3 + edge;
		int lines[8] = {
			px1, py1, px2, py2, px3, py3, px4, py4,
		};

		canvas_draw_lines(pCanvas, lines, 2, color);
		break;
	}
	case FlatButton::ARROW_RIGHT:
	{
		int edge = h / 2 - 2;
		int px1 = x1 + w * 2 / 3;
		int py1 = y1 + h / 2;
		int px2 = px1 - edge;
		int py2 = py1 - edge;
		int px3 = x1 + w * 2 / 3;
		int py3 = y1 + h / 2 - 1;
		int px4 = px3 - edge;
		int py4 = py3 + edge;
		int lines[8] = {
			px1, py1, px2, py2, px3, py3, px4, py4,
		};

		canvas_draw_lines(pCanvas, lines, 2, color);
		break;
	}
	case FlatButton::ARROW_UP:
	{
		int edge = w / 2 - 2;
		int px1 = x1 + w / 2;
		int py1 = y1 + h / 3 - 1;
		int px2 = px1 - edge;
		int py2 = py1 + edge;
		int px3 = x1 + w / 2;
		int py3 = y1 + h / 3;
		int px4 = px3 + edge;
		int py4 = py3 + edge;
		int lines[8] = {
			px1, py1, px2, py2, px3, py3, px4, py4,
		};

		canvas_draw_lines(pCanvas, lines, 2, color);
		break;
	}
	case FlatButton::ARROW_DOWN:
	{
		int edge = w / 2 - 2;
		int px1 = x1 + w / 2;
		int py1 = y1 + h * 2 / 3 + 1;
		int px2 = px1 - edge;
		int py2 = py1 - edge;
		int px3 = x1 + w / 2;
		int py3 = y1 + h * 2 / 3;
		int px4 = px3 + edge;
		int py4 = py3 - edge;
		int lines[8] = {
			px1, py1, px2, py2, px3, py3, px4, py4,
		};

		canvas_draw_lines(pCanvas, lines, 2, color);
		break;
	}
	default:
		break;
	}
}

void FlatButton::Draw(flat_canvas_t* pCanvas)
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
		flat_image_t* pImage = this->GetImage();
		
		if (this->GetEnabled())
		{
			if (m_bPressed && this->GetCapture())
			{
				if (m_pPressImage)
				{
					pImage = m_pPressImage;
				}
			}
			else if (this->GetCapture())
			{
				if (m_pCaptureImage)
				{
					pImage = m_pCaptureImage;
				}
			}
			else if (this->GetFocused())
			{
				if (m_pFocusedImage)
				{
					pImage = m_pFocusedImage;
				}
			}
		}
		else
		{
			if (m_pGreyImage)
			{
				pImage = m_pGreyImage;
			}
		}

		canvas_draw_image(pCanvas, x1, y1, x2, y2, pImage, 
			this->GetImageLayoutValue());
	}
	else
	{
		unsigned int back_color = this->GetBackColorValue();

		if (this->GetEnabled())
		{
			if (m_bPressed && this->GetCapture())
			{
				back_color = m_nPressColor;
			}
			else if (this->GetCapture())
			{
				back_color = m_nCaptureColor;
			}
			else if (this->GetFocused())
			{
				back_color = m_nFocusedColor;
			}
		}
		else
		{
			back_color = m_nGreyColor;
		}

		canvas_draw_block(pCanvas, x1, y1, x2, y2, back_color);
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());

		if (m_nArrowType != 0)
		{
			draw_arrow(pCanvas, x1, y1, x2, y2, this->GetForeColorValue(),
				m_nArrowType);
		}
	}

	if (!m_sText.Empty())
	{
		unsigned int text_color = this->GetForeColorValue();

//		if (this->GetEnabled())
//		{
//			if (m_bPressed && this->GetCapture())
//			{
//				text_color = m_nPressColor;
//			}
//			else if (this->GetCapture())
//			{
//				text_color = m_nCaptureColor;
//			}
//			else if (this->GetFocused())
//			{
//				text_color = m_nFocusedColor;
//			}
//		}
//		else
//		{
//			text_color = m_nGreyColor;
//		}
		
		this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
		canvas_draw_text_center(pCanvas, x1, y1, x2, y2, text_color,
			m_sText.CString(), 0, 0);
	}
}

void FlatButton::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_empty_string(DT_STRING, "");
	char buffer[32];
	FrameArgData def_capture_color(DT_STRING,
		flat_rgba_to_string(INIT_CAPTURE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_focused_color(DT_STRING,
		flat_rgba_to_string(INIT_FOCUSED_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_press_color(DT_STRING,
		flat_rgba_to_string(INIT_PRESS_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_grey_color(DT_STRING,
		flat_rgba_to_string(INIT_GREY_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "CaptureImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "FocusedImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "PressImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "GreyImage", FLAT_METATYPE_IMAGE,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_add_prop(pMD, "FocusedColor", FLAT_METATYPE_COLOR,
		&def_focused_color);
	flat_metadata_add_prop(pMD, "PressColor", FLAT_METATYPE_COLOR,
		&def_press_color);
	flat_metadata_add_prop(pMD, "GreyColor", FLAT_METATYPE_COLOR,
		&def_grey_color);
	flat_metadata_add_prop(pMD, "PushClick", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "HoverClick", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_part_prop(pMD, "BackImage");
	flat_metadata_part_prop(pMD, "CaptureImage");
	flat_metadata_part_prop(pMD, "FocusedImage");
	flat_metadata_part_prop(pMD, "PressImage");
	flat_metadata_part_prop(pMD, "GreyImage");
	flat_metadata_part_prop(pMD, "ImageLayout");
	flat_metadata_add_event(pMD, "click");
}

int FlatButton::OnClickReset()
{
	m_bPressed = false;
	return 1;
}

int FlatButton::OnMouseHover(int x, int y)
{
	if (m_bHoverClick)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click", FrameArgList(), NULL);
		}

		FlatWidget* pDelegator = this->GetDelegator();

		if (pDelegator)
		{
			pDelegator->OnPartClick(this, x, y);
		}

		return 1;
	}

	return 0;
}

int FlatButton::OnLeftDown(int x, int y, unsigned int flags)
{
	m_bPressed = true;
	m_nClickX = x - this->GetGlobalLeft();
	m_nClickY = y - this->GetGlobalTop();
	
	if (m_bPushClick)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click", FrameArgList(), NULL);
		}

		FlatWidget* pDelegator = this->GetDelegator();

		if (pDelegator)
		{
			pDelegator->OnPartClick(this, x, y);
		}

		return 1;
	}
	
	return 1;
}

int FlatButton::OnLeftUp(int x, int y, unsigned int flags)
{
	m_bPressed = false;

	if (m_bPushClick)
	{
		return 0;
	}
	
	if (this->GetCapture())
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click", FrameArgList(), NULL);
		}

		FlatWidget* pDelegator = this->GetDelegator();

		if (pDelegator)
		{
			pDelegator->OnPartClick(this, x, y);
		}
	}

	return 1;
}

int FlatButton::OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags)
{
	FlatWidget* pDelegator = this->GetDelegator();

	if (pDelegator)
	{
		pDelegator->OnPartDrag(this, x, y, m_nClickX, m_nClickY, flags);
	}
	
	return 1;
}

int FlatButton::OnKeyDown(unsigned int key, unsigned int flags)
{
	switch (key)
	{
	case IInputService::KEY_RETURN:
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "click", FrameArgList(), NULL);
		}

		FlatWidget* pDelegator = this->GetDelegator();

		if (pDelegator)
		{
			pDelegator->OnPartClick(this, 0, 0);
		}
		
		break;
	}
	default:
		return 0;
	}
	
	return 1;
}

void FlatButton::SetText(const char* value)
{
	Assert(value != NULL);

	m_sText = value;
}

const char* FlatButton::GetText() const
{
	return m_sText.CString();
}

void FlatButton::SetCaptureImage(const char* value)
{
	Assert(value != NULL);

	m_sCaptureImage = value;
	m_pCaptureImage = flat_load_image(this->GetFlatUI(), m_pCaptureImage,
		value, false);
}

const char* FlatButton::GetCaptureImage() const
{
	return m_sCaptureImage.CString();
}

void FlatButton::SetFocusedImage(const char* value)
{
	Assert(value != NULL);

	m_sFocusedImage = value;
	m_pFocusedImage = flat_load_image(this->GetFlatUI(), m_pFocusedImage,
		value, false);
}

const char* FlatButton::GetFocusedImage() const
{
	return m_sFocusedImage.CString();
}

void FlatButton::SetPressImage(const char* value)
{
	Assert(value != NULL);

	m_sPressImage = value;
	m_pPressImage = flat_load_image(this->GetFlatUI(), m_pPressImage, value,
		false);
}

const char* FlatButton::GetPressImage() const
{
	return m_sPressImage.CString();
}

void FlatButton::SetGreyImage(const char* value)
{
	Assert(value != NULL);

	m_sGreyImage = value;
	m_pGreyImage = flat_load_image(this->GetFlatUI(), m_pGreyImage, value,
		false);
}

const char* FlatButton::GetGreyImage() const
{
	return m_sGreyImage.CString();
}

void FlatButton::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatButton::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

void FlatButton::SetFocusedColor(const char* value)
{
	Assert(value != NULL);

	m_nFocusedColor = flat_string_to_rgba(value);
}

ArgString FlatButton::GetFocusedColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nFocusedColor, buffer, sizeof(buffer));
}

void FlatButton::SetPressColor(const char* value)
{
	Assert(value != NULL);

	m_nPressColor = flat_string_to_rgba(value);
}

ArgString FlatButton::GetPressColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nPressColor, buffer, sizeof(buffer));
}

void FlatButton::SetGreyColor(const char* value)
{
	Assert(value != NULL);

	m_nGreyColor = flat_string_to_rgba(value);
}

ArgString FlatButton::GetGreyColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nGreyColor, buffer, sizeof(buffer));
}

void FlatButton::SetPushClick(bool value)
{
	m_bPushClick = value;
}

bool FlatButton::GetPushClick() const
{
	return m_bPushClick;
}

void FlatButton::SetHoverClick(bool value)
{
	m_bHoverClick = value;
}

bool FlatButton::GetHoverClick() const
{
	return m_bHoverClick;
}

void FlatButton::SetArrowType(int value)
{
	m_nArrowType = value;
}
