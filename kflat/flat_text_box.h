// flat_text_box.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_TEXT_BOX_H
#define __FLAT_TEXT_BOX_H

#include "flat_widget.h"

// FlatTextBox

class FlatTextBox : public FlatWidget
{
public:
	FlatTextBox();
	virtual ~FlatTextBox();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Callbacks.
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnKeyUp(unsigned int key, unsigned int flags);
	virtual int OnChar(unsigned int ch, unsigned int flags);
	virtual int OnGotFocus();
	virtual int OnLostFocus();
	virtual int OnContextMenu(const uniqid_t& menu_id);
	virtual int OnSizeChanged();

	// Text.
	void SetText(const char* value);
	const char* GetText() const;

	// Password character.
	void SetPasswordChar(const char* value);
	const char* GetPasswordChar() const;

	// Read only mode.
	void SetReadOnly(bool value);
	bool GetReadOnly() const;

	// Only input digit.
	void SetDigitOnly(bool value);
	bool GetDigitOnly() const;

	// Maximum character number.
	void SetMaxLength(int value);
	int GetMaxLength() const;

	// Back color of selected text.
	void SetSelectBackColor(const char* value);
	ArgString GetSelectBackColor() const;

	// Fore color of selected text.
	void SetSelectForeColor(const char* value);
	ArgString GetSelectForeColor() const;

	// Get character number.
	int GetTextLength() const;
	// Get selected content.
	ArgString GetSelectedText() const;

	// Select all.
	bool SelectAll();
	// Cancel select.
	bool CancelSelect();
	// Cut to clipboard.
	bool Cut();
	// Test can copy to clipboard.
	bool CanCopy();
	// Copy to clipboard.
	bool Copy();
	// Test can paste from clipboard.
	bool CanPaste();
	// Paste from clipboard.
	bool Paste();

private:
	// Draw text.
	void DrawContent(flat_canvas_t* pCanvas, int x3, int y3, int x4, int y4,
		const char* text);
	// Get mouse position.
	int GetInWhere(int x, int y);

	// Process key down.
	int KeyDownNormal(unsigned int key);
	int KeyDownControl(unsigned int key);
	int KeyDownShift(unsigned int key);
	
	// Expand input buffer.
	void ExpandInputBuffer(int need_size);
	// Refresh text from input buffer.
	void RefreshText();
	// Recount display range.
	void RecountDisplay();
	// Set new input start position.
	void SetInputStart(int new_value);
	// Set new input count.
	void SetInputCount(int new_value);
	// Change select text.
	bool ChangeSelect(int start, int count);
	// Delete input text.
	bool DeleteInput(int start, int count);
	// Append to input buffer.
	int AddToInput(unsigned int* pData, int count);

private:
	FrameString m_sText;
	FrameString m_sPasswordChar;
	bool m_bReadOnly;
	bool m_bDigitOnly;
	int m_nMaxLength;
	unsigned int m_nSelectBackColor;
	unsigned int m_nSelectForeColor;
	unsigned int m_InputStack[16];
	unsigned int* m_pInputBuffer;
	int m_nInputBufferSize;
	int m_nInputStart;
	int m_nInputCount;
	int m_nSelectHead;
	int m_nSelectTail;
	int m_nDisplayHead;
	int m_nDisplayTail;
	bool m_bFirstAction;
	int m_nDragBegin;
};

#endif // __FLAT_TEXT_BOX_H
