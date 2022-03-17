// flat_group_box.cpp
// Created by lulimin on 2020/9/11.

#include "flat_group_box.h"
#include "flat_ui.h"

/// \file flat_group_box.cpp
/// \brief User interface group box.

/// object: FlatGroupBox
/// \brief User interface group box object.
DEF_OBJECT(FlatGroupBox, FlatWidget);

/// property: string Text
/// \brief Show text.
DEF_PROP(const char*, FlatGroupBox, Text);
/// property: bool ClipView
/// \brief Clip children view.
DEF_PROP(bool, FlatGroupBox, ClipView);

/// function: bool AddChild(object id)
/// \brief Add child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatGroupBox, AddChild, const uniqid_t&);
/// function: bool RemoveChild(object id)
/// \brief Remove child widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatGroupBox, RemoveChild, const uniqid_t&);
/// function: bool ClearChildren()
/// \brief Clear child widgets.
DEF_FUNC_0(bool, FlatGroupBox, ClearChildren);
/// function [children table] GetChildList()
/// \brief Get children widget id list.
DEF_FUNC_A(FlatGroupBox, GetChildList);

// FlatGroupBox

FlatGroupBox::FlatGroupBox()
{
	m_bClipView = false;
}

FlatGroupBox::~FlatGroupBox()
{
}

bool FlatGroupBox::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->CreateContainer();
	return true;
}

bool FlatGroupBox::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatGroupBox::Draw(flat_canvas_t* pCanvas)
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
	bool need_draw;
	
	if (m_bClipView)
	{
		need_draw = canvas_set_range(pCanvas, x1, y1, width, height);
	}
	else
	{
		need_draw = true;
	}

	if (need_draw)
	{
		if (this->ExistBackImage())
		{
			this->DrawBackImage(pCanvas, x1, y1, x2, y2);
		}

		if (!m_sText.Empty())
		{
			this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());

			int text_width = canvas_get_text_width(pCanvas, m_sText.CString(), 0,
				0);
			int text_height = canvas_get_text_height(pCanvas, m_sText.CString());
			int offset = text_height / 2;
			int x3 = x1 + offset;
			int y3 = y1 + offset;
			int x4 = x2 - offset;
			int y4 = y2 - offset;
			int lines[] = {
				x3, y3, x3 + offset, y3,
				x3 + offset + text_width, y3, x4, y3,
				x3, y3, x3, y4,
				x4, y3, x4, y4,
				x3, y4, x4, y4
			};

			canvas_draw_lines(pCanvas, lines, 5, this->GetForeColorValue());
			canvas_draw_text(pCanvas, x3 + offset, y1, 
				this->GetForeColorValue(), m_sText.CString(), 0, 0);
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

		if (m_bClipView)
		{
			canvas_reset_range(pCanvas);
		}
	}
}

void FlatGroupBox::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_empty_string(DT_STRING, "");

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "ClipView", FLAT_METATYPE_BOOLEAN,
		&def_false);
}

void FlatGroupBox::SetText(const char* value)
{
	Assert(value != NULL);

	m_sText = value;
}

const char* FlatGroupBox::GetText() const
{
	return m_sText.CString();
}

void FlatGroupBox::SetClipView(bool value)
{
	m_bClipView = value;
}

bool FlatGroupBox::GetClipView() const
{
	return m_bClipView;
}

bool FlatGroupBox::AddChild(const uniqid_t& id)
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

bool FlatGroupBox::RemoveChild(const uniqid_t& id)
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

bool FlatGroupBox::ClearChildren()
{
	flat_container_t* pContainer = this->GetContainer();

	Assert(pContainer != NULL);

	flat_container_erase_all(pContainer);
	return true;
}

void FlatGroupBox::GetChildList(const IArgList& args, IArgList* res)
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
