// flat_font.h
// Created by lulimin on 2020/12/14.

#ifndef __FLAT_FONT_H
#define __FLAT_FONT_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"

// FlatFont

struct flat_canvas_t;
struct flat_font_type_t;

class FlatFont : public IObject
{
public:
	FlatFont();
	virtual ~FlatFont();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Font name.
	void SetName(const char* value);
	const char* GetName() const;
	// Get hash value of name.
	unsigned int GetNameHash() const { return m_nNameHash; }

	// Font resource.
	void SetResource(const char* value);
	const char* GetResource() const;

	// Font image size.
	void SetImageSize(int value);
	int GetImageSize() const { return m_nImageSize; }

	// Font image group.
	void SetImageGroup(int value);
	int GetImageGroup() const { return m_nImageGroup; }

	// Character height.
	void SetCharHeight(int value);
	int GetCharHeight() const { return m_nCharHeight; }

	// Interval between character.
	void SetInterval(int value);
	int GetInterval() const { return m_nInterval; }

	// Fixed character width.
	void SetFixedWidth(bool value) { m_bFixedWidth = value; }
	bool GetFixedWidth() const { return m_bFixedWidth; }

	// Smooth style.
	void SetSmooth(bool value) { m_bSmooth = value; }
	bool GetSmooth() const { return m_bSmooth; }

	// Italic style.
	void SetItalic(bool value) { m_bItalic = value; }
	bool GetItalic() const { return m_bItalic; }

	// Bold style.
	void SetBold(bool value) { m_bBold = value; }
	bool GetBold() const { return m_bBold; }
	
	// Load resource.
	bool Load(flat_canvas_t* pCanvas);

	// Get font type.
	flat_font_type_t* GetFontType() const { return m_pFontType;  }

private:
	flat_font_type_t* m_pFontType;
	FrameString m_sName;
	unsigned int m_nNameHash;
	FrameString m_sResource;
	int m_nImageSize;
	int m_nImageGroup;
	int m_nCharHeight;
	int m_nInterval;
	bool m_bFixedWidth;
	bool m_bSmooth;
	bool m_bItalic;
	bool m_bBold;
};

#endif // __FLAT_FONT_H
