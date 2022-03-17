// flat_window.cpp
// Created by lulimin on 2020/9/29.

#include "flat_window.h"
#include "../kgraphic/i_render_service.h"
#include "../kgraphic/i_renderable.h"

/// \file flat_window.cpp
/// \brief User interface window.

/// object: FlatWindow
/// \brief User interface window object.
DEF_OBJECT(FlatWindow, IObject);

/// readonly: object UIObject
/// \brief User interface object id.
DEF_READ_F(uniqid_t, FlatWindow, UIObject, GetUIObjectID);

/// property: int Width
/// \brief Width of window.
DEF_PROP(int, FlatWindow, Width);
/// property: int Height
/// \brief Height of window.
DEF_PROP(int, FlatWindow, Height);

/// function: int GetInitialWidth()
/// \brief Get initial width of window.
DEF_FUNC_0(int, FlatWindow, GetInitialWidth);
/// function: int GetInitialHeight()
/// \brief Get initial height of window.
DEF_FUNC_0(int, FlatWindow, GetInitialHeight);
/// function: bool SetBackColor(float red, float green, float blue, float alpha)
/// \brief Set color of background.
/// \param red,green,blue,alpha Color value.
DEF_FUNC_4(bool, FlatWindow, SetBackColor, float, float, float, float);
/// function: bool StartRender()
/// \brief Initialize renderer.
DEF_FUNC_0(bool, FlatWindow, StartRender);
/// function: bool SetUIObject(object id)
/// \brief Set user interface object id.
/// \param id UI object.
DEF_FUNC_1(bool, FlatWindow, SetUIObject, const uniqid_t&);

// FlatWindow

FlatWindow::FlatWindow()
{
	m_pRenderService = NULL;
	m_nWidth = 1024;
	m_nHeight = 768;
	m_fBackColorRed = 0.0F;
	m_fBackColorGreen = 0.0F;
	m_fBackColorBlue = 0.0F;
	m_fBackColorAlpha = 1.0F;
}

FlatWindow::~FlatWindow()
{
}

bool FlatWindow::Startup(const IArgList& args)
{
	m_pRenderService = (IRenderService*)frame_get_service("RenderService");

	if (NULL == m_pRenderService)
	{
		K_LOG("(FlatWindow::Startup)No render service.");
		return false;
	}
	
	frame_add_work(this);
	return true;
}

bool FlatWindow::Shutdown()
{
	frame_remove_work(this);
	SAFE_RELEASE(m_pRenderService);
	return true;
}

void FlatWindow::Work(double frame_time)
{
	m_pRenderService->CheckAssetThread();
	
	IRenderable* pUI = (IRenderable*)frame_get_object(m_UIObject);

	if (pUI)
	{
		pUI->Update((float)frame_time);
	}

	m_pRenderService->FrameBegin(m_fBackColorRed, m_fBackColorGreen, 
		m_fBackColorBlue, m_fBackColorAlpha);

	if (pUI)
	{
		pUI->Render((float)frame_time);
	}

	m_pRenderService->FrameEnd();
}

void FlatWindow::SetWidth(int value)
{ 
	m_nWidth = value;
}

int FlatWindow::GetWidth() const
{ 
	return m_nWidth;
}

void FlatWindow::SetHeight(int value)
{ 
	m_nHeight = value;
}

int FlatWindow::GetHeight() const
{ 
	return m_nHeight;
}

int FlatWindow::GetInitialWidth()
{
	Assert(m_pRenderService != NULL);

	return m_pRenderService->GetWindowWidth();
}

int FlatWindow::GetInitialHeight()
{
	Assert(m_pRenderService != NULL);

	return m_pRenderService->GetWindowHeight();
}

bool FlatWindow::SetBackColor(float red, float green, float blue, float alpha)
{
	m_fBackColorRed = red;
	m_fBackColorGreen = green;
	m_fBackColorBlue = blue;
	m_fBackColorAlpha = alpha;
	return true;
}

bool FlatWindow::StartRender()
{
	Assert(m_pRenderService != NULL);

	if (!m_pRenderService->StartRender(0, 1, m_nWidth, m_nHeight))
	{
		return false;
	}

	return true;
}

bool FlatWindow::SetUIObject(const uniqid_t& id)
{
	m_UIObject = id;
	return true;
}

uniqid_t FlatWindow::GetUIObjectID() const
{
	return m_UIObject;
}
