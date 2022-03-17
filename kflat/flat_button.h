// flat_button.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_BUTTON_H
#define __FLAT_BUTTON_H

#include "flat_widget.h"
#include "../inc/frame_types.h"

// FlatButton

class FlatButton : public FlatWidget
{
public:
	// Styles of arrow button.
	enum
	{
		ARROW_NONE,
		ARROW_LEFT,
		ARROW_RIGHT,
		ARROW_UP,
		ARROW_DOWN,
	};

public:
	FlatButton();
	virtual ~FlatButton();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnClickReset();
	virtual int OnMouseHover(int x, int y);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftUp(int x, int y, unsigned int flags);
	virtual int OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);

	// Text on button.
	void SetText(const char* value);
	const char* GetText() const;

	// Image in captured status.
	void SetCaptureImage(const char* value);
	const char* GetCaptureImage() const;

	// Image in focused status.
	void SetFocusedImage(const char* value);
	const char* GetFocusedImage() const;

	// Image in pressed status.
	void SetPressImage(const char* value);
	const char* GetPressImage() const;

	// Image in grey status.
	void SetGreyImage(const char* value);
	const char* GetGreyImage() const;

	// Color in captured.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;

	// Color in focused.
	void SetFocusedColor(const char* value);
	ArgString GetFocusedColor() const;

	// Color in pressed.
	void SetPressColor(const char* value);
	ArgString GetPressColor() const;

	// Color in grey.
	void SetGreyColor(const char* value);
	ArgString GetGreyColor() const;

	// Generate click event when mouse down.
	void SetPushClick(bool value);
	bool GetPushClick() const;

	// Generate click event when mouse hover.
	void SetHoverClick(bool value);
	bool GetHoverClick() const;

	// Set arrow type.
	void SetArrowType(int value);

private:
	FrameString m_sText;
	FrameString m_sCaptureImage;
	FrameString m_sFocusedImage;
	FrameString m_sPressImage;
	FrameString m_sGreyImage;
	flat_image_t* m_pCaptureImage;
	flat_image_t* m_pFocusedImage;
	flat_image_t* m_pPressImage;
	flat_image_t* m_pGreyImage;
	unsigned int m_nCaptureColor;
	unsigned int m_nFocusedColor;
	unsigned int m_nPressColor;
	unsigned int m_nGreyColor;
	bool m_bPushClick;
	bool m_bHoverClick;
	bool m_bPressed;
	int m_nArrowType;
	int m_nClickX;
	int m_nClickY;
};

#endif // __FLAT_BUTTON_H
