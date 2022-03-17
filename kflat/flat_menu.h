// flat_menu.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_MENU_H
#define __FLAT_MENU_H

#include "flat_widget.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

// FlatMenu

class FlatMenu : public FlatWidget
{
private:
	// Menu strip.
	struct strip_t
	{
		FrameString sName;
		FrameString sText;
		bool bDisabled;
		bool bCapture;
	};

public:
	FlatMenu();
	virtual ~FlatMenu();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnMouseMove(int x, int y, unsigned int flags);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnPopup();

	// Menu strip height.
	void SetStripHeight(int value);
	int GetStripHeight() const;

	// Color of disabled menu.
	void SetDisableColor(const char* value);
	ArgString GetDisableColor() const;

	// Captured color.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;

	// Owner widget id.
	void SetOwner(const uniqid_t& value);
	uniqid_t GetOwner() const;

	// Add menu strip.
	bool AddStrip(const char* name, const char* text);
	// Remove menu strip.
	bool RemoveStrip(const char* name);
	// Clear menu strips.
	bool ClearStrips();
	// Disable menu strip.
	bool DisableStrip(const char* name);
	// Enable menu strip.
	bool EnableStrip(const char* name);

private:
	// Adjust menu size.
	void AdjustSize();

private:
	int m_nStripHeight;
	unsigned int m_nDisableColor;
	unsigned int m_nCaptureColor;
	uniqid_t m_OwnerID;
	TPodArray<strip_t*, 1, FrameAlloc> m_Strips;
};

#endif // __FLAT_MENU_H
