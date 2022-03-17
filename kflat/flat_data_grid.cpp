// flat_data_grid.cpp
// Created by lulimin on 2020/9/11.

#include "flat_data_grid.h"
#include "flat_ui.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_data_grid.cpp
/// \brief User interface data grid.

/// object: FlatDataGrid
/// \brief User interface data grid object.
DEF_OBJECT(FlatDataGrid, FlatWidget);

/// readonly: int CurrentRow
/// \brief Index of current select row.
DEF_READ(int, FlatDataGrid, CurrentRow);
/// readonly: int CurrentColumn
/// \brief Index of current select column.
DEF_READ(int, FlatDataGrid, CurrentColumn);
/// readonly: object VScrollBar
/// \brief Vertical scroll bar.
DEF_READ_F(uniqid_t, FlatDataGrid, VScrollBar, GetVScrollBarID);
/// readonly: object HScrollBar
/// \brief Horizontal scroll bar.
DEF_READ_F(uniqid_t, FlatDataGrid, HScrollBar, GetHScrollBarID);
/// readonly: object CrossLabel
/// \brief Cross label.
DEF_READ_F(uniqid_t, FlatDataGrid, CrossLabel, GetCrossLabelID);

/// property: bool RowHeadersVisible
/// \brief Row headers visible.
DEF_PROP(bool, FlatDataGrid, RowHeadersVisible);
/// property: bool ColumnHeadersVisible
/// \brief Column headers visible.
DEF_PROP(bool, FlatDataGrid, ColumnHeadersVisible);
/// property: bool RowSelectable
/// \brief Row can select.
DEF_PROP(bool, FlatDataGrid, RowSelectable);
/// property: bool ColumnSelectable
/// \brief Column can select.
DEF_PROP(bool, FlatDataGrid, ColumnSelectable);
/// property: string TextAlign
/// \brief Align style of text.
DEF_PROP(const char*, FlatDataGrid, TextAlign);
/// property: int RowHeight
/// \brief Height of row.
DEF_PROP(int, FlatDataGrid, RowHeight);
/// property: int ColumnWidth
/// \brief Width of column.
DEF_PROP(int, FlatDataGrid, ColumnWidth);
/// property: int RowHeaderWidth
/// \brief Width of row height.
DEF_PROP(int, FlatDataGrid, RowHeaderWidth);
/// property: int ColumnHeaderHeight
/// \brief Height of column header.
DEF_PROP(int, FlatDataGrid, ColumnHeaderHeight);
/// property: int RowCount
/// \brief Row number.
DEF_PROP(int, FlatDataGrid, RowCount);
/// property: int ColumnCount
/// \brief Column number.
DEF_PROP(int, FlatDataGrid, ColumnCount);
/// property: int ScrollSize
/// \brief Scroll bar size.
DEF_PROP(int, FlatDataGrid, ScrollSize);
/// property: bool DynamicSlide
/// \brief Dynamic scroll slide bar size.
DEF_PROP(bool, FlatDataGrid, DynamicSlide);
/// property: string ScrollStyle
/// \brief Preset scroll style.
//DEF_PROP(const char*, FlatDataGrid, ScrollStyle);
/// property: bool VScrollAlways
/// \brief Always show vertical scroll bar.
DEF_PROP(bool, FlatDataGrid, VScrollAlways);
/// property: bool HScrollAlways
/// \brief Always show horizontal scroll bar.
DEF_PROP(bool, FlatDataGrid, HScrollAlways);
/// property: string GridLineColor
/// \param Color of grid line.
DEF_PROP(ArgString, FlatDataGrid, GridLineColor);
/// property: string SelectForeColor
/// \brief Fore color of selected item.
DEF_PROP(ArgString, FlatDataGrid, SelectForeColor);
/// property: string SelectBackColor
/// \brief Background color of selected item.
DEF_PROP(ArgString, FlatDataGrid, SelectBackColor);
/// property: string CaptureColor
/// \brief Background color of captured item.
DEF_PROP(ArgString, FlatDataGrid, CaptureColor);
/// property: string HeaderForeColor
/// \brief Fore color of header item.
DEF_PROP(ArgString, FlatDataGrid, HeaderForeColor);
/// property: string HeaderBackColor
/// \brief Background color of header item.
DEF_PROP(ArgString, FlatDataGrid, HeaderBackColor);
/// property: string HeaderFont
/// \brief Font of header.
DEF_PROP(const char*, FlatDataGrid, HeaderFont);

/// function: bool SetRowHeaderText(int row, string text)
/// \brief Set text of row header.
/// \param row Index of row.
/// \param text Text of header.
DEF_FUNC_2(bool, FlatDataGrid, SetRowHeaderText, int, const char*);
/// function: bool SetColumnHeaderText(int column, string text)
/// \brief Set text of column header.
/// \param column Index of column.
/// \param text Text of header.
DEF_FUNC_2(bool, FlatDataGrid, SetColumnHeaderText, int, const char*);
/// function: bool SetColumnHeaderWidth(int column, int width)
/// \brief Set width of column.
/// \param column Index of column.
/// \param width Width of column.
DEF_FUNC_2(bool, FlatDataGrid, SetColumnHeaderWidth, int, int);
/// function: bool SetColumnHeaderAlign(int column, string align)
/// \brief Set align style of column.
/// \param column Index of column.
/// \param align Align style.
DEF_FUNC_2(bool, FlatDataGrid, SetColumnHeaderAlign, int, const char*);
/// function: bool SetGridText(int row, int column, string text)
/// \param Set text of grid.
/// \param row,column Index of grid.
/// \param text Grid text.
DEF_FUNC_3(bool, FlatDataGrid, SetGridText, int, int, const char*);
/// function: string GetGridText(int row, int column)
/// \brief Get text of grid.
/// \param row,column Index of grid.
DEF_FUNC_2(const char*, FlatDataGrid, GetGridText, int, int);
/// function: string SetGridWidget(int row, int column, object id)
/// \brief Set widget in grid.
/// \param row,column Index of grid.
/// \param id Widget object id.
DEF_FUNC_3(bool, FlatDataGrid, SetGridWidget, int, int, const uniqid_t&);
/// function: object GetGridWidget(int row, int column)
/// \brief Get widget in grid.
/// \param row,column Index of grid.
DEF_FUNC_2(uniqid_t, FlatDataGrid, GetGridWidget, int, int);
/// function: bool SetGridForeColor(int row, int column, string color)
/// \brief Set fore color of grid.
/// \param row,column Index of grid.
/// \param color Fore color value.
DEF_FUNC_3(bool, FlatDataGrid, SetGridForeColor, int, int, const char*);
/// function: bool SetGridForeColor(int row, int column, string color)
/// \brief Set background color of grid.
/// \param row,column Index of grid.
/// \param color Background color value.
DEF_FUNC_3(bool, FlatDataGrid, SetGridBackColor, int, int, const char*);
/// function: bool RemoveRow(int row)
/// \brief Remove row by index.
/// \param row Index of row.
DEF_FUNC_1(bool, FlatDataGrid, RemoveRow, int);

/// function: bool SelectRow(int row)
/// \brief Select current row.
/// \param row Index of row.
DEF_FUNC_1(bool, FlatDataGrid, SelectRow, int);
/// function: bool SelectColumn(int column)
/// \brief Select current column.
/// \param column Index of column.
DEF_FUNC_1(bool, FlatDataGrid, SelectColumn, int);
/// function: bool SelectGrid(int row, int column)
/// \brief Select current grid.
/// \param row,column Index of grid.
DEF_FUNC_2(bool, FlatDataGrid, SelectGrid, int, int);

/// function: bool BeginUdpate()
/// \brief Begin update.
DEF_FUNC_0(bool, FlatDataGrid, BeginUpdate);
/// function: bool EndUpdate()
/// \brief End update.
DEF_FUNC_0(bool, FlatDataGrid, EndUpdate);

// Text align styles.
#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

#define INIT_GRID_LINE_COLOR 0xFFFFFFFF
#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000
#define INIT_CAPTURE_COLOR 0xFF800000
#define INIT_HEADER_FORE_COLOR 0xFFFFFFFF
#define INIT_HEADER_BACK_COLOR 0xFF404040

// FlatDataGrid

FlatDataGrid::FlatDataGrid()
{
	m_bRowHeadersVisible = false;
	m_bColumnHeadersVisible = false;
	m_bRowSelectable = false;
	m_bColumnSelectable = false;
	m_bUpdating = false;
	m_nVerticalBegin = 0;
	m_nHorizontalBegin = 0;
	m_nDisplayRowBegin = 0;
	m_nDisplayRowEnd = 0;
	m_nDisplayColumnBegin = 0;
	m_nDisplayColumnEnd = 0;
	m_nCurrentRow = -1;
	m_nCurrentColumn = -1;
	m_nCaptureRow = -1;
	m_nCaptureColumn = -1;
	m_nTextAlign = 0;
	m_nRowCount = 0;
	m_nColumnCount = 0;
	m_nRowHeight = 24;
	m_nColumnWidth = 48;
	m_nRowHeaderWidth = 0;
	m_nColumnHeaderHeight = 0;
	m_pRowHeaders = NULL;
	m_pColumnHeaders = NULL;
	m_pValues = NULL;
	m_nGridLineColor = INIT_GRID_LINE_COLOR;
	m_nSelectForeColor = INIT_SELECT_FORE_COLOR;
	m_nSelectBackColor = INIT_SELECT_BACK_COLOR;
	m_nCaptureColor = INIT_CAPTURE_COLOR;
	m_nHeaderForeColor = INIT_HEADER_FORE_COLOR;
	m_nHeaderBackColor = INIT_HEADER_BACK_COLOR;
}

FlatDataGrid::~FlatDataGrid()
{
	if (m_pRowHeaders)
	{
		for (int i = 0; i < m_nRowCount; ++i)
		{
			K_DELETE(m_pRowHeaders[i]);
		}

		K_FREE(m_pRowHeaders);
	}

	if (m_pColumnHeaders)
	{
		for (int k = 0; k < m_nColumnCount; ++k)
		{
			K_DELETE(m_pColumnHeaders[k]);
		}

		K_FREE(m_pColumnHeaders);
	}

	if (m_pValues)
	{
		int grid_num = m_nRowCount * m_nColumnCount;

		for (int j = 0; j < grid_num; ++j)
		{
			K_DELETE(m_pValues[j]);
		}

		K_FREE(m_pValues);
	}
}

bool FlatDataGrid::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}

	this->SetCanFocus(true);
	this->CreateParts();
	this->CreateScroll();
	flat_scroll_initialize(this->GetScroll(), this->GetFlatUI(), true, true);
	return true;
}

bool FlatDataGrid::Shutdown()
{
	if (m_pValues)
	{
		int grid_num = m_nRowCount * m_nColumnCount;

		for (int j = 0; j < grid_num; ++j)
		{
			uniqid_t id = m_pValues[j]->WidgetID;

			if (uniqid_not_null(id))
			{
				FlatWidget* pWidget = flat_get_widget(id);

				if (pWidget)
				{
					pWidget->Release();
				}
			}
		}
	}
	
	return FlatWidget::Shutdown();
}

void FlatDataGrid::Draw(flat_canvas_t* pCanvas)
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
	bool exist_back_image = this->ExistBackImage();

	if (exist_back_image)
	{
		this->DrawBackImage(pCanvas, x1, y1, x2, y2);
	}
	else
	{
		canvas_draw_block(pCanvas, x1, y1, x2, y2, this->GetBackColorValue());
	}

	flat_scroll_draw(this->GetScroll(), pCanvas);

	if (!exist_back_image)
	{
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}
}

void FlatDataGrid::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_empty_string(DT_STRING, "");
	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_text_align(DT_STRING, "Left");
	char buffer[32];
	FrameArgData def_grid_line_color(DT_STRING,
		flat_rgba_to_string(INIT_GRID_LINE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_select_fore_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_FORE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_select_back_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_BACK_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_capture_color(DT_STRING,
		flat_rgba_to_string(INIT_CAPTURE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_header_fore_color(DT_STRING,
		flat_rgba_to_string(INIT_HEADER_FORE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_header_back_color(DT_STRING,
		flat_rgba_to_string(INIT_HEADER_BACK_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "RowHeadersVisible", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "ColumnHeadersVisible", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "RowSelectable", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "ColumnSelectable", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "TextAlign", FLAT_METATYPE_STRING,
		&def_text_align);
	flat_metadata_add_prop(pMD, "RowHeight", FLAT_METATYPE_INT32,
		NULL);
	flat_metadata_add_prop(pMD, "ColumnWidth", FLAT_METATYPE_INT32,
		NULL);
	flat_metadata_add_prop(pMD, "RowHeaderWidth", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "ColumnHeaderHeight", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "RowCount", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "ColumnCount", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "ScrollSize", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "DynamicSlide", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "VScrollAlways", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "HScrollAlways", FLAT_METATYPE_BOOLEAN,
		&def_false);
	//flat_metadata_add_prop(pMD, "ScrollStyle", FLAT_METATYPE_STRING,
	//	&def_empty_string);
	flat_metadata_add_prop(pMD, "GridLineColor", FLAT_METATYPE_COLOR,
		&def_grid_line_color);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_add_prop(pMD, "CaptureColor", FLAT_METATYPE_COLOR,
		&def_capture_color);
	flat_metadata_add_prop(pMD, "HeaderForeColor", FLAT_METATYPE_COLOR,
		&def_header_fore_color);
	flat_metadata_add_prop(pMD, "HeaderBackColor", FLAT_METATYPE_COLOR,
		&def_header_back_color);
	flat_metadata_add_prop(pMD, "HeaderFont", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_event(pMD, "click");
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_part(pMD, "VScrollBar", "FlatVScrollBar");
	flat_metadata_add_part(pMD, "HScrollBar", "FlatHScrollBar");
	flat_metadata_add_part(pMD, "CrossLabel", "FlatLabel");
}

FlatWidget* FlatDataGrid::GetRegionOf(int x, int y)
{
	if (!this->GetVisible())
	{
		return NULL;
	}

	if (!this->InSelfRegion(x, y))
	{
		// Not in region of widget.
		return NULL;
	}

	if (this->GetDesignMode())
	{
		// In design status.
		return this;
	}

	int row;
	int column;

	if (this->GetInRowColumn(x, y, &row, &column))
	{
		if ((row >= 0) && (row < m_nRowCount) &&
			(column >= 0) && (column < m_nColumnCount))
		{
			grid_value_t* pValue = m_pValues[row * m_nColumnCount + column];

			if (uniqid_not_null(pValue->WidgetID))
			{
				FlatWidget* pGridWidget = flat_get_widget(pValue->WidgetID);

				if (pGridWidget)
				{
					FlatWidget* p = pGridWidget->GetRegionOf(x, y);

					if (p)
					{
						return p;
					}
				}
			}
		}
	}

	FlatWidget* pWidget = this->InPartRegion(x, y);

	if (pWidget)
	{
		return pWidget;
	}

	if (this->OnTestTransparency(x, y))
	{
		// Is transparency part.
		return NULL;
	}

	return this;
}

// Draw aligned text.
static void draw_text(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color, const char* text, int align)
{
	if (util_string_empty(text))
	{
		return;
	}
	
	switch (align)
	{
	case ALIGN_LEFT:
		canvas_draw_text_left(pCanvas, x1, y1, x2, y2, color, text, 0, 0);
		break;
	case ALIGN_CENTER:
		canvas_draw_text_center(pCanvas, x1, y1, x2, y2, color, text, 0, 0);
		break;
	case ALIGN_RIGHT:
		canvas_draw_text_right(pCanvas, x1, y1, x2, y2, color, text, 0, 0);
		break;
	default:
		Assert(0);
		break;
	}
}

void FlatDataGrid::DrawScrollRegion(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	int x1 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y1 = this->GetGlobalTop() - m_nVerticalBegin;
	int r_beg = 0;
	int r_end = m_nRowCount - 1;

	if (r_beg < m_nDisplayRowBegin)
	{
		r_beg = m_nDisplayRowBegin;
	}

	if (r_end > m_nDisplayRowEnd)
	{
		r_end = m_nDisplayRowEnd;
	}

	int c_beg = 0;
	int c_end = m_nColumnCount - 1;

	if (c_beg < m_nDisplayColumnBegin)
	{
		c_beg = m_nDisplayColumnBegin;
	}

	if (c_end > m_nDisplayColumnEnd)
	{
		c_end = m_nDisplayColumnEnd;
	}

	bool set_header_font = false;
	bool set_font = false;

	if (m_bColumnHeadersVisible && (m_nDisplayRowBegin < 0))
	{
		const char* header_font = m_sHeaderFont.CString();

		if (util_string_empty(header_font))
		{
			this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
			set_font = true;
		}
		else
		{
			this->GetFlatUI()->SetCurrentFont(header_font);
			set_header_font = true;
		}
		
		int header_height = m_nColumnHeaderHeight;

		if (0 == header_height)
		{
			header_height = m_nRowHeight;
		}

		int x3 = x1;
		int y3 = y1;
		
		if (m_bRowHeadersVisible)
		{
			int corner_width = m_nRowHeaderWidth;

			if (0 == corner_width)
			{
				corner_width = m_nColumnWidth;
			}

			int x2 = x1 + corner_width;
			int y2 = y1 + header_height;

			if (m_nDisplayColumnBegin < 0)
			{
				canvas_draw_block(pCanvas, x1, y1, x2, y2, m_nHeaderBackColor);
				canvas_draw_frame(pCanvas, x1, y1, x2, y2, m_nGridLineColor);
			}

			x3 += corner_width;
		}

		for (int c = 0; c <= c_end; ++c)
		{
			int header_width = m_pColumnHeaders[c]->nWidth;

			if (0 == header_width)
			{
				header_width = m_nColumnWidth;
			}

			if (c >= c_beg)
			{
				int x4 = x3 + header_width;
				int y4 = y3 + header_height;

				canvas_draw_block(pCanvas, x3, y3, x4, y4, m_nHeaderBackColor);
				canvas_draw_frame(pCanvas, x3, y3, x4, y4, m_nGridLineColor);

				int align = m_nTextAlign;

				if (m_pColumnHeaders[c]->nAlign >= 0)
				{
					align = m_pColumnHeaders[c]->nAlign;
				}

				draw_text(pCanvas, x3, y3, x4, y4, m_nHeaderForeColor,
					m_pColumnHeaders[c]->sText.CString(), align);
			}

			x3 += header_width;
		}
	}

	if (m_bRowHeadersVisible && (m_nDisplayColumnBegin < 0))
	{
		if (!set_header_font)
		{
			const char* header_font = m_sHeaderFont.CString();

			if (util_string_empty(header_font))
			{
				this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
				set_font = true;
			}
			else
			{
				this->GetFlatUI()->SetCurrentFont(header_font);
				set_header_font = true;
			}
		}

		int header_width = m_nRowHeaderWidth;

		if (0 == header_width)
		{
			header_width = m_nColumnWidth;
		}

		int header_height = m_nRowHeight;
		int x3 = x1;
		int y3 = y1;

		if (m_bColumnHeadersVisible)
		{
			if (m_nColumnHeaderHeight > 0)
			{
				y3 += m_nColumnHeaderHeight;
			}
			else
			{
				y3 += header_height;
			}
		}

		y3 += r_beg * header_height;

		for (int r = r_beg; r <= r_end; ++r)
		{
			int x4 = x3 + header_width;
			int y4 = y3 + header_height;
			
			canvas_draw_block(pCanvas, x3, y3, x4, y4, m_nHeaderBackColor);
			canvas_draw_frame(pCanvas, x3, y3, x4, y4, m_nGridLineColor);
			draw_text(pCanvas, x3, y3, x4, y4, m_nHeaderForeColor,
				m_pRowHeaders[r]->sText.CString(), m_nTextAlign);
			y3 += header_height;
		}
	}

	int x_start = x1;
	int y_start = y1;

	if (m_bRowHeadersVisible)
	{
		if (m_nRowHeaderWidth > 0)
		{
			x_start += m_nRowHeaderWidth;
		}
		else
		{
			x_start += m_nColumnWidth;
		}
	}

	if (m_bColumnHeadersVisible)
	{
		if (m_nColumnHeaderHeight > 0)
		{
			y_start += m_nColumnHeaderHeight;
		}
		else
		{
			y_start += m_nRowHeight;
		}
	}

	if (!set_font)
	{
		this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
	}
	
	int y5 = y_start + r_beg * m_nRowHeight;

	for (int row = r_beg; row <= r_end; ++row)
	{
		int x5 = x_start;
		
		for (int col = 0; col <= c_end; ++col)
		{
			int column_width = m_nColumnWidth;

			if (m_pColumnHeaders)
			{
				if (m_pColumnHeaders[col]->nWidth > 0)
				{
					column_width = m_pColumnHeaders[col]->nWidth;
				}
			}

			if (col >= c_beg)
			{
				int x6 = x5 + column_width;
				int y6 = y5 + m_nRowHeight;
				grid_value_t* pValue = m_pValues[row * m_nColumnCount + col];
				FlatWidget* pWidget;

				if (uniqid_not_null(pValue->WidgetID))
				{
					pWidget = flat_get_widget(pValue->WidgetID);
				}
				else
				{
					pWidget = NULL;
				}

				if (pWidget)
				{
					pWidget->SetGlobalLeftTop(x5, y5);
					pWidget->Draw(pCanvas);
				}
				else
				{
					unsigned int fore_color = this->GetForeColorValue();
					unsigned int back_color = this->GetBackColorValue();

					if (m_nCurrentRow == row)
					{
						if ((m_nCurrentColumn == col) || 
							(m_nCurrentColumn == -1))
						{
							fore_color = m_nSelectForeColor;
							back_color = m_nSelectBackColor;
						}
					}
					else if (m_nCurrentColumn == col)
					{
						if (m_nCurrentRow == -1)
						{
							fore_color = m_nSelectForeColor;
							back_color = m_nSelectBackColor;
						}
					}

					if (this->GetCapture() && (m_nCaptureRow == row) &&
						(m_nCaptureColumn == col))
					{
						if ((m_nCurrentRow != row) ||
							(m_nCurrentColumn != col))
						{
							back_color = m_nCaptureColor;
						}
					}

					canvas_draw_block(pCanvas, x5, y5, x6, y6, back_color);
					canvas_draw_frame(pCanvas, x5, y5, x6, y6, 
						m_nGridLineColor);

					int align = m_nTextAlign;

					if (m_pColumnHeaders)
					{
						if (m_pColumnHeaders[col]->nAlign >= 0)
						{
							align = m_pColumnHeaders[col]->nAlign;
						}
					}

					draw_text(pCanvas, x5, y5, x6, y6, fore_color,
						pValue->sText.CString(), align);
				}
			}

			x5 += column_width;
		}

		y5 += m_nRowHeight;
	}
}

int FlatDataGrid::GetVerticalWhole()
{
	int height = m_nRowHeight * m_nRowCount;

	if (m_bColumnHeadersVisible)
	{
		if (m_nColumnHeaderHeight > 0)
		{
			height += m_nColumnHeaderHeight;
		}
		else
		{
			height += m_nRowHeight;
		}
	}
	
	return height;
}

int FlatDataGrid::GetVerticalReveal()
{
	return flat_scroll_reveal_height(this->GetScroll());
}

int FlatDataGrid::GetVerticalBegin()
{
	return m_nVerticalBegin;
}

void FlatDataGrid::SetVerticalBegin(int value)
{
	m_nVerticalBegin = value;
	this->UpdateDisplayRange();
}

int FlatDataGrid::GetHorizontalWhole()
{
	int width = 0;

	if (m_pColumnHeaders)
	{
		for (int i = 0; i < m_nColumnCount; ++i)
		{
			if (m_pColumnHeaders[i]->nWidth > 0)
			{
				width += m_pColumnHeaders[i]->nWidth;
			}
			else
			{
				width += m_nColumnWidth;
			}
		}
	}
	else
	{
		width += m_nColumnWidth * m_nColumnCount;
	}

	if (m_bRowHeadersVisible)
	{
		if (m_nRowHeaderWidth > 0)
		{
			width += m_nRowHeaderWidth;
		}
		else
		{
			width += m_nColumnWidth;
		}
	}

	return width;
}

int FlatDataGrid::GetHorizontalReveal()
{
	return flat_scroll_reveal_width(this->GetScroll());
}

int FlatDataGrid::GetHorizontalBegin()
{
	return m_nHorizontalBegin;
}

void FlatDataGrid::SetHorizontalBegin(int value)
{
	m_nHorizontalBegin = value;
	this->UpdateDisplayRange();
}

bool FlatDataGrid::GetInRowColumn(int x, int y, int* pRow, int* pColumn)
{
	Assert(pRow != NULL);
	Assert(pColumn != NULL);
	
	int x1 = this->GetGlobalLeft() - m_nHorizontalBegin;
	int y1 = this->GetGlobalTop() - m_nVerticalBegin;
	int r_beg = 0;
	int r_end = m_nRowCount - 1;

	if (r_beg < m_nDisplayRowBegin)
	{
		r_beg = m_nDisplayRowBegin;
	}

	if (r_end > m_nDisplayRowEnd)
	{
		r_end = m_nDisplayRowEnd;
	}

	int c_beg = 0;
	int c_end = m_nColumnCount - 1;

	if (c_beg < m_nDisplayColumnBegin)
	{
		c_beg = m_nDisplayColumnBegin;
	}

	if (c_end > m_nDisplayColumnEnd)
	{
		c_end = m_nDisplayColumnEnd;
	}

	if (m_bColumnHeadersVisible && (m_nDisplayRowBegin < 0))
	{
		int header_height = m_nColumnHeaderHeight;

		if (0 == header_height)
		{
			header_height = m_nRowHeight;
		}

		int x3 = x1;
		int y3 = y1;

		if (m_bRowHeadersVisible)
		{
			int corner_width = m_nRowHeaderWidth;

			if (0 == corner_width)
			{
				corner_width = m_nColumnWidth;
			}

			int x2 = x1 + corner_width;
			int y2 = y1 + header_height;

			if (flat_in_rectangle(x, y, x1, y1, x2, y2))
			{
				*pRow = -1;
				*pColumn = -1;
				return true;
			}

			x3 += corner_width;
		}

		for (int c = 0; c <= c_end; ++c)
		{
			int header_width = m_pColumnHeaders[c]->nWidth;

			if (0 == header_width)
			{
				header_width = m_nColumnWidth;
			}

			if (c >= c_beg)
			{
				int x4 = x3 + header_width;
				int y4 = y3 + header_height;

				if (flat_in_rectangle(x, y, x3, y3, x4, y4))
				{
					*pRow = -1;
					*pColumn = c;
					return true;
				}
			}

			x3 += header_width;
		}
	}

	if (m_bRowHeadersVisible && (m_nDisplayColumnBegin < 0))
	{
		int header_width = m_nRowHeaderWidth;

		if (0 == header_width)
		{
			header_width = m_nColumnWidth;
		}

		int header_height = m_nRowHeight;
		int x3 = x1;
		int y3 = y1;

		if (m_bColumnHeadersVisible)
		{
			if (m_nColumnHeaderHeight > 0)
			{
				y3 += m_nColumnHeaderHeight;
			}
			else
			{
				y3 += header_height;
			}
		}

		y3 += r_beg * header_height;

		for (int r = r_beg; r <= r_end; ++r)
		{
			int x4 = x3 + header_width;
			int y4 = y3 + header_height;

			if (flat_in_rectangle(x, y, x3, y3, x4, y4))
			{
				*pRow = r;
				*pColumn = -1;
				return true;
			}

			y3 += header_height;
		}
	}

	int x_start = x1;
	int y_start = y1;

	if (m_bRowHeadersVisible)
	{
		if (m_nRowHeaderWidth > 0)
		{
			x_start += m_nRowHeaderWidth;
		}
		else
		{
			x_start += m_nColumnWidth;
		}
	}

	if (m_bColumnHeadersVisible)
	{
		if (m_nColumnHeaderHeight > 0)
		{
			y_start += m_nColumnHeaderHeight;
		}
		else
		{
			y_start += m_nRowHeight;
		}
	}

	int y5 = y_start + r_beg * m_nRowHeight;

	for (int row = r_beg; row <= r_end; ++row)
	{
		int x5 = x_start;

		for (int col = 0; col <= c_end; ++col)
		{
			int column_width = m_nColumnWidth;

			if (m_pColumnHeaders)
			{
				if (m_pColumnHeaders[col]->nWidth > 0)
				{
					column_width = m_pColumnHeaders[col]->nWidth;
				}
			}

			if (col >= c_beg)
			{
				int x6 = x5 + column_width;
				int y6 = y5 + m_nRowHeight;

				if (flat_in_rectangle(x, y, x5, y5, x6, y6))
				{
					*pRow = row;
					*pColumn = col;
					return true;
				}
			}

			x5 += column_width;
		}

		y5 += m_nRowHeight;
	}
	
	return false;
}

int FlatDataGrid::OnMouseMove(int x, int y, unsigned int flags)
{
	if (m_nCaptureColor != 0)
	{
		int row;
		int column;

		if (this->GetInRowColumn(x, y, &row, &column))
		{
			m_nCaptureRow = row;
			m_nCaptureColumn = column;
		}
		else
		{
			m_nCaptureRow = -1;
			m_nCaptureColumn = -1;
		}
	}
	
	return 1;
}

int FlatDataGrid::OnMouseWheel(int x, int y, double delta, unsigned int flags)
{
	if (flags & IInputService::FLAG_CONTROL)
	{
		int reveal_width = this->GetHorizontalReveal();
		int whole_width = this->GetHorizontalWhole();

		if (reveal_width >= whole_width)
		{
			return 0;
		}

		int new_value = m_nHorizontalBegin + (int)((-delta) * 10.0);

		if (new_value < 0)
		{
			new_value = 0;
		}
		else if ((new_value + reveal_width) >= whole_width)
		{
			new_value = whole_width - reveal_width;
		}

		this->SetHorizontalBegin(new_value);
		flat_scroll_adjust_hscroll(this->GetScroll());
	}
	else
	{
		int reveal_height = this->GetVerticalReveal();
		int whole_height = this->GetVerticalWhole();

		if (reveal_height >= whole_height)
		{
			return 0;
		}

		int new_value = m_nVerticalBegin + (int)((-delta) * 10.0);

		if (new_value < 0)
		{
			new_value = 0;
		}
		else if ((new_value + reveal_height) >= whole_height)
		{
			new_value = whole_height - reveal_height;
		}

		this->SetVerticalBegin(new_value);
		flat_scroll_adjust_vscroll(this->GetScroll());
	}

	return 1;
}

int FlatDataGrid::OnLeftDown(int x, int y, unsigned int flags)
{
	int row;
	int column;

	if (!this->GetInRowColumn(x, y, &row, &column))
	{
		return 0;
	}

	if (m_bRowSelectable && m_bColumnSelectable)
	{
		this->SelectGrid(row, column);
	}
	else if (m_bRowSelectable)
	{
		this->SelectRow(row);
	}
	else if (m_bColumnSelectable)
	{
		this->SelectColumn(column);
	}

	return 1;
}

int FlatDataGrid::OnLeftDual(int x, int y, unsigned int flags)
{
	return 1;
}

int FlatDataGrid::OnKeyDown(unsigned int key, unsigned int flags)
{
	switch (key)
	{
	case IInputService::KEY_LEFT:
	{
		if (!m_bColumnSelectable)
		{
			return 0;
		}

		if (m_nCurrentColumn < 0)
		{
			return 0;
		}

		if (m_nCurrentColumn == 0)
		{
			if (m_nHorizontalBegin > 0)
			{
				this->SetHorizontalBegin(0);
				flat_scroll_adjust_hscroll(this->GetScroll());
				return 1;
			}

			return 0;
		}

		this->SelectColumn(m_nCurrentColumn - 1);
		return 1;
	}
	case IInputService::KEY_RIGHT:
	{
		if (!m_bColumnSelectable)
		{
			return 0;
		}

		if (m_nCurrentColumn < 0)
		{
			return 0;
		}

		if ((m_nCurrentColumn + 1) >= m_nColumnCount)
		{
			return 0;
		}

		this->SelectColumn(m_nCurrentColumn + 1);
		return 1;
	}
	case IInputService::KEY_UP:
	{
		if (!m_bRowSelectable)
		{
			return 0;
		}

		if (m_nCurrentRow < 0)
		{
			return 0;
		}

		if (m_nCurrentRow == 0)
		{
			if (m_nVerticalBegin > 0)
			{
				this->SetVerticalBegin(0);
				flat_scroll_adjust_vscroll(this->GetScroll());
				return 1;
			}

			return 0;
		}
		
		this->SelectRow(m_nCurrentRow - 1);
		return 1;
	}
	case IInputService::KEY_DOWN:
	{
		if (!m_bRowSelectable)
		{
			return 0;
		}

		if (m_nCurrentRow < 0)
		{
			return 0;
		}

		if ((m_nCurrentRow + 1) >= m_nRowCount)
		{
			return 0;
		}
		
		this->SelectRow(m_nCurrentRow + 1);
		return 1;
	}
	case IInputService::KEY_PRIOR:
	{
		if (!m_bRowSelectable)
		{
			return 0;
		}

		if (m_nCurrentRow < 0)
		{
			return 0;
		}
		
		if (m_nCurrentRow == 0)
		{
			if (m_nVerticalBegin > 0)
			{
				this->SetVerticalBegin(0);
				flat_scroll_adjust_vscroll(this->GetScroll());
				return 1;
			}

			return 0;
		}

		int page_row_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nRowHeight - 1;

		if (page_row_num < 1)
		{
			page_row_num = 1;
		}

		int new_row = m_nCurrentRow - page_row_num;

		if (new_row < 0)
		{
			new_row = 0;
		}

		this->SelectRow(new_row);
		return 1;
	}
	case IInputService::KEY_NEXT:
	{
		if (!m_bRowSelectable)
		{
			return 0;
		}

		if (m_nCurrentRow < 0)
		{
			return 0;
		}

		if ((m_nCurrentRow + 1) >= m_nRowCount)
		{
			return 0;
		}

		int page_row_num = flat_scroll_reveal_height(this->GetScroll()) /
			m_nRowHeight - 1;

		if (page_row_num < 1)
		{
			page_row_num = 1;
		}

		int new_row = m_nCurrentRow + page_row_num;

		if (new_row >= m_nRowCount)
		{
			new_row = m_nRowCount - 1;
		}

		this->SelectRow(new_row);
		return 1;
	}
	case IInputService::KEY_HOME:
	{
		if (flags & IInputService::FLAG_CONTROL)
		{
			if (!m_bColumnSelectable)
			{
				return 0;
			}

			if (m_nCurrentColumn < 0)
			{
				return 0;
			}

			if (m_nCurrentColumn == 0)
			{
				if (m_nHorizontalBegin > 0)
				{
					this->SetHorizontalBegin(0);
					flat_scroll_adjust_hscroll(this->GetScroll());
					return 1;
				}

				return 0;
			}

			this->SelectColumn(0);
			return 1;
		}
		else
		{
			if (!m_bRowSelectable)
			{
				return 0;
			}

			if (m_nCurrentRow < 0)
			{
				return 0;
			}

			if (m_nCurrentRow == 0)
			{
				if (m_nVerticalBegin > 0)
				{
					this->SetVerticalBegin(0);
					flat_scroll_adjust_vscroll(this->GetScroll());
					return 1;
				}

				return 0;
			}

			this->SelectRow(0);
			return 1;
		}
		
		break;
	}
	case IInputService::KEY_END:
	{
		if (flags & IInputService::FLAG_CONTROL)
		{
			if (!m_bColumnSelectable)
			{
				return 0;
			}

			if (m_nCurrentColumn < 0)
			{
				return 0;
			}

			if ((m_nCurrentColumn + 1) >= m_nColumnCount)
			{
				return 0;
			}

			this->SelectColumn(m_nColumnCount - 1);
			return 1;
		}
		else
		{
			if (!m_bRowSelectable)
			{
				return 0;
			}

			if (m_nCurrentRow < 0)
			{
				return 0;
			}

			if ((m_nCurrentRow + 1) >= m_nRowCount)
			{
				return 0;
			}

			this->SelectRow(m_nRowCount - 1);
			return 1;
		}
		
		break;
	}
	case IInputService::KEY_RETURN:
	{
		break;
	}
	default:
		break;
	}

	return 0;
}

int FlatDataGrid::OnSizeChanged()
{
	this->PerformLayout();
	this->UpdateScroll();
	this->UpdateDisplayRange();
	return 1;
}

int FlatDataGrid::OnPartSelect(FlatWidget* pPart, int new_value, int old_value)
{
	Assert(pPart != NULL);

	flat_scroll_changed(this->GetScroll(), pPart, new_value, old_value);
	return 1;
}

void FlatDataGrid::SetRowHeadersVisible(bool value)
{
	if (value == m_bRowHeadersVisible)
	{
		return;
	}
	
	if (value)
	{
		if ((m_nRowCount > 0) && (NULL == m_pRowHeaders))
		{
			m_pRowHeaders = (row_header_t**)K_ALLOC(
				sizeof(row_header_t*) * m_nRowCount);

			for (int k = 0; k < m_nRowCount; ++k)
			{
				m_pRowHeaders[k] = K_NEW(row_header_t);
			}
		}
	}
	else
	{
		if (m_pRowHeaders)
		{
			for (int i = 0; i < m_nRowCount; ++i)
			{
				K_DELETE(m_pRowHeaders[i]);
			}

			K_FREE(m_pRowHeaders);
			m_pRowHeaders = NULL;
		}
	}
	
	m_bRowHeadersVisible = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

bool FlatDataGrid::GetRowHeadersVisible() const
{
	return m_bRowHeadersVisible;
}

void FlatDataGrid::SetColumnHeadersVisible(bool value)
{
	if (value == m_bColumnHeadersVisible)
	{
		return;
	}
	
	if (value)
	{
		if ((m_nColumnCount > 0) && (NULL == m_pColumnHeaders))
		{
			m_pColumnHeaders = (column_header_t**)K_ALLOC(
				sizeof(column_header_t*) * m_nColumnCount);

			for (int i = 0; i < m_nColumnCount; ++i)
			{
				m_pColumnHeaders[i] = K_NEW(column_header_t);
			}
		}
	}
	
	m_bColumnHeadersVisible = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

bool FlatDataGrid::GetColumnHeadersVisible() const
{
	return m_bColumnHeadersVisible;
}

void FlatDataGrid::SetRowSelectable(bool value)
{
	m_bRowSelectable = value;
}

bool FlatDataGrid::GetRowSelectable() const
{
	return m_bRowSelectable;
}

void FlatDataGrid::SetColumnSelectable(bool value)
{
	m_bColumnSelectable = value;
}

bool FlatDataGrid::GetColumnSelectable() const
{
	return m_bColumnSelectable;
}

void FlatDataGrid::SetTextAlign(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Left") == 0)
	{
		m_nTextAlign = ALIGN_LEFT;
	}
	else if (stricmp(value, "Center") == 0)
	{
		m_nTextAlign = ALIGN_CENTER;
	}
	else if (stricmp(value, "Right") == 0)
	{
		m_nTextAlign = ALIGN_RIGHT;
	}
}

const char* FlatDataGrid::GetTextAlign() const
{
	switch (m_nTextAlign)
	{
	case ALIGN_LEFT:
		return "Left";
	case ALIGN_CENTER:
		return "Center";
	case ALIGN_RIGHT:
		return "Right";
	default:
		Assert(0);
		break;
	}

	return "";
}

void FlatDataGrid::SetRowHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nRowHeight = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetRowHeight() const
{
	return m_nRowHeight;
}

void FlatDataGrid::SetColumnWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nColumnWidth = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetColumnWidth() const
{
	return m_nColumnWidth;
}

void FlatDataGrid::SetRowHeaderWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nRowHeaderWidth = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetRowHeaderWidth() const
{
	return m_nRowHeaderWidth;
}

void FlatDataGrid::SetColumnHeaderHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nColumnHeaderHeight = value;

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetColumnHeaderHeight() const
{
	return m_nColumnHeaderHeight;
}

void FlatDataGrid::SetRowCount(int value)
{
	if (value < 0)
	{
		return;
	}

	if (value == m_nRowCount)
	{
		return;
	}

	int old_grid_num = m_nRowCount * m_nColumnCount;
	int new_grid_num = value * m_nColumnCount;
	
	if (new_grid_num != old_grid_num)
	{
		grid_value_t** pNewValues = NULL;

		if (new_grid_num > 0)
		{
			pNewValues = (grid_value_t**)K_ALLOC(
				sizeof(grid_value_t*) * new_grid_num);
			memset(pNewValues, 0, sizeof(grid_value_t*) * new_grid_num);
		}

		if (new_grid_num > old_grid_num)
		{
			if (pNewValues)
			{
				if (m_pValues)
				{
					memcpy(pNewValues, m_pValues,
						sizeof(grid_value_t*) * old_grid_num);
				}

				for (size_t k = old_grid_num; k < new_grid_num; ++k)
				{
					pNewValues[k] = K_NEW(grid_value_t);
				}
			}
		}
		else
		{
			if (pNewValues)
			{
				memcpy(pNewValues, m_pValues,
					sizeof(grid_value_t*) * new_grid_num);
			}

			for (int i = new_grid_num; i < old_grid_num; ++i)
			{
				K_DELETE(m_pValues[i]);
			}
		}

		if (m_pValues)
		{
			K_FREE(m_pValues);
		}

		m_pValues = pNewValues;
	}

	if (m_bRowHeadersVisible)
	{
		row_header_t** pNewHeaders = NULL;

		if (value > 0)
		{
			pNewHeaders = (row_header_t**)K_ALLOC(
				sizeof(row_header_t*) * value);
			memset(pNewHeaders, 0, sizeof(row_header_t*) * value);
		}

		if (value > m_nRowCount)
		{
			if (pNewHeaders)
			{
				if (m_pRowHeaders)
				{
					memcpy(pNewHeaders, m_pRowHeaders,
						sizeof(row_header_t*) * m_nRowCount);
				}

				for (int i = m_nRowCount; i < value; ++i)
				{
					pNewHeaders[i] = K_NEW(row_header_t);
				}
			}
		}
		else
		{
			if (pNewHeaders)
			{
				memcpy(pNewHeaders, m_pRowHeaders, 
					sizeof(row_header_t*) * value);
			}

			for (int k = value; k < m_nRowCount; ++k)
			{
				K_DELETE(m_pRowHeaders[k]);
			}
		}

		if (m_pRowHeaders)
		{
			K_FREE(m_pRowHeaders);
		}

		m_pRowHeaders = pNewHeaders;
	}

	m_nRowCount = value;

	if (m_nCurrentRow >= m_nRowCount)
	{
		m_nCurrentRow = -1;
	}

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetRowCount() const
{
	return m_nRowCount;
}

void FlatDataGrid::SetColumnCount(int value)
{
	if (value < 0)
	{
		return;
	}

	if (value == m_nColumnCount)
	{
		return;
	}

	int old_grid_num = m_nRowCount * m_nColumnCount;
	int new_grid_num = m_nRowCount * value;

	if (new_grid_num != old_grid_num)
	{
		grid_value_t** pNewValues = NULL;

		if (new_grid_num > 0)
		{
			pNewValues = (grid_value_t**)K_ALLOC(
				sizeof(grid_value_t*) * new_grid_num);
			memset(pNewValues, 0, sizeof(grid_value_t*) * new_grid_num);
		}

		if (new_grid_num > old_grid_num)
		{
			if (pNewValues)
			{
				if (m_pValues)
				{
					for (int r = 0; r < m_nRowCount; ++r)
					{
						memcpy(pNewValues + r * value,
							m_pValues + r * m_nColumnCount,
							sizeof(grid_value_t*) * m_nColumnCount);
					}
				}

				for (int row = 0; row < m_nRowCount; ++row)
				{
					for (int col = m_nColumnCount; col < value; ++col)
					{
						pNewValues[row * value + col] = K_NEW(grid_value_t);
					}
				}
			}
		}
		else
		{
			if (pNewValues)
			{
				for (int r = 0; r < m_nRowCount; ++r)
				{
					memcpy(pNewValues + r * value, 
						m_pValues + r * m_nColumnCount,
						sizeof(grid_value_t*) * value);
				}
			}

			for (int row = 0; row < m_nRowCount; ++row)
			{
				for (int col = value; col < m_nColumnCount; ++col)
				{
					K_DELETE(m_pValues[row * m_nColumnCount + col]);
				}
			}
		}

		if (m_pValues)
		{
			K_FREE(m_pValues);
		}

		m_pValues = pNewValues;
	}

	if (m_bColumnHeadersVisible || (m_pColumnHeaders != NULL))
	{
		column_header_t** pNewHeaders = NULL;

		if (value > 0)
		{
			pNewHeaders = (column_header_t**)K_ALLOC(
				sizeof(column_header_t*) * value);
			memset(pNewHeaders, 0, sizeof(column_header_t*) * value);
		}

		if (value > m_nColumnCount)
		{
			if (pNewHeaders)
			{
				if (m_pColumnHeaders)
				{
					memcpy(pNewHeaders, m_pColumnHeaders,
						sizeof(column_header_t*) * m_nColumnCount);
				}

				for (int i = m_nColumnCount; i < value; ++i)
				{
					pNewHeaders[i] = K_NEW(column_header_t);
				}
			}
		}
		else
		{
			if (pNewHeaders)
			{
				memcpy(pNewHeaders, m_pColumnHeaders,
					sizeof(column_header_t*) * value);
			}

			for (int k = value; k < m_nColumnCount; ++k)
			{
				K_DELETE(m_pColumnHeaders[k]);
			}
		}

		if (m_pColumnHeaders)
		{
			K_FREE(m_pColumnHeaders);
		}

		m_pColumnHeaders = pNewHeaders;
	}

	m_nColumnCount = value;

	if (m_nCurrentColumn >= m_nColumnCount)
	{
		m_nCurrentColumn = -1;
	}

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}
}

int FlatDataGrid::GetColumnCount() const
{
	return m_nColumnCount;
}

void FlatDataGrid::SetScrollSize(int value)
{
	if (value < 0)
	{
		return;
	}

	flat_scroll_set_bar_size(this->GetScroll(), value);
	this->PerformLayout();
}

int FlatDataGrid::GetScrollSize() const
{
	return flat_scroll_get_bar_size(this->GetScroll());
}

void FlatDataGrid::SetDynamicSlide(bool value)
{
	flat_scroll_set_dynamic_slide(this->GetScroll(), value);
}

bool FlatDataGrid::GetDynamicSlide() const
{
	return flat_scroll_get_dynamic_slide(this->GetScroll());
}

//void FlatDataGrid::SetScrollStyle(const char* value)
//{
//	Assert(value != NULL);
//
//	m_sScrollStyle = value;
//
//	if (!util_string_empty(value))
//	{
//		FlatWidget* pWidget = this->GetFlatUI()->GetScrollStyle(value);
//
//		if (pWidget)
//		{
//			flat_scroll_t* pSrcScroll = pWidget->GetScroll();
//
//			Assert(pSrcScroll != NULL);
//
//			flat_scroll_clone_style(this->GetScroll(), pSrcScroll);
//		}
//	}
//}

//const char* FlatDataGrid::GetScrollStyle() const
//{
//	return m_sScrollStyle.CString();
//}

void FlatDataGrid::SetVScrollAlways(bool value)
{
	flat_scroll_set_vscroll_always(this->GetScroll(), value);
}

bool FlatDataGrid::GetVScrollAlways() const
{
	return flat_scroll_get_vscroll_always(this->GetScroll());
}

void FlatDataGrid::SetHScrollAlways(bool value)
{
	flat_scroll_set_hscroll_always(this->GetScroll(), value);
}

bool FlatDataGrid::GetHScrollAlways() const
{
	return flat_scroll_get_hscroll_always(this->GetScroll());
}

void FlatDataGrid::SetGridLineColor(const char* value)
{
	Assert(value != NULL);

	m_nGridLineColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetGridLineColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nGridLineColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetCaptureColor(const char* value)
{
	Assert(value != NULL);

	m_nCaptureColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetCaptureColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaptureColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetHeaderForeColor(const char* value)
{
	Assert(value != NULL);

	m_nHeaderForeColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetHeaderForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nHeaderForeColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetHeaderBackColor(const char* value)
{
	Assert(value != NULL);

	m_nHeaderBackColor = flat_string_to_rgba(value);
}

ArgString FlatDataGrid::GetHeaderBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nHeaderBackColor, buffer, sizeof(buffer));
}

void FlatDataGrid::SetHeaderFont(const char* value)
{
	Assert(value != NULL);

	m_sHeaderFont = value;
}

const char* FlatDataGrid::GetHeaderFont() const
{
	return m_sHeaderFont.CString();
}

int FlatDataGrid::GetCurrentRow() const
{
	return m_nCurrentRow;
}

int FlatDataGrid::GetCurrentColumn() const
{
	return m_nCurrentColumn;
}

uniqid_t FlatDataGrid::GetVScrollBarID() const
{
	return flat_scroll_vscroll_id(this->GetScroll());
}

uniqid_t FlatDataGrid::GetHScrollBarID() const
{
	return flat_scroll_hscroll_id(this->GetScroll());
}

uniqid_t FlatDataGrid::GetCrossLabelID() const
{
	return flat_scroll_cross_label_id(this->GetScroll());
}

bool FlatDataGrid::SetRowHeaderText(int row, const char* text)
{
	Assert(text != NULL);
	
	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	if (NULL == m_pRowHeaders)
	{
		return false;
	}
	
	m_pRowHeaders[row]->sText = text;
	return true;
}

bool FlatDataGrid::SetColumnHeaderText(int column, const char* text)
{
	Assert(text != NULL);
	
	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	if (NULL == m_pColumnHeaders)
	{
		return false;
	}
	
	m_pColumnHeaders[column]->sText = text;
	return true;
}

bool FlatDataGrid::SetColumnHeaderWidth(int column, int width)
{
	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	if (width < 0)
	{
		return false;
	}

	if ((NULL == m_pColumnHeaders) && (m_nColumnCount > 0))
	{
		m_pColumnHeaders = (column_header_t**)K_ALLOC(
			sizeof(column_header_t*) * m_nColumnCount);

		for (int i = 0; i < m_nColumnCount; ++i)
		{
			m_pColumnHeaders[i] = K_NEW(column_header_t);
		}
	}
	
	m_pColumnHeaders[column]->nWidth = width;
	return true;
}

bool FlatDataGrid::SetColumnHeaderAlign(int column, const char* align)
{
	Assert(align != NULL);

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	int value = 0;

	if (stricmp(align, "Left") == 0)
	{
		value = ALIGN_LEFT;
	}
	else if (stricmp(align, "Center") == 0)
	{
		value = ALIGN_CENTER;
	}
	else if (stricmp(align, "Right") == 0)
	{
		value = ALIGN_RIGHT;
	}
	else
	{
		return false;
	}

	if ((NULL == m_pColumnHeaders) && (m_nColumnCount > 0))
	{
		m_pColumnHeaders = (column_header_t**)K_ALLOC(
			sizeof(column_header_t*) * m_nColumnCount);

		for (int i = 0; i < m_nColumnCount; ++i)
		{
			m_pColumnHeaders[i] = K_NEW(column_header_t);
		}
	}

	m_pColumnHeaders[column]->nAlign = value;
	return true;
}

bool FlatDataGrid::SetGridText(int row, int column, const char* text)
{
	Assert(text != NULL);

	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	m_pValues[row * m_nColumnCount + column]->sText = text;
	return true;
}

const char* FlatDataGrid::GetGridText(int row, int column)
{
	if ((row < 0) || (row >= m_nRowCount))
	{
		return "";
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return "";
	}

	return m_pValues[row * m_nColumnCount + column]->sText.CString();
}

bool FlatDataGrid::SetGridWidget(int row, int column, const uniqid_t& id)
{
	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	FlatWidget* pWidget = flat_get_widget(id);
	grid_value_t* pValue = m_pValues[row * m_nColumnCount + column];

	if (NULL == pWidget)
	{
		pValue->WidgetID = uniqid_t();
	}
	else
	{
		int column_width = m_nColumnWidth;

		if (m_pColumnHeaders)
		{
			if (m_pColumnHeaders[column]->nWidth > 0)
			{
				column_width = m_pColumnHeaders[column]->nWidth;
			}
		}
		
		pWidget->SetParent(this);
		pWidget->SetSize(column_width, m_nRowHeight);
		pValue->WidgetID = id;
	}

	return true;
}

uniqid_t FlatDataGrid::GetGridWidget(int row, int column)
{
	if ((row < 0) || (row >= m_nRowCount))
	{
		return uniqid_t();
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return uniqid_t();
	}
	
	return m_pValues[row * m_nColumnCount + column]->WidgetID;
}

bool FlatDataGrid::SetGridForeColor(int row, int column, const char* color)
{
	Assert(color != NULL);

	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}
	
	m_pValues[row * m_nColumnCount + column]->nForeColor = 
		flat_string_to_rgba(color);
	return true;
}

bool FlatDataGrid::SetGridBackColor(int row, int column, const char* color)
{
	Assert(color != NULL);

	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	if ((column < 0) || (column >= m_nColumnCount))
	{
		return false;
	}

	m_pValues[row * m_nColumnCount + column]->nBackColor =
		flat_string_to_rgba(color);
	return true;
}

bool FlatDataGrid::RemoveRow(int row)
{
	if ((row < 0) || (row >= m_nRowCount))
	{
		return false;
	}

	int new_row_num = m_nRowCount - 1;
	int new_grid_num = new_row_num * m_nColumnCount;
	grid_value_t** pNewValues = NULL;

	if (new_grid_num > 0)
	{
		pNewValues = (grid_value_t**)K_ALLOC(
			sizeof(grid_value_t*) * new_grid_num);
		memset(pNewValues, 0, sizeof(grid_value_t*) * new_grid_num);
	}

	int front_num = row * m_nColumnCount;

	if (front_num > 0)
	{
		if (pNewValues)
		{
			memcpy(pNewValues, m_pValues, sizeof(grid_value_t*) * front_num);
		}
	}

	int back_num = (new_row_num - row) * m_nColumnCount;

	if (back_num > 0)
	{
		if (pNewValues)
		{
			memcpy(pNewValues + front_num, 
				m_pValues + front_num + m_nColumnCount,
				sizeof(grid_value_t*) * back_num);
		}
	}

	for (int i = 0; i < m_nColumnCount; ++i)
	{
		K_DELETE(m_pValues[front_num + i]);
	}

	if (m_pValues)
	{
		K_FREE(m_pValues);
	}

	m_pValues = pNewValues;
	m_nRowCount = new_row_num;

	if (m_nCurrentRow >= m_nRowCount)
	{
		m_nCurrentRow = -1;
	}

	if (!m_bUpdating)
	{
		this->UpdateScroll();
		this->UpdateDisplayRange();
	}

	return true;
}

bool FlatDataGrid::SelectRow(int row)
{
	if ((row < -1) || (row >= m_nRowCount))
	{
		return false;
	}

	if (row == m_nCurrentRow)
	{
		return false;
	}
	
	int old_row = m_nCurrentRow;

	m_nCurrentRow = row;
	this->ShowSelected();
	
	if (!m_bUpdating)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select_row",
				FrameArgList() << old_row, NULL);
		}
	}

	return true;
}

bool FlatDataGrid::SelectColumn(int column)
{
	if ((column < -1) || (column >= m_nColumnCount))
	{
		return false;
	}

	if (column == m_nCurrentColumn)
	{
		return false;
	}

	int old_column = m_nCurrentColumn;

	m_nCurrentColumn = column;
	this->ShowSelected();
	
	if (!m_bUpdating)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select_column",
				FrameArgList() << old_column, NULL);
		}
	}

	return true;
}

bool FlatDataGrid::SelectGrid(int row, int column)
{
	if ((row < -1) || (row >= m_nRowCount) || 
		(column < -1) || (column >= m_nColumnCount))
	{
		return false;
	}

	if ((row == m_nCurrentRow) && (column == m_nCurrentColumn))
	{
		return false;
	}
	
	int old_row = m_nCurrentRow;
	int old_column = m_nCurrentColumn;

	m_nCurrentRow = row;
	m_nCurrentColumn = column;
	this->ShowSelected();
	
	if (!m_bUpdating)
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select",
				FrameArgList() << old_row << old_column, NULL);
		}
	}

	return true;
}

bool FlatDataGrid::BeginUpdate()
{
	m_bUpdating = true;
	return true;
}

bool FlatDataGrid::EndUpdate()
{
	m_bUpdating = false;
	this->UpdateScroll();
	this->UpdateDisplayRange();
	return true;
}

void FlatDataGrid::ShowSelected()
{
	if ((m_nCurrentRow < 0) && (m_nCurrentColumn < 0))
	{
		if (m_nVerticalBegin > 0)
		{
			this->SetVerticalBegin(0);
			flat_scroll_adjust_vscroll(this->GetScroll());
		}

		if (m_nHorizontalBegin > 0)
		{
			this->SetHorizontalBegin(0);
			flat_scroll_adjust_hscroll(this->GetScroll());
		}

		return;
	}

	if (m_nCurrentRow >= 0)
	{
		int y1 = m_nCurrentRow * m_nRowHeight;

		if (m_bColumnHeadersVisible)
		{
			if (m_nColumnHeaderHeight > 0)
			{
				y1 += m_nColumnHeaderHeight;
			}
			else
			{
				y1 += m_nRowHeight;
			}
		}

		if (y1 < m_nVerticalBegin)
		{
			this->SetVerticalBegin(y1);
			flat_scroll_adjust_vscroll(this->GetScroll());
		}
		else
		{
			int y2 = y1 + m_nRowHeight;
			int reveal_height = flat_scroll_reveal_height(this->GetScroll());

			if (y2 > (m_nVerticalBegin + reveal_height))
			{
				this->SetVerticalBegin(y2 - reveal_height);
				flat_scroll_adjust_vscroll(this->GetScroll());
			}
		}
	}

	if (m_nCurrentColumn >= 0)
	{
		int x1 = m_nCurrentColumn * m_nColumnWidth;

		if (m_bRowHeadersVisible)
		{
			if (m_nRowHeaderWidth > 0)
			{
				x1 += m_nRowHeaderWidth;
			}
			else
			{
				x1 += m_nColumnWidth;
			}
		}

		if (x1 < m_nHorizontalBegin)
		{
			this->SetHorizontalBegin(x1);
			flat_scroll_adjust_hscroll(this->GetScroll());
		}
		else
		{
			int x2 = x1 + m_nColumnWidth;
			int reveal_width = flat_scroll_reveal_width(this->GetScroll());

			if (x2 > (m_nHorizontalBegin + reveal_width))
			{
				this->SetHorizontalBegin(x2 - reveal_width);
				flat_scroll_adjust_hscroll(this->GetScroll());
			}
		}
	}
}

void FlatDataGrid::PerformLayout()
{
	flat_scroll_perform_layout(this->GetScroll());
}

void FlatDataGrid::UpdateScroll()
{
	flat_scroll_update(this->GetScroll());
}

void FlatDataGrid::UpdateDisplayRange()
{
	flat_scroll_t* pScroll = this->GetScroll();
	int reveal_width = flat_scroll_reveal_width(pScroll);
	int reveal_height = flat_scroll_reveal_height(pScroll);
	int x_beg = m_nHorizontalBegin;
	int x_end = x_beg + reveal_width;
	int y_beg = m_nVerticalBegin;
	int y_end = y_beg + reveal_height;
	int x1 = 0;
	int y1 = 0;

	m_nDisplayRowBegin = m_nRowCount;
	m_nDisplayRowEnd = -1;
	m_nDisplayColumnBegin = m_nColumnCount;
	m_nDisplayColumnEnd = -1;

	if (m_bColumnHeadersVisible)
	{
		int header_height = m_nColumnHeaderHeight;

		if (0 == header_height)
		{
			header_height = m_nRowHeight;
		}

		int y2 = y1 + header_height;

		if (y2 > y_beg)
		{
			m_nDisplayRowBegin = -1;
		}
	}

	if (m_bRowHeadersVisible)
	{
		int header_width = m_nRowHeaderWidth;

		if (0 == header_width)
		{
			header_width = m_nColumnWidth;
		}

		int x2 = x1 + header_width;

		if (x2 > x_beg)
		{
			m_nDisplayColumnBegin = -1;
		}
	}

	int x_start = x1;
	int y_start = y1;

	if (m_bRowHeadersVisible)
	{
		if (m_nRowHeaderWidth > 0)
		{
			x_start += m_nRowHeaderWidth;
		}
		else
		{
			x_start += m_nColumnWidth;
		}
	}

	if (m_bColumnHeadersVisible)
	{
		if (m_nColumnHeaderHeight > 0)
		{
			y_start += m_nColumnHeaderHeight;
		}
		else
		{
			y_start += m_nRowHeight;
		}
	}

	int y5 = y_start;

	for (int row = 0; row < m_nRowCount; ++row)
	{
		int y6 = y5 + m_nRowHeight;

		if ((y6 > y_beg) && (y5 < y_end))
		{
			if (row < m_nDisplayRowBegin)
			{
				m_nDisplayRowBegin = row;
			}

			if (row > m_nDisplayRowEnd)
			{
				m_nDisplayRowEnd = row;
			}
		}
		
		y5 += m_nRowHeight;
	}

	int x5 = x_start;

	for (int col = 0; col < m_nColumnCount; ++col)
	{
		int column_width = m_nColumnWidth;

		if (m_pColumnHeaders)
		{
			if (m_pColumnHeaders[col]->nWidth > 0)
			{
				column_width = m_pColumnHeaders[col]->nWidth;
			}
		}

		int x6 = x5 + column_width;

		if ((x6 > x_beg) && (x5 < x_end))
		{
			if (col < m_nDisplayColumnBegin)
			{
				m_nDisplayColumnBegin = col;
			}

			if (col > m_nDisplayColumnEnd)
			{
				m_nDisplayColumnEnd = col;
			}
		}

		x5 += column_width;
	}
}
