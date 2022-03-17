// flat_combo_box.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_COMBO_BOX_H
#define __FLAT_COMBO_BOX_H

#include "flat_widget.h"

// FlatComboBox

class FlatTextBox;
class FlatButton;
class FlatListBox;

class FlatComboBox : public FlatWidget
{
public:
	FlatComboBox();
	virtual ~FlatComboBox();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnKeyUp(unsigned int key, unsigned int flags);
	virtual int OnChar(unsigned int ch, unsigned int flags);
	virtual int OnGotFocus();
	virtual int OnLostFocus();
	virtual int OnSizeChanged();
	virtual int OnPartClick(FlatWidget* pPart, int x, int y);
	virtual int OnPartSelect(FlatWidget* pPart, int new_value, int old_value);

	// Text of input box.
	void SetText(const char* value);
	const char* GetText() const;

	// Drop button width.
	void SetDropButtonWidth(int value);
	int GetDropButtonWidth() const;

	// Drop down list width.
	void SetDropDownWidth(int value);
	int GetDropDownWidth() const;

	// Drop down list height.
	void SetDropDownHeight(int value);
	int GetDropDownHeight() const;

	// Get part widget id.
	uniqid_t GetInputBoxID() const;
	uniqid_t GetDropButtonID() const;
	uniqid_t GetDropListID() const;

	// Set dropped status.
	void SetDroppedDown(bool value);
	// Get drop button status.
	bool GetDroppedDown() const;

private:
	// Perform part widget layout.
	void PerformLayout();

private:
	FlatTextBox* m_pInputBox;
	FlatButton* m_pDropButton;
	FlatListBox* m_pDropList;
	int m_nDropButtonWidth;
	int m_nDropDownWidth;
	int m_nDropDownHeight;
};

#endif // __FLAT_COMBO_BOX_H
