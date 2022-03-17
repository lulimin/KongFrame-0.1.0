// flat_check_box.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_CHECK_BOX_H
#define __FLAT_CHECK_BOX_H

#include "flat_widget.h"

// FlatCheckBox

class FlatCheckBox : public FlatWidget
{
public:
	FlatCheckBox();
	virtual ~FlatCheckBox();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnLeftDown(int x, int y, unsigned int flags);

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Checked image.
	void SetCheckedImage(const char* value);
	const char* GetCheckedImage() const;

	// Checked status.
	void SetChecked(bool value);
	bool GetChecked() const;

private:
	FrameString m_sText;
	FrameString m_sCheckedImage;
	flat_image_t* m_pCheckedImage;
	bool m_bChecked;
};

#endif // __FLAT_CHECK_BOX_H
