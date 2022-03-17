// flat_progress_bar.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_PROGRESS_BAR_H
#define __FLAT_PROGRESS_BAR_H

#include "flat_widget.h"

// FlatProgressBar

class FlatProgressBar : public FlatWidget
{
public:
	FlatProgressBar();
	virtual ~FlatProgressBar();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Minimum value.
	void SetMinimum(int value);
	int GetMinimum() const;

	// Maximum value.
	void SetMaximum(int value);
	int GetMaximum() const;

	// Current value.
	void SetValue(int value);
	int GetValue() const;

private:
	// Adjust current value.
	void AdjustValue();

private:
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
};

#endif // __FLAT_PROGRESS_BAR_H
