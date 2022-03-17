// flat_menu.cpp
// Created by lulimin on 2020/9/11.

#include "flat_menu.h"
#include "flat_ui.h"

/// \file flat_menu.cpp
/// \brief User interface menu.

/// object: FlatMenu
/// \brief User interface menu object.
DEF_OBJECT(FlatMenu, FlatWidget);

/// property: int StripHeight
/// \brief Menu strip height.
DEF_PROP(int, FlatMenu, StripHeight);
/// property: string DisableColor
/// \brief Color of disabled menu.
DEF_PROP(ArgString, FlatMenu, DisableColor);
/// property: string CaptureColor
/// \brief Color when mouse captured.
DEF_PROP(ArgString, FlatMenu, CaptureColor);
/// property: object Owner
/// \brief Owner widget object id.
DEF_PROP(uniqid_t, FlatMenu, Owner);

/// function: bool AddStrip(string name, string text)
/// \brief Add menu strip.
/// \param name Menu strip name.
/// \param text Menu strip text.
DEF_FUNC_2(bool, FlatMenu, AddStrip, const char*, const char*);
/// function: bool RemoveStrip(string name)
/// \brief Remove menu strip.
/// \param name Menu strip name.
DEF_FUNC_1(bool, FlatMenu, RemoveStrip, const char*);
/// function: bool ClearStrips()
/// \brief Clear menu strips.
DEF_FUNC_0(bool, FlatMenu, ClearStrips);
/// function: bool DisableStrip(string name)
/// \brief Disable menu strip.
/// \param name Menu strip name.
DEF_FUNC_1(bool, FlatMenu, DisableStrip, const char*);
/// function: bool EnableStrip(string name)
/// \brief Enable menu strip.
/// \param name Menu strip name.
DEF_FUNC_1(bool, FlatMenu, EnableStrip, const char*);

#define INIT_DISABLE_COLOR 0xFFC0C0C0
#define INIT_CAPTURE_COLOR 0xFF0000FF

// FlatMenu

FlatMenu::FlatMenu()
{
	m_nStripHeight = 30;
	m_nDisableColor = INIT_DISABLE_COLOR;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
}

FlatMenu::~FlatMenu()
{
	this->ClearStrips();
}

bool FlatMenu::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetWidth(100);
	return true;
}

bool FlatMenu::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatMenu::Draw(flat_canvas_t* pCanvas)
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

	this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());

	int x3 = x1 + 4;
	int y3 = y1 + 4;
	int strip_width = width - 8;
	int strip_height = m_nStripHeight;

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		strip_t* pStrip = m_Strips[i];

		if (pStrip->sText == "-")
		{
			int line_y = y3 + strip_height / 4;
			int lines[] = { x1, line_y, x2, line_y };

			canvas_draw_lines(pCanvas, lines, 1, this->GetForeColorValue());
			y3 += strip_height / 2;
		}
		else
		{
			unsigned int color;

			if (pStrip->bDisabled)
			{
				color = m_nDisableColor;
			}
			else
			{
				color = this->GetForeColorValue();
			}

			int x4 = x3 + strip_width;
			int y4 = y3 + strip_height;

			if (this->GetCapture() && pStrip->bCapture)
			{
				canvas_draw_block(pCanvas, x3, y3, x4, y4, m_nCaptureColor);
			}
			
			canvas_draw_text_left(pCanvas, x3, y3, x4, y4, color,
				pStrip->sText.CString(), 0, 0);
			y3 += strip_height;
		}
	}
}

void FlatMenu::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	char buffer[32];
	FrameArgData def_disable_color(DT_STRING,
		flat_rgba_to_string(INIT_DISABLE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_capture_color(DT_STRING,
		flat_rgba_to_string(INIT_CAPTURE_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "StripHeight", FLAT_METATYPE_INT32,
		NULL);
	flat_metadata_add_prop(pMD, "DisableColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
}

int FlatMenu::OnMouseMove(int x, int y, unsigned int flags)
{
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int x3 = x1 + 4;
	int y3 = y1 + 4;
	int strip_width = this->GetWidth() - 8;
	int strip_height = m_nStripHeight;

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		strip_t* pStrip = m_Strips[i];

		if (pStrip->sText == "-")
		{
			y3 += strip_height / 2;
			continue;
		}

		int x4 = x3 + strip_width;
		int y4 = y3 + strip_height;

		if (flat_in_rectangle(x, y, x3, y3, x4, y4))
		{
			pStrip->bCapture = true;
		}
		else
		{
			pStrip->bCapture = false;
		}

		y3 += strip_height;
	}
	
	return 1;
}

int FlatMenu::OnLeftDown(int x, int y, unsigned int flags)
{
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int x3 = x1 + 4;
	int y3 = y1 + 4;
	int strip_width = this->GetWidth() - 8;
	int strip_height = m_nStripHeight;

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		strip_t* pStrip = m_Strips[i];

		if (pStrip->sText == "-")
		{
			y3 += strip_height / 2;
			continue;
		}
		
		if (!pStrip->bDisabled)
		{
			int x4 = x3 + strip_width;
			int y4 = y3 + strip_height;

			if (flat_in_rectangle(x, y, x3, y3, x4, y4))
			{
				if (this->GetEnabled())
				{
					frame_process_event(this, 0, pStrip->sName.CString(),
						FrameArgList(), NULL);
				}

				FlatWidget* pDelegator = this->GetDelegator();

				if (pDelegator)
				{
					pDelegator->OnPartSelect(this, (int)i, -1);
				}

				this->GetFlatUI()->RemoveFloating(this->GetUID());
				return 1;
			}
		}

		y3 += strip_height;
	}
	
	return 1;
}

int FlatMenu::OnPopup()
{
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "popup", FrameArgList(), NULL);
	}

	return 1;
}

void FlatMenu::SetStripHeight(int value)
{
	if (value < 8)
	{
		return;
	}
	
	m_nStripHeight = value;
}

int FlatMenu::GetStripHeight() const
{
	return m_nStripHeight;
}

void FlatMenu::SetDisableColor(const char* value)
{
	Assert(value != NULL);

	m_nDisableColor = flat_string_to_rgba(value);
}

ArgString FlatMenu::GetDisableColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nDisableColor, buffer, sizeof(buffer));
}

void FlatMenu::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatMenu::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

void FlatMenu::SetOwner(const uniqid_t& value)
{
	m_OwnerID = value;
}

uniqid_t FlatMenu::GetOwner() const
{
	return m_OwnerID;
}

void FlatMenu::AdjustSize()
{
	int height = 8;

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		strip_t* pStrip = m_Strips[i];

		if (pStrip->sText == "-")
		{
			height += m_nStripHeight / 2;
		}
		else
		{
			height += m_nStripHeight;
		}
	}

	this->SetHeight(height);
}

bool FlatMenu::AddStrip(const char* name, const char* text)
{
	Assert(name != NULL);
	Assert(text != NULL);
	
	strip_t* pStrip = K_NEW(strip_t);

	pStrip->sName = name;
	pStrip->sText = text;
	pStrip->bDisabled = false;
	pStrip->bCapture = false;
	m_Strips.PushBack(pStrip);
	this->AdjustSize();
	return true;
}

bool FlatMenu::RemoveStrip(const char* name)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		if (m_Strips[i]->sName == name)
		{
			m_Strips.Remove(i);
			this->AdjustSize();
			return true;
		}
	}
	
	return false;
}

bool FlatMenu::ClearStrips()
{
	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		K_DELETE(m_Strips[i]);
	}

	m_Strips.Clear();
	this->AdjustSize();
	return true;
}

bool FlatMenu::DisableStrip(const char* name)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		if (m_Strips[i]->sName == name)
		{
			m_Strips[i]->bDisabled = true;
			return true;
		}
	}

	return false;
}

bool FlatMenu::EnableStrip(const char* name)
{
	Assert(name != NULL);

	for (size_t i = 0; i < m_Strips.Size(); ++i)
	{
		if (m_Strips[i]->sName == name)
		{
			m_Strips[i]->bDisabled = false;
			return true;
		}
	}

	return false;
}
