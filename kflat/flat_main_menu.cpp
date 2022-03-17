// flat_main_menu.cpp
// Created by lulimin on 2020/9/11.

#include "flat_main_menu.h"
#include "flat_menu.h"
#include "flat_ui.h"

/// \file flat_main_menu.cpp
/// \brief User interface main menu.

/// object: FlatMainMenu
/// \brief User interface main menu object.
DEF_OBJECT(FlatMainMenu, FlatWidget);

/// property: string CaptureColor
/// \brief Color when mouse captured.
DEF_PROP(ArgString, FlatMainMenu, CaptureColor);

/// function: bool CreateMenu(string name, string title, int width)
/// \brief Create menu.
/// \param name Menu name.
/// \param title Menu title.
/// \param width Menu width.
DEF_FUNC_3(uniqid_t, FlatMainMenu, CreateMenu, const char*, const char*, int);
/// function: bool RemoveMenu(string name)
/// \brief Remove menu.
/// \param name Menu name.
DEF_FUNC_1(bool, FlatMainMenu, RemoveMenu, const char*);
/// function: bool ClearMenus()
/// \brief Clear all menus.
DEF_FUNC_0(bool, FlatMainMenu, ClearMenus);

#define INIT_CAPTURE_COLOR 0xFF0000FF

// FlatMainMenu

FlatMainMenu::FlatMainMenu()
{
	m_nCaptureColor = INIT_CAPTURE_COLOR;
}

FlatMainMenu::~FlatMainMenu()
{
	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		K_DELETE(m_Menus[i]);
	}
}

bool FlatMainMenu::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	return true;
}

bool FlatMainMenu::Shutdown()
{
	this->ClearMenus();
	return FlatWidget::Shutdown();
}

void FlatMainMenu::Draw(flat_canvas_t* pCanvas)
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
	}

	this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());

	int x3 = x1;

	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		menu_t* pData = m_Menus[i];
		FlatMenu* pMenu = pData->pMenu;
		int x4 = x3 + pData->nWidth;

		if (this->GetCapture() && pData->bCapture)
		{
			canvas_draw_block(pCanvas, x3, y1, x4, y2, m_nCaptureColor);
		}

		canvas_draw_text_center(pCanvas, x3, y1, x4, y2,
			this->GetForeColorValue(), pData->sTitle.CString(), 0, 0);
		x3 += pData->nWidth;
	}
}

void FlatMainMenu::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	char buffer[32];
	FrameArgData def_capture_color(DT_STRING,
		flat_rgba_to_string(INIT_CAPTURE_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
}

int FlatMainMenu::OnMouseMove(int x, int y, unsigned int flags)
{
	FlatUI* pFlatUI = this->GetFlatUI();
	int height = this->GetHeight();
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int y2 = y1 + height;
	int x3 = x1;
	bool menu_poped = false;

	for (size_t k = 0; k < m_Menus.Size(); ++k)
	{
		menu_t* pData = m_Menus[k];

		if (pFlatUI->FindFloating(pData->pMenu->GetUID()))
		{
			menu_poped = true;
			break;
		}
	}

	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		menu_t* pData = m_Menus[i];
		FlatMenu* pMenu = pData->pMenu;
		uniqid_t menu_id = pMenu->GetUID();
		int x4 = x3 + pData->nWidth;

		if (flat_in_rectangle(x, y, x3, y1, x4, y2))
		{
			pData->bCapture = true;
			
			if (menu_poped)
			{
				if (!pFlatUI->FindFloating(menu_id))
				{
					pMenu->SetGlobalLeftTop(x3, y2);
					pFlatUI->AddFloating(menu_id);
				}
			}
		}
		else
		{
			pData->bCapture = false;

			if (menu_poped)
			{
				pFlatUI->RemoveFloating(menu_id);
			}
		}

		x3 += pData->nWidth;
	}
	
	return 1;
}

int FlatMainMenu::OnLeftDown(int x, int y, unsigned int flags)
{
	FlatUI* pFlatUI = this->GetFlatUI();
	int height = this->GetHeight();
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int y2 = y1 + height;
	int x3 = x1;

	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		menu_t* pData = m_Menus[i];
		FlatMenu* pMenu = pData->pMenu;
		uniqid_t menu_id = pMenu->GetUID();
		int x4 = x3 + pData->nWidth;

		if (flat_in_rectangle(x, y, x3, y1, x4, y2))
		{
			if (pFlatUI->FindFloating(menu_id))
			{
				pFlatUI->RemoveFloating(menu_id);
			}
			else
			{
				pMenu->SetGlobalLeftTop(x3, y2);
				pFlatUI->AddFloating(menu_id);
			}
		}
		else
		{
			pFlatUI->RemoveFloating(menu_id);
		}

		x3 += pData->nWidth;
	}

	return 1;
}

int FlatMainMenu::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	this->GetFlatUI()->RemoveFloating(pPart->GetUID());
	return 1;
}

void FlatMainMenu::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatMainMenu::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

uniqid_t FlatMainMenu::CreateMenu(const char* name, const char* title,
	int width)
{
	Assert(name != NULL);
	Assert(title != NULL);

	if (width < 8)
	{
		return uniqid_t();
	}

	FlatMenu* pMenu = (FlatMenu*)this->GetFlatUI()->CreateWidget("FlatMenu");

	pMenu->SetName(name);
	pMenu->SetDelegator(this);

	menu_t* pData = K_NEW(menu_t);

	pData->pMenu = pMenu;
	pData->sTitle = title;
	pData->nWidth = width;
	pData->bCapture = false;
	m_Menus.PushBack(pData);
	return pMenu->GetUID();
}

bool FlatMainMenu::RemoveMenu(const char* name)
{
	Assert(name != NULL);
	
	unsigned int hash = util_string_hash(name);

	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		menu_t* pData = m_Menus[i];
		FlatMenu* pMenu = pData->pMenu;

		if ((pMenu->GetNameHash() == hash) &&
			(strcmp(pMenu->GetName(), name) == 0))
		{
			pMenu->Release();
			K_DELETE(pData);
			m_Menus.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatMainMenu::ClearMenus()
{
	for (size_t i = 0; i < m_Menus.Size(); ++i)
	{
		menu_t* pData = m_Menus[i];

		pData->pMenu->Release();
		K_DELETE(pData);
	}
	
	m_Menus.Clear();
	return true;
}
