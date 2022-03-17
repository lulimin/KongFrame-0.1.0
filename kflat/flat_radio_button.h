// flat_radio_button.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_RADIO_BUTTON_H
#define __FLAT_RADIO_BUTTON_H

#include "flat_widget.h"

// FlatRadioButton

class FlatRadioButton : public FlatWidget
{
public:
	FlatRadioButton();
	virtual ~FlatRadioButton();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Cllbacks.
	virtual int OnLeftDown(int x, int y, unsigned int flags);

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Checked image.
	void SetCheckedImage(const char* value);
	const char* GetCheckedImage() const;

	// Get check status.
	bool GetChecked() const;

	// Set checked status.
	bool SetCheckedValue();
	// Clear checked status.
	bool ClearCheckedValue();

private:
	FrameString m_sText;
	FrameString m_sCheckedImage;
	flat_image_t* m_pCheckedImage;
	bool m_bChecked;
};

#endif // __FLAT_RADIO_BUTTON_H
