// flat_v_scroll_bar.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_V_SCROLL_BAR_H
#define __FLAT_V_SCROLL_BAR_H

#include "flat_widget.h"

// FlatVScrollBar

class FlatButton;

class FlatVScrollBar : public FlatWidget
{
public:
	FlatVScrollBar();
	virtual ~FlatVScrollBar();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartClick(FlatWidget* pPart, int x, int y);
	virtual int OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
		unsigned int flags);

	// Minimum value.
	void SetMinimum(int value);
	int GetMinimum() const;

	// Maximum value.
	void SetMaximum(int value);
	int GetMaximum() const;

	// Current value.
	void SetValue(int value);
	int GetValue() const;

	// Small change step.
	void SetSmallChange(int value);
	int GetSmallChange() const;

	// Large change step.
	void SetLargeChange(int value);
	int GetLargeChange() const;

	// Step button size.
	void SetStepButtonSize(int value);
	int GetStepButtonSize() const;

	// Slide button size.
	void SetSlideButtonSize(int value);
	int GetSlideButtonSize() const;

	// Get part widget object id.
	uniqid_t GetUpButtonID() const;
	uniqid_t GetDownButtonID() const;
	uniqid_t GetSlideButtonID() const;

	// Adjust scroll value.
	bool AdjustValue(int new_value);
	// Adjust slide button size.
	bool AdjustSlideSize(float rate, int min_size);

private:
	// Increase value.
	bool IncreaseValue(int delta);
	// Set layout of part widgets.
	void PerformLayout();
	// Move slide button to corrent position.
	bool LocateSlideButton();
	// Value changed.
	void ValueChanged(int old_value);

private:
	FlatButton* m_pUpButton;
	FlatButton* m_pDownButton;
	FlatButton* m_pSlideButton;
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
	int m_nSmallChange;
	int m_nLargeChange;
	int m_nStepButtonSize;
	int m_nSlideButtonSize;
};

#endif // __FLAT_V_SCROLL_BAR_H
