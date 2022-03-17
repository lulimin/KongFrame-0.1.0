// flat_form.cpp
// Created by lulimin on 2020/9/11.

#include "flat_form.h"
#include "flat_ui.h"
#include "../inc/auto_buffer.h"

/// \file flat_form.cpp
/// \brief User interface form.

/// object: FlatForm
/// \brief User interface form object.
DEF_OBJECT(FlatForm, FlatWidget);

/// readonly: bool DialogMode
/// \brief In dialog mode.
DEF_READ(bool, FlatForm, DialogMode);

/// property: bool Transparent
/// \brief Transparent mode.
DEF_PROP(bool, FlatForm, Transparent);
/// property: int StepSize
/// \brief Step size in design mode.
DEF_PROP(int, FlatForm, StepSize);

/// function: bool AddChild(object id)
/// \brief Add child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatForm, AddChild, const uniqid_t&);
/// function: bool RemoveChild(object id)
/// \brief Remove child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatForm, RemoveChild, const uniqid_t&);
/// function: bool ClearChildren()
/// \brief Clear child widgets.
DEF_FUNC_0(bool, FlatForm, ClearChildren);
/// function [children table] GetChildList()
/// \brief Get children widget id list.
DEF_FUNC_A(FlatForm, GetChildList);

/// function: bool Show()
/// \brief Show form.
DEF_FUNC_0(bool, FlatForm, Show);
/// function: bool ShowDialog()
/// \brief Show form in dialog mode.
DEF_FUNC_0(bool, FlatForm, ShowDialog);
/// function: bool Close()
/// \brief Close form.
DEF_FUNC_0(bool, FlatForm, Close);

// FlatForm

FlatForm::FlatForm()
{
	m_bDialogMode = false;
	m_bTransparent = false;
	m_nStepSize = 0;
}

FlatForm::~FlatForm()
{
}

bool FlatForm::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	this->CreateContainer();
	return true;
}

bool FlatForm::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatForm::Draw(flat_canvas_t* pCanvas)
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

	if (canvas_set_range(pCanvas, x1, y1, width, height))
	{
		if (!m_bTransparent)
		{
			if (this->ExistBackImage())
			{
				this->DrawBackImage(pCanvas, x1, y1, x2, y2);
			}
			else
			{
				canvas_draw_block(pCanvas, x1, y1, x2, y2,
					this->GetBackColorValue());
			}
		}

		if (this->GetDesignMode() && (m_nStepSize > 0))
		{
			int rows = this->GetHeight() / m_nStepSize;
			int columns = this->GetWidth() / m_nStepSize;
			int lines = rows + columns;
			TAutoBuffer<int, 1024, FrameAlloc> auto_buf(lines * 4);
			int* coords = auto_buf.GetBuffer();
			int y3 = y1 + m_nStepSize;

			for (int r = 0; r < rows; ++r)
			{
				coords[0] = x1;
				coords[1] = y3;
				coords[2] = x2;
				coords[3] = y3;
				coords += 4;
				y3 += m_nStepSize;
			}

			int x3 = x1 + m_nStepSize;

			for (int c = 0; c < columns; ++c)
			{
				coords[0] = x3;
				coords[1] = y1;
				coords[2] = x3;
				coords[3] = y2;
				coords += 4;
				x3 += m_nStepSize;
			}

			if (lines > 0)
			{
				canvas_draw_lines(pCanvas, auto_buf.GetBuffer(), lines,
					0xFF303030);
			}
		}

		// Draw children widgets.
		flat_container_t* pContainer = this->GetContainer();

		Assert(pContainer != NULL);

		size_t widget_size = flat_container_get_size(pContainer);
		FlatWidget** pWidgets = flat_container_get_data(pContainer);

		for (size_t i = 0; i < widget_size; ++i)
		{
			FlatWidget* pWidget = pWidgets[i];

			if (NULL == pWidget)
			{
				continue;
			}

			pWidget->Draw(pCanvas);
		}

		if (!m_bTransparent)
		{
			if (!this->ExistBackImage())
			{
				canvas_draw_frame(pCanvas, x1, y1, x2, y2,
					this->GetForeColorValue());
			}
		}

		canvas_reset_range(pCanvas);
	}
}

void FlatForm::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);

	flat_metadata_add_prop(pMD, "Transparent", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_event(pMD, "show");
}

bool FlatForm::GetDialogMode() const
{
	return m_bDialogMode;
}

void FlatForm::SetTransparent(bool value)
{ 
	m_bTransparent = value;
}

bool FlatForm::GetTransparent() const
{ 
	return m_bTransparent;
}

void FlatForm::SetStepSize(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nStepSize = value;
}

int FlatForm::GetStepSize() const
{
	return m_nStepSize;
}

bool FlatForm::AddChild(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}

	flat_container_t* pContainer = this->GetContainer();

	Assert(pContainer != NULL);

	return flat_container_add(pContainer, pWidget);
}

bool FlatForm::RemoveChild(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}

	flat_container_t* pContainer = this->GetContainer();

	Assert(pContainer != NULL);
	
	return flat_container_remove(pContainer, pWidget);
}

bool FlatForm::ClearChildren()
{
	flat_container_t* pContainer = this->GetContainer();

	Assert(pContainer != NULL);

	flat_container_erase_all(pContainer);
	return true;
}

void FlatForm::GetChildList(const IArgList& args, IArgList* res)
{
	res->Clear();

	flat_container_t* pContainer = this->GetContainer();
	size_t widget_size = flat_container_get_size(pContainer);
	FlatWidget** pWidgets = flat_container_get_data(pContainer);

	for (size_t i = 0; i < widget_size; ++i)
	{
		FlatWidget* pWidget = pWidgets[i];

		if (NULL == pWidget)
		{
			continue;
		}

		res->AddUniqid(pWidget->GetUID());
	}
}

bool FlatForm::Show()
{
	if (this->GetParent() != NULL)
	{
		return false;
	}

	FlatUI* pFlatUI = this->GetFlatUI();
	FlatWidget* pTop = pFlatUI->GetTopDialog();

	if (NULL == pTop)
	{
		return false;
	}

	m_bDialogMode = false;

	flat_container_t* pContainer = pTop->GetContainer();

	if (NULL == pContainer)
	{
		return false;
	}

	if (!flat_container_add(pContainer, this))
	{
		return false;
	}
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "show", FrameArgList(), NULL);
	}

	return true;
}

bool FlatForm::ShowDialog()
{
	if (this->GetParent() != NULL)
	{
		return false;
	}

	m_bDialogMode = true;
	
	FlatUI* pFlatUI = this->GetFlatUI();

	if (!pFlatUI->AddDialog(this))
	{
		return false;
	}

	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "show", FrameArgList(), NULL);
	}

	return true;
}

bool FlatForm::Close()
{
	FlatWidget* pParent = this->GetParent();

	if (m_bDialogMode)
	{
		FlatUI* pFlatUI = this->GetFlatUI();

		if (!pFlatUI->RemoveDialog(this))
		{
			return false;
		}
	}
	else
	{
		if (NULL == pParent)
		{
			return false;
		}

		flat_container_t* pContainer = pParent->GetContainer();

		Assert(pContainer != NULL);

		if (!flat_container_remove(pContainer, this))
		{
			this->SetParent(NULL);
			return false;
		}
	}

	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "close", FrameArgList(), NULL);
	}

	return true;
}
