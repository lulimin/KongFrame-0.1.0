// flat_data_grid.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_DATA_GRID_H
#define __FLAT_DATA_GRID_H

#include "flat_widget.h"

// FlatDataGrid

class FlatDataGrid : public FlatWidget
{
private:
	// Row header.
	struct row_header_t
	{
		FrameString sText;
	};

	// Column header.
	struct column_header_t
	{
		FrameString sText;
		int nWidth;
		int nAlign;

		column_header_t()
		{
			nWidth = 0;
			nAlign = -1;
		}
	};
	
	// Grid content.
	struct grid_value_t
	{
		FrameString sText;
		uniqid_t WidgetID;
		unsigned int nForeColor;
		unsigned int nBackColor;

		grid_value_t()
		{
			nForeColor = 0;
			nBackColor = 0;
		}
	};

public:
	FlatDataGrid();
	virtual ~FlatDataGrid();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);
	// Get coordinate in which widget.
	virtual FlatWidget* GetRegionOf(int x, int y);

	// Scroll related.
	virtual void DrawScrollRegion(flat_canvas_t* pCanvas);
	virtual int GetVerticalWhole();
	virtual int GetVerticalReveal();
	virtual int GetVerticalBegin();
	virtual void SetVerticalBegin(int value);
	virtual int GetHorizontalWhole();
	virtual int GetHorizontalReveal();
	virtual int GetHorizontalBegin();
	virtual void SetHorizontalBegin(int value);

	// Callbacks.
	virtual int OnMouseMove(int x, int y, unsigned int flags);
	virtual int OnMouseWheel(int x, int y, double delta, unsigned int flags);
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftDual(int x, int y, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// Row headers visible.
	void SetRowHeadersVisible(bool value);
	bool GetRowHeadersVisible() const;

	// Column headers visible.
	void SetColumnHeadersVisible(bool value);
	bool GetColumnHeadersVisible() const;

	// Row can select.
	void SetRowSelectable(bool value);
	bool GetRowSelectable() const;

	// Column can select.
	void SetColumnSelectable(bool value);
	bool GetColumnSelectable() const;

	// Text align.
	void SetTextAlign(const char* value);
	const char* GetTextAlign() const;

	// Height of row.
	void SetRowHeight(int value);
	int GetRowHeight() const;

	// Width of column.
	void SetColumnWidth(int value);
	int GetColumnWidth() const;

	// Width of row header.
	void SetRowHeaderWidth(int value);
	int GetRowHeaderWidth() const;

	// Height of column header.
	void SetColumnHeaderHeight(int value);
	int GetColumnHeaderHeight() const;

	// Row number.
	void SetRowCount(int value);
	int GetRowCount() const;

	// Column number.
	void SetColumnCount(int value);
	int GetColumnCount() const;

	// Scroll bar size.
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// Dynamic scroll slide bar size.
	void SetDynamicSlide(bool value);
	bool GetDynamicSlide() const;

	// Preset scroll style.
	//void SetScrollStyle(const char* value);
	//const char* GetScrollStyle() const;

	// Always show vertical scroll bar.
	void SetVScrollAlways(bool value);
	bool GetVScrollAlways() const;

	// Always show horizontal scroll bar.
	void SetHScrollAlways(bool value);
	bool GetHScrollAlways() const;

	// Grid line color.
	void SetGridLineColor(const char* value);
	ArgString GetGridLineColor() const;

	// Select fore color.
	void SetSelectForeColor(const char* value);
	ArgString GetSelectForeColor() const;

	// Select background color.
	void SetSelectBackColor(const char* value);
	ArgString GetSelectBackColor() const;

	// Background color of captured item.
	void SetCaptureColor(const char* value);
	ArgString GetCaptureColor() const;
	
	// Header fore color.
	void SetHeaderForeColor(const char* value);
	ArgString GetHeaderForeColor() const;

	// Header background color.
	void SetHeaderBackColor(const char* value);
	ArgString GetHeaderBackColor() const;

	// Header font.
	void SetHeaderFont(const char* value);
	const char* GetHeaderFont() const;

	// Get current row.
	int GetCurrentRow() const;
	// Get current column.
	int GetCurrentColumn() const;

	// Get scroll widget id.
	uniqid_t GetVScrollBarID() const;
	uniqid_t GetHScrollBarID() const;
	uniqid_t GetCrossLabelID() const;

	// Set text of row header.
	bool SetRowHeaderText(int row, const char* text);
	// Set text of column header.
	bool SetColumnHeaderText(int column, const char* text);
	// Set width of column.
	bool SetColumnHeaderWidth(int column, int width);
	// Set align of column.
	bool SetColumnHeaderAlign(int column, const char* align);
	// Set text of grid.
	bool SetGridText(int row, int column, const char* text);
	// Get text of grid.
	const char* GetGridText(int row, int column);
	// Set widget id of grid.
	bool SetGridWidget(int row, int column, const uniqid_t& id);
	// Get widget id of grid.
	uniqid_t GetGridWidget(int row, int column);
	// Set grid fore color.
	bool SetGridForeColor(int row, int column, const char* color);
	// Set grid background color.
	bool SetGridBackColor(int row, int column, const char* color);
	// Remove row by index.
	bool RemoveRow(int row);

	// Select row.
	bool SelectRow(int row);
	// Select column.
	bool SelectColumn(int column);
	// Select grid.
	bool SelectGrid(int row, int column);

	// Begin update.
	bool BeginUpdate();
	// End update.
	bool EndUpdate();

private:
	// Get mouse in where.
	bool GetInRowColumn(int x, int y, int* pRow, int* pColumn);
	// Guarantee selected visible.
	void ShowSelected();
	// Perform part widget layout.
	void PerformLayout();
	// Update scroll status.
	void UpdateScroll();
	// Update display content.
	void UpdateDisplayRange();

private:
	bool m_bRowHeadersVisible;
	bool m_bColumnHeadersVisible;
	bool m_bRowSelectable;
	bool m_bColumnSelectable;
	bool m_bUpdating;
	int m_nVerticalBegin;
	int m_nHorizontalBegin;
	int m_nDisplayRowBegin;
	int m_nDisplayRowEnd;
	int m_nDisplayColumnBegin;
	int m_nDisplayColumnEnd;
	int m_nCurrentRow;
	int m_nCurrentColumn;
	int m_nCaptureRow;
	int m_nCaptureColumn;
	int m_nTextAlign;
	int m_nRowCount;
	int m_nColumnCount;
	int m_nRowHeight;
	int m_nColumnWidth;
	int m_nRowHeaderWidth;
	int m_nColumnHeaderHeight;
	row_header_t** m_pRowHeaders;
	column_header_t** m_pColumnHeaders;
	grid_value_t** m_pValues;
	unsigned int m_nGridLineColor;
	unsigned int m_nSelectForeColor;
	unsigned int m_nSelectBackColor;
	unsigned int m_nCaptureColor;
	unsigned int m_nHeaderForeColor;
	unsigned int m_nHeaderBackColor;
	FrameString m_sHeaderFont;
	//FrameString m_sScrollStyle;
};

#endif // __FLAT_DATA_GRID_H
