// flat_picker.cpp
// Created by lulimin on 2021/1/15.

#include "flat_picker.h"
//#include "flat_pick_group.h"
#include "flat_button.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_picker.cpp
/// \brief User interface picker.

/// object: FlatPicker
/// \brief User interface picker object.
DEF_OBJECT(FlatPicker, FlatWidget);

/// readonly: LeftEdge
/// \brief Left edge widget.
DEF_READ(uniqid_t, FlatPicker, LeftEdge);
/// readonly: RightEdge
/// \brief Right edge widget.
DEF_READ(uniqid_t, FlatPicker, RightEdge);
/// readonly: UpEdge
/// \brief Up edge widget.
DEF_READ(uniqid_t, FlatPicker, UpEdge);
/// readonly: DownEdge
/// \brief Down edge widget.
DEF_READ(uniqid_t, FlatPicker, DownEdge);
/// readonly: LeftUpEdge
/// \brief Left-up corner widget.
DEF_READ(uniqid_t, FlatPicker, LeftUpEdge);
/// readonly: LeftDownEdge
/// \brief Left-down corner widget.
DEF_READ(uniqid_t, FlatPicker, LeftDownEdge);
/// readonly: RightUpEdge
/// \brief Right-up corner widget.
DEF_READ(uniqid_t, FlatPicker, RightUpEdge);
/// readonly: RightDownEdge
/// \brief Right-down corner widget.
DEF_READ(uniqid_t, FlatPicker, RightDownEdge);

/// property: int BorderSize
/// \brief Border size.
DEF_PROP(int, FlatPicker, BorderSize);
/// property: int StepSize
/// \brief Move stop size.
DEF_PROP(int, FlatPicker, StepSize);
/// property: bool Actived
/// \brief Actived status.
DEF_PROP(bool, FlatPicker, Actived);
/// property: object Target
/// \brief Target widget id.
DEF_PROP(uniqid_t, FlatPicker, Target);

// FlatPicker

FlatPicker::FlatPicker()
{
	m_nBorderSize = 8;
	m_nStepSize = 8;
	m_bActived = false;
	m_pLeftEdge = NULL;
	m_pRightEdge = NULL;
	m_pUpEdge = NULL;
	m_pDownEdge = NULL;
	m_pLeftUpEdge = NULL;
	m_pLeftDownEdge = NULL;
	m_pRightUpEdge = NULL;
	m_pRightDownEdge = NULL;
	m_nClickX = -1;
	m_nClickY = -1;
}

FlatPicker::~FlatPicker()
{
}

bool FlatPicker::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();

	FlatUI* pFlatUI = this->GetFlatUI();

	m_pLeftEdge = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pRightEdge = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pUpEdge = (FlatButton*)pFlatUI->CreatePartWidget(this,"FlatButton");
	m_pDownEdge = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pLeftUpEdge = (FlatButton*)pFlatUI->CreatePartWidget(this, "FlatButton");
	m_pLeftDownEdge = (FlatButton*)pFlatUI->CreatePartWidget(this,
		"FlatButton");
	m_pRightUpEdge = (FlatButton*)pFlatUI->CreatePartWidget(this,
		"FlatButton");
	m_pRightDownEdge = (FlatButton*)pFlatUI->CreatePartWidget(this, 
		"FlatButton");
	return true;
}

bool FlatPicker::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatPicker::Draw(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	if (!this->GetVisible())
	{
		return;
	}

	int width = this->GetWidth();
	int height = this->GetHeight();
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int x2 = x1 + width;
	int y2 = y1 + height;

	canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	
	if (m_bActived)
	{
		m_pLeftEdge->Draw(pCanvas);
		m_pRightEdge->Draw(pCanvas);
		m_pUpEdge->Draw(pCanvas);
		m_pDownEdge->Draw(pCanvas);
		m_pLeftUpEdge->Draw(pCanvas);
		m_pLeftDownEdge->Draw(pCanvas);
		m_pRightUpEdge->Draw(pCanvas);
		m_pRightDownEdge->Draw(pCanvas);
	}
}

FlatWidget* FlatPicker::GetRegionOf(int x, int y)
{
	if (!this->GetVisible())
	{
		return NULL;
	}

	if (m_bActived)
	{
		FlatWidget* pWidget = this->InPartRegion(x, y);

		if (pWidget)
		{
			return pWidget;
		}
	}

	if (!this->InSelfRegion(x, y))
	{
		// Not in region of widget.
		return NULL;
	}

	return this;
}

int FlatPicker::OnClickReset()
{
	return 0;
}

int FlatPicker::OnLeftDown(int x, int y, unsigned int flags)
{
	m_nClickX = x - this->GetGlobalLeft();
	m_nClickY = y - this->GetGlobalTop();
	return 0;
}

int FlatPicker::OnLeftUp(int x, int y, unsigned int flags)
{
	return 0;
}

static inline int get_step(int step_size, unsigned int flags)
{
	if (flags & IInputService::FLAG_CONTROL)
	{
		return 1;
	}

	return step_size;
}

int FlatPicker::OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags)
{
	if (!m_bActived)
	{
		return 0;
	}
	
	FlatWidget* pTarget = flat_get_widget(m_TargetID);

	if (NULL == pTarget)
	{
		return 0;
	}

	FlatWidget* pTargetParent = pTarget->GetParent();

	if (NULL == pTargetParent)
	{
		return 0;
	}

	int parent_left = pTargetParent->GetGlobalLeft();
	int parent_top = pTargetParent->GetGlobalTop();
	int parent_width = pTargetParent->GetWidth();
	int parent_height = pTargetParent->GetHeight();
	int beg_gx = parent_left - m_nClickX;
	int beg_gy = parent_top - m_nClickY;
	int end_gx = beg_gx + parent_width;
	int end_gy = beg_gy + parent_height;
	int start_gx = gcx - m_nClickX;
	int start_gy = gcy - m_nClickY;
	int track_gx = x - gcx + start_gx;
	int track_gy = y - gcy + start_gy;

	if (track_gx < beg_gx)
	{
		track_gx = beg_gx;
	}

	if (track_gx > end_gx)
	{
		track_gx = end_gx;
	}

	if (track_gy < beg_gy)
	{
		track_gy = beg_gy;
	}

	if (track_gy > end_gy)
	{
		track_gy = end_gy;
	}

	int step = get_step(m_nStepSize, flags);

	if (step > 1)
	{
		int parent_gx = pTargetParent->GetGlobalLeft();
		int parent_gy = pTargetParent->GetGlobalTop();
		int local_x = (track_gx - parent_gx) / step * step;
		int local_y = (track_gy - parent_gy) / step * step;

		track_gx = parent_gx + local_x;
		track_gy = parent_gy + local_y;
	}

	this->SetGlobalLeftTop(track_gx, track_gy);
	pTarget->SetGlobalLeftTop(track_gx, track_gy);

	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}

	return 1;
}

int FlatPicker::OnKeyDown(unsigned int key, unsigned int flags)
{
	return 0;
}

int FlatPicker::OnSizeChanged()
{
	this->PerformLayout();
	return 1;
}

int FlatPicker::OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
	unsigned int flags)
{
	Assert(pPart != NULL);
	
	bool changed = false;
	
	if (pPart == m_pLeftEdge)
	{
		changed = this->DragLeft(m_pLeftEdge, x, cx, flags);
	}
	else if (pPart == m_pRightEdge)
	{
		changed = this->DragRight(m_pRightEdge, x, cx, flags);
	}
	else if (pPart == m_pUpEdge)
	{
		changed = this->DragUp(m_pUpEdge, y, cy, flags);
	}
	else if (pPart == m_pDownEdge)
	{
		changed = this->DragDown(m_pDownEdge, y, cy, flags);
	}
	else if (pPart == m_pLeftUpEdge)
	{
		bool drag_left = this->DragLeft(m_pLeftUpEdge, x, cx, flags);
		bool drag_up = this->DragUp(m_pLeftUpEdge, y, cy, flags);

		changed = drag_left || drag_up;
	}
	else if (pPart == m_pLeftDownEdge)
	{
		bool drag_left = this->DragLeft(m_pLeftDownEdge, x, cx, flags);
		bool drag_down = this->DragDown(m_pLeftDownEdge, y, cy, flags);

		changed = drag_left || drag_down;
	}
	else if (pPart == m_pRightUpEdge)
	{
		bool drag_right = this->DragRight(m_pRightUpEdge, x, cx, flags);
		bool drag_up = this->DragUp(m_pRightUpEdge, y, cy, flags);

		changed = drag_right || drag_up;
	}
	else if (pPart == m_pRightDownEdge)
	{
		bool drag_right = this->DragRight(m_pRightDownEdge, x, cx, flags);
		bool drag_down = this->DragDown(m_pRightDownEdge, y, cy, flags);

		changed = drag_right || drag_down;
	}

	if (changed && this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}

	return 1;
}

bool FlatPicker::DragLeft(FlatButton* pEdge, int x, int cx, unsigned int flags)
{
	Assert(pEdge != NULL);

	FlatWidget* pTarget = flat_get_widget(m_TargetID);

	if (NULL == pTarget)
	{
		return false;
	}

	FlatWidget* pTargetParent = pTarget->GetParent();

	if (NULL == pTargetParent)
	{
		return false;
	}

	int new_x = x - pEdge->GetGlobalLeft();
	int new_width = this->GetWidth() + cx - new_x;

	if ((pTarget->GetLeft() + pTarget->GetWidth() - new_width) < 0)
	{
		new_width = pTarget->GetLeft() + pTarget->GetWidth();
	}

	int step = get_step(m_nStepSize, flags);

	if (step > 1)
	{
		new_width = new_width / step * step;
	}

	if (new_width < pTarget->GetMinimumWidth())
	{
		new_width = pTarget->GetMinimumWidth();
	}

	int new_left = this->GetLeft() + this->GetWidth() - new_width;

	this->SetWidth(new_width);
	this->SetLeft(new_left);
	pTarget->SetWidth(new_width);
	pTarget->SetGlobalLeft(this->GetGlobalLeft());
	return true;
}

bool FlatPicker::DragRight(FlatButton* pEdge, int x, int cx, unsigned int flags)
{
	Assert(pEdge != NULL);

	FlatWidget* pTarget = flat_get_widget(m_TargetID);

	if (NULL == pTarget)
	{
		return false;
	}

	FlatWidget* pTargetParent = pTarget->GetParent();

	if (NULL == pTargetParent)
	{
		return false;
	}

	int new_x = x - pEdge->GetGlobalLeft();
	int new_width = this->GetWidth() + new_x - cx;

	if ((pTarget->GetLeft() + new_width) > pTargetParent->GetWidth())
	{
		new_width = pTargetParent->GetWidth() - pTarget->GetLeft();
	}

	int step = get_step(m_nStepSize, flags);

	if (step > 1)
	{
		new_width = new_width / step * step;
	}

	if (new_width < pTarget->GetMinimumWidth())
	{
		new_width = pTarget->GetMinimumWidth();
	}

	this->SetWidth(new_width);
	pTarget->SetWidth(new_width);
	return true;
}

bool FlatPicker::DragUp(FlatButton* pEdge, int y, int cy, unsigned int flags)
{
	Assert(pEdge != NULL);

	FlatWidget* pTarget = flat_get_widget(m_TargetID);

	if (NULL == pTarget)
	{
		return false;
	}

	FlatWidget* pTargetParent = pTarget->GetParent();

	if (NULL == pTargetParent)
	{
		return false;
	}

	int new_y = y - pEdge->GetGlobalTop();
	int new_height = this->GetHeight() + cy - new_y;

	if ((pTarget->GetTop() + pTarget->GetHeight() - new_height) < 0)
	{
		new_height = pTarget->GetTop() + pTarget->GetHeight();
	}

	int step = get_step(m_nStepSize, flags);

	if (step > 1)
	{
		new_height = new_height / step * step;
	}

	if (new_height < pTarget->GetMinimumHeight())
	{
		new_height = pTarget->GetMinimumHeight();
	}

	int new_top = this->GetTop() + this->GetHeight() - new_height;

	this->SetHeight(new_height);
	this->SetTop(new_top);
	pTarget->SetHeight(new_height);
	pTarget->SetGlobalTop(this->GetGlobalTop());
	return true;
}

bool FlatPicker::DragDown(FlatButton* pEdge, int y, int cy, unsigned int flags)
{
	Assert(pEdge != NULL);

	FlatWidget* pTarget = flat_get_widget(m_TargetID);

	if (NULL == pTarget)
	{
		return false;
	}

	FlatWidget* pTargetParent = pTarget->GetParent();

	if (NULL == pTargetParent)
	{
		return false;
	}

	int new_y = y - pEdge->GetGlobalTop();
	int new_height = this->GetHeight() + new_y - cy;

	if ((pTarget->GetTop() + new_height) > pTargetParent->GetHeight())
	{
		new_height = pTargetParent->GetHeight() - pTarget->GetTop();
	}
	
	int step = get_step(m_nStepSize, flags);

	if (step > 1)
	{
		new_height = new_height / step * step;
	}

	if (new_height < pTarget->GetMinimumHeight())
	{
		new_height = pTarget->GetMinimumHeight();
	}

	this->SetHeight(new_height);
	pTarget->SetHeight(new_height);
	return true;
}

void FlatPicker::SetBorderSize(int value)
{
	if (value < 1)
	{
		return;
	}
	
	m_nBorderSize = value;
}

int FlatPicker::GetBorderSize() const
{
	return m_nBorderSize;
}

void FlatPicker::SetStepSize(int value)
{
	if (value < 1)
	{
		return;
	}

	m_nStepSize = value;
}

int FlatPicker::GetStepSize() const
{
	return m_nStepSize;
}

void FlatPicker::SetActived(bool value)
{
	m_bActived = value;
}

bool FlatPicker::GetActived() const
{
	return m_bActived;
}

void FlatPicker::SetTarget(const uniqid_t& value)
{
	FlatWidget* pWidget = flat_get_widget(value);

	if (NULL == pWidget)
	{
		return;
	}

	m_TargetID = value;
}

uniqid_t FlatPicker::GetTarget() const
{
	return m_TargetID;
}

uniqid_t FlatPicker::GetLeftEdge() const
{
	return m_pLeftEdge->GetUID();
}

uniqid_t FlatPicker::GetRightEdge() const
{
	return m_pRightEdge->GetUID();
}

uniqid_t FlatPicker::GetUpEdge() const
{
	return m_pUpEdge->GetUID();
}

uniqid_t FlatPicker::GetDownEdge() const
{
	return m_pDownEdge->GetUID();
}

uniqid_t FlatPicker::GetLeftUpEdge() const
{
	return m_pLeftUpEdge->GetUID();
}

uniqid_t FlatPicker::GetLeftDownEdge() const
{
	return m_pLeftDownEdge->GetUID();
}

uniqid_t FlatPicker::GetRightUpEdge() const
{
	return m_pRightUpEdge->GetUID();
}

uniqid_t FlatPicker::GetRightDownEdge() const
{
	return m_pRightDownEdge->GetUID();
}

void FlatPicker::PerformLayout()
{
	int edge_size = m_nBorderSize;

	m_pLeftEdge->SetLeft(-edge_size);
	m_pLeftEdge->SetTop((this->GetHeight() - edge_size) / 2);
	m_pLeftEdge->SetSize(edge_size, edge_size);
	m_pRightEdge->SetLeft(this->GetWidth());
	m_pRightEdge->SetTop((this->GetHeight() - edge_size) / 2);
	m_pRightEdge->SetSize(edge_size, edge_size);
	m_pUpEdge->SetLeft((this->GetWidth() - edge_size) / 2);
	m_pUpEdge->SetTop(-edge_size);
	m_pUpEdge->SetSize(edge_size, edge_size);
	m_pDownEdge->SetLeft((this->GetWidth() - edge_size) / 2);
	m_pDownEdge->SetTop(this->GetHeight());
	m_pDownEdge->SetSize(edge_size, edge_size);
	m_pLeftUpEdge->SetLeft(-edge_size);
	m_pLeftUpEdge->SetTop(-edge_size);
	m_pLeftUpEdge->SetSize(edge_size, edge_size);
	m_pLeftDownEdge->SetLeft(-edge_size);
	m_pLeftDownEdge->SetTop(this->GetHeight());
	m_pLeftDownEdge->SetSize(edge_size, edge_size);
	m_pRightUpEdge->SetLeft(this->GetWidth());
	m_pRightUpEdge->SetTop(-edge_size);
	m_pRightUpEdge->SetSize(edge_size, edge_size);
	m_pRightDownEdge->SetLeft(this->GetWidth());
	m_pRightDownEdge->SetTop(this->GetHeight());
	m_pRightDownEdge->SetSize(edge_size, edge_size);
}
