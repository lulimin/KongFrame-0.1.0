// flat_label.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_LABEL_H
#define __FLAT_LABEL_H

#include "flat_widget.h"
#include "../inc/frame_types.h"

// FlatLabel

class FlatLabel : public FlatWidget
{
public:
	FlatLabel();
	virtual ~FlatLabel();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Text align.
	void SetTextAlign(const char* value);
	const char* GetTextAlign() const;

	// Draw background.
	void SetBackground(bool value);
	bool GetBackground() const;

	// Draw border.
	void SetBorder(bool value);
	bool GetBorder() const;

private:
	FrameString m_sText;
	int m_nTextAlign;
	bool m_bBackground;
	bool m_bBorder;
};

#endif // __FLAT_LABEL_H
