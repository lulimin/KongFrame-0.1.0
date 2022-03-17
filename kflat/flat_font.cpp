// flat_font.cpp
// Created by lulimin on 2020/12/14.

#include "flat_font.h"
#include "flat_canvas.h"
#include "../inc/frame_mem.h"

/// \file flat_font.cpp
/// \brief User interface font.

/// object: FlatFont
/// \brief User interface font object.
DEF_OBJECT(FlatFont, IObject);

/// property: string Name
/// \brief Cursor name.
DEF_PROP(const char*, FlatFont, Name);
/// property: string Resource
/// \brief Cursor resource.
DEF_PROP(const char*, FlatFont, Resource);
/// property: int ImageSize
/// \brief Font image size.
DEF_PROP(int, FlatFont, ImageSize);
/// property: int ImageGroup
/// \brief Font image group.
DEF_PROP(int, FlatFont, ImageGroup);
/// property: int CharHeight
/// \brief Character height.
DEF_PROP(int, FlatFont, CharHeight);
/// property: int Interval
/// \brief Interval between characters.
DEF_PROP(int, FlatFont, Interval);
/// property: bool FixedWidth
/// \brief Fixed character width.
DEF_PROP(bool, FlatFont, FixedWidth);
/// property: bool Smooth
/// \brief Smooth style.
DEF_PROP(bool, FlatFont, Smooth);
/// property: bool Italic
/// \brief Italic style.
DEF_PROP(bool, FlatFont, Italic);
/// property: bool Bold
/// \brief Bold style.
DEF_PROP(bool, FlatFont, Bold);

// FlatFont

FlatFont::FlatFont()
{
	m_pFontType = NULL;
	m_nNameHash = 0;
	m_nImageSize = 512;
	m_nImageGroup = 0;
	m_nCharHeight = 12;
	m_nInterval = 0;
	m_bFixedWidth = true;
	m_bSmooth = false;
	m_bItalic = false;
	m_bBold = false;
}

FlatFont::~FlatFont()
{
}

bool FlatFont::Startup(const IArgList& args)
{
	return true;
}

bool FlatFont::Shutdown()
{
	return true;
}

void FlatFont::SetName(const char* value)
{
	Assert(value != NULL);

	m_sName = value;
	m_nNameHash = util_string_hash(value);
}

const char* FlatFont::GetName() const
{
	return m_sName.CString();
}

void FlatFont::SetResource(const char* value)
{
	Assert(value != NULL);

	m_sResource = value;
}

const char* FlatFont::GetResource() const
{
	return m_sResource.CString();
}

void FlatFont::SetImageSize(int value)
{
	if ((value != 256) && (value != 512) && (value != 1024) && (value != 2048))
	{
		return;
	}

	m_nImageSize = value;
}

void FlatFont::SetImageGroup(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nImageGroup = value;
}

void FlatFont::SetCharHeight(int value)
{
	if ((value < 8) || (value > 128))
	{
		return;
	}

	m_nCharHeight = value;
}

void FlatFont::SetInterval(int value)
{
	if ((value < 0) || (value > 100))
	{
		return;
	}

	m_nInterval = value;
}

bool FlatFont::Load(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	flat_font_desc_t desc;

	desc.pName = m_sName.CString();
	desc.pResource = m_sResource.CString();
	desc.nImageSize = m_nImageSize;
	desc.nImageGroup = m_nImageGroup;
	desc.nCharHeight = m_nCharHeight;
	desc.nInterval = m_nInterval;
	desc.bFixedWidth = m_bFixedWidth;
	desc.bSmooth = m_bSmooth;
	desc.bItalic = m_bItalic;
	desc.bBold = m_bBold;
	m_pFontType = canvas_add_font(pCanvas, desc);
	return m_pFontType != NULL;
}
