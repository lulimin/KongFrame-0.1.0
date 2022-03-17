// flat_cursor.cpp
// Created by lulimin on 2020/12/15.

#include "flat_cursor.h"

/// \file flat_cursor.cpp
/// \brief User interface cursor.

/// object: FlatCursor
/// \brief User interface cursor object.
DEF_OBJECT(FlatCursor, IObject);

/// property: string Name
/// \brief Cursor name.
DEF_PROP(const char*, FlatCursor, Name);
/// property: string Resource
/// \brief Cursor resource.
DEF_PROP(const char*, FlatCursor, Resource);

/// function: bool Load()
/// \brief Load cursor.
DEF_FUNC_0(bool, FlatCursor, Load);

#ifdef K_PLATFORM_WINDOWS
#include <windows.h>
#endif // K_PLATFORM_WINDOWS

// Flat cursor implementation.
struct flat_cursor_imp_t
{
#ifdef K_PLATFORM_WINDOWS
	HCURSOR hCursor;
#endif // K_PLATFORM_WINDOWS
};

// Show cursor.
bool flat_show_cursor()
{
#ifdef K_PLATFORM_WINDOWS
	ShowCursor(TRUE);
#endif // K_PLATFORM_WINDOWS
	return true;
}

// Hide cursor.
bool flat_hide_cursor()
{
#ifdef K_PLATFORM_WINDOWS
	ShowCursor(FALSE);
#endif // K_PLATFORM_WINDOWS
	return true;
}

// FlatCursor

FlatCursor::FlatCursor()
{
	m_nNameHash = 0;
#ifdef K_PLATFORM_WINDOWS
	m_pImp = K_NEW(flat_cursor_imp_t);
	m_pImp->hCursor = NULL;
#else
	m_pImp = NULL;
#endif // K_PLATFORM_WINDOWS
}

FlatCursor::~FlatCursor()
{
	if (m_pImp)
	{
		K_DELETE(m_pImp);
	}
}

bool FlatCursor::Startup(const IArgList& args)
{
	return true;
}

bool FlatCursor::Shutdown()
{
	return true;
}

void FlatCursor::SetName(const char* value)
{
	Assert(value != NULL);

	m_sName = value;
	m_nNameHash = util_string_hash(value);
}

const char* FlatCursor::GetName() const
{
	return m_sName.CString();
}

void FlatCursor::SetResource(const char* value)
{
	Assert(value != NULL);

	m_sResource = value;
}

const char* FlatCursor::GetResource() const
{
	return m_sResource.CString();
}

bool FlatCursor::Load()
{
#ifdef K_PLATFORM_WINDOWS
	if (m_sResource == "IDC_ARROW")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_ARROW);
	}
	else if (m_sResource == "IDC_IBEAM")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_IBEAM);
	}
	else if (m_sResource == "IDC_WAIT")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_WAIT);
	}
	else if (m_sResource == "IDC_HELP")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_HELP);
	}
	else if (m_sResource == "IDC_HAND")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_HAND);
	}
	else if (m_sResource == "IDC_SIZEWE")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_SIZEWE);
	}
	else if (m_sResource == "IDC_SIZENS")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_SIZENS);
	}
	else if (m_sResource == "IDC_SIZENWSE")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_SIZENWSE);
	}
	else if (m_sResource == "IDC_SIZENESW")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_SIZENESW);
	}
	else if (m_sResource == "IDC_SIZEALL")
	{
		m_pImp->hCursor = LoadCursor(NULL, IDC_SIZEALL);
	}
	else
	{
		char file_name[512];

		plat_safe_sprintf(file_name, sizeof(file_name), "%s%s",
			frame_get_asset_path(), m_sResource.CString());
		m_pImp->hCursor = LoadCursorFromFile(file_name);

		if (NULL == m_pImp->hCursor)
		{
			K_LOG("(FlatCursor::SetResource)Load cursor error.");
			K_LOG(file_name);
			return false;
		}
	}
#endif // K_PLATFORM_WINDOWS
	
	return true;
}

bool FlatCursor::Apply()
{
#ifdef K_PLATFORM_WINDOWS
	if (m_pImp->hCursor != NULL)
	{
		SetCursor(m_pImp->hCursor);
	}
#endif // K_PLATFORM_WINDOWS

	return true;
}
