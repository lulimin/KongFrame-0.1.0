// flat_window.h
// Created by lulimin on 2020/9/29.

#ifndef __FLAT_WINDOW_H
#define __FLAT_WINDOW_H

#include "../inc/frame_def.h"

// FlatWindow

class IRenderService;

class FlatWindow : public IObject
{
public:
	FlatWindow();
	virtual ~FlatWindow();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();
	// Execute per frame.
	virtual void Work(double frame_time);

	// Width of window.
	void SetWidth(int value);
	int GetWidth() const;

	// Height of window.
	void SetHeight(int value);
	int GetHeight() const;

	// Get initial window size.
	int GetInitialWidth();
	int GetInitialHeight();

	// Set color of background.
	bool SetBackColor(float red, float green, float blue, float alpha);
	// Initialize graphic renderer.
	bool StartRender();
	// Set user interface object id.
	bool SetUIObject(const uniqid_t& id);
	// Get user interface object id.
	uniqid_t GetUIObjectID() const;

private:
	IRenderService* m_pRenderService;
	int m_nWidth;
	int m_nHeight;
	float m_fBackColorRed;
	float m_fBackColorGreen;
	float m_fBackColorBlue;
	float m_fBackColorAlpha;
	uniqid_t m_UIObject;
};

#endif // __FLAT_WINDOW_H
