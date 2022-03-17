// flat_track_bar.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_TRACK_BAR_H
#define __FLAT_TRACK_BAR_H

#include "flat_widget.h"

// FlatTrackBar

class FlatButton;

class FlatTrackBar : public FlatWidget
{
public:
	FlatTrackBar();
	virtual ~FlatTrackBar();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);
	// Get coordinate in which widget.
	virtual FlatWidget* GetRegionOf(int x, int y);

	// Callbacks.
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
		unsigned int flags);
	
	// Minimum horizontal value.
	void SetMinimum(int value);
	int GetMinimum() const;

	// Maximum horizontal value.
	void SetMaximum(int value);
	int GetMaximum() const;

	// Current horizontal value.
	void SetValue(int value);
	int GetValue() const;

	// Minimum vertical value.
	void SetVerticalMinimum(int value);
	int GetVerticalMinimum() const;

	// Maximum vertical value.
	void SetVerticalMaximum(int value);
	int GetVerticalMaximum() const;

	// Current vertical value.
	void SetVerticalValue(int value);
	int GetVerticalValue() const;

	// Get track button.
	uniqid_t GetTrackButtonID() const;

private:
	// Adjust current value.
	void AdjustValue();
	void AdjustVerticalValue();
	// Set layout of part widgets .
	void PerformLayout();
	// Value changed.
	void ValueChanged(int old_value);
	void VerticalValueChanged(int old_value);
	// Locate track button.
	void LocateTrackButton();

private:
	FlatButton* m_pTrackButton;
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
	int m_nVerticalMinimum;
	int m_nVerticalMaximum;
	int m_nVerticalValue;
};

#endif // __FLAT_TRACK_BAR_H
