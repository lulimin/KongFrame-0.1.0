// flat_main_menu.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_MAIN_MENU_H
#define __FLAT_MAIN_MENU_H

#include "flat_widget.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

// FlatMainMenu

class FlatMenu;

class FlatMainMenu : public FlatWidget
{
private:
	// Menu item.
	struct menu_t
	{
		FlatMenu* pMenu;
		FrameString sTitle;
		int nWidth;
		bool bCapture;
	};

public:
	FlatMainMenu();
	virtual ~FlatMainMenu();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnMouseMove(int x, int y, unsigned int flags);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// Captured color.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;

	// Create menu.
	uniqid_t CreateMenu(const char* name, const char* title, int width);
	// Remove menu.
	bool RemoveMenu(const char* name);
	// Clear all menus.
	bool ClearMenus();

private:
	unsigned int m_nCaptureColor;
	TPodArray<menu_t*, 1, FrameAlloc> m_Menus;
};

#endif // __FLAT_MAIN_MENU_H
