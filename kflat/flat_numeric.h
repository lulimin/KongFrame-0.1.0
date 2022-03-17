// flat_numeric.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_NUMERIC_H
#define __FLAT_NUMERIC_H

#include "flat_widget.h"

// FlatNumeric

class FlatNumeric : public FlatWidget
{
public:
	FlatNumeric();
	virtual ~FlatNumeric();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnMouseWheel(int x, int y, double delta, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnKeyUp(unsigned int key, unsigned int flags);
	virtual int OnChar(unsigned int ch, unsigned int flags);
	virtual int OnGotFocus();
	virtual int OnLostFocus();

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Numeral value.
	void SetValue(double value);
	double GetValue() const;

	// Integer value.
	void SetIntValue(int value);
	int GetIntValue() const;

	// Minimum value.
	void SetMinimum(double value);
	double GetMinimum() const;

	// Maximum value.
	void SetMaximum(double value);
	double GetMaximum() const;

	// Small step.
	void SetSmallStep(double value);
	double GetSmallStep() const;

	// Large step.
	void SetLargeStep(double value);
	double GetLargeStep() const;

	// Read only mode.
	void SetReadOnly(bool value);
	bool GetReadOnly() const;

	// Fraction part number.
	void SetFractions(int value);
	int GetFractions() const;

	// Maximum character number.
	void SetMaxLength(int value);
	int GetMaxLength() const;

	// Back color of selected text.
	void SetSelectBackColor(const char* value);
	ArgString GetSelectBackColor() const;

	// Fore color of selected text.
	void SetSelectForeColor(const char* value);
	ArgString GetSelectForeColor() const;

private:
	// Expand input buffer.
	void ExpandInputBuffer(int need_size);
	// Refresh text from input buffer.
	void RefreshText();
	// Change value.
	bool ChangeValue(double new_value);

	// Process key down.
	int KeyDownNormal(unsigned int key);
	int KeyDownControl(unsigned int key);
	int KeyDownShift(unsigned int key);

private:
	double m_dValue;
	double m_dMinimum;
	double m_dMaximum;
	double m_dSmallStep;
	double m_dLargeStep;
	bool m_bReadOnly;
	int m_nFractions;
	int m_nMaxLength;
	unsigned int m_nSelectBackColor;
	unsigned int m_nSelectForeColor;
	char m_InputStack[16];
	char* m_pInputBuffer;
	int m_nInputBufferSize;
	int m_nInputStart;
	int m_nInputCount;
	int m_nSelectHead;
	int m_nSelectTail;
};

#endif // __FLAT_NUMERIC_H
