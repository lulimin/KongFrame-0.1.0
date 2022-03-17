// flat_text_box.cpp
// Created by lulimin on 2020/9/11.

#include "flat_text_box.h"
#include "flat_utility.h"
#include "flat_ui.h"
#include "flat_menu.h"
#include "../kgraphic/i_input_service.h"
#include "../inc/auto_buffer.h"
#include "../inc/unicode.h"

/// \file flat_text_box.cpp
/// \brief User interface text box.

/// object: FlatTextBox
/// \brief User interface text box object.
DEF_OBJECT(FlatTextBox, FlatWidget);

/// readonly: int TextLength
/// \brief Text character number.
DEF_READ(int, FlatTextBox, TextLength);
/// readonly: string SelectedText
/// \brief Get selected content.
DEF_READ(ArgString, FlatTextBox, SelectedText);

/// property: string Text
/// \brief Content.
DEF_PROP(const char*, FlatTextBox, Text);
/// property: string PasswordChar
/// \brief Passowrd character.
DEF_PROP(const char*, FlatTextBox, PasswordChar);
/// property: bool ReadOnly
/// \brief Read only mode.
DEF_PROP(bool, FlatTextBox, ReadOnly);
/// property: bool DigitOnly
/// \brief Only input digit.
DEF_PROP(bool, FlatTextBox, DigitOnly);
/// property: int MaxLength
/// \brief Maximum character number.
DEF_PROP(int, FlatTextBox, MaxLength);
/// property: string SelectBackColor
/// \brief Back color of selected text.
DEF_PROP(ArgString, FlatTextBox, SelectBackColor);
/// property: string SelectForeColor
/// \brief Fore color of selected text.
DEF_PROP(ArgString, FlatTextBox, SelectForeColor);

/// function: bool SelectAll()
/// \brief Select all text.
DEF_FUNC_0(bool, FlatTextBox, SelectAll);
/// function: bool CancelSelect()
/// \brief Cancel select any text.
DEF_FUNC_0(bool, FlatTextBox, CancelSelect);
/// function: bool Cut()
/// \brief Cut selected text and copy to clipboard.
DEF_FUNC_0(bool, FlatTextBox, Cut);
/// function: bool CanCopy()
/// \brief Test can copy text to clipboard.
DEF_FUNC_0(bool, FlatTextBox, CanCopy);
/// function: bool Copy()
/// \brief Copy selected text to clipboard.
DEF_FUNC_0(bool, FlatTextBox, Copy);
/// function: bool CanPaste()
/// \brief Test can paste text form clipboard.
DEF_FUNC_0(bool, FlatTextBox, CanPaste);
/// function: bool Paste()
/// \brief Paste text from clipboard.
DEF_FUNC_0(bool, FlatTextBox, Paste);

#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000

// FlatTextBox

FlatTextBox::FlatTextBox()
{
	//m_sPasswordChar = "*";
	m_bReadOnly = false;
	m_bDigitOnly = false;
	m_nMaxLength = 0;
	m_nSelectForeColor = INIT_SELECT_FORE_COLOR;
	m_nSelectBackColor = INIT_SELECT_BACK_COLOR;
	m_InputStack[0] = 0;
	m_pInputBuffer = m_InputStack;
	m_nInputBufferSize = sizeof(m_InputStack) / sizeof(m_InputStack[0]);
	m_nInputStart = 0;
	m_nInputCount = 0;
	m_nSelectHead = 0;
	m_nSelectTail = 0;
	m_nDisplayHead = 0;
	m_nDisplayTail = 0;
	m_bFirstAction = false;
	m_nDragBegin = 0;
}

FlatTextBox::~FlatTextBox()
{
	if (m_pInputBuffer != m_InputStack)
	{
		K_FREE(m_pInputBuffer);
	}
}

bool FlatTextBox::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	this->SetCanFocus(true);
	return true;
}

bool FlatTextBox::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatTextBox::DrawContent(flat_canvas_t* pCanvas, int x3, int y3, int x4,
	int y4, const char* text)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);

	int draw_width = 0;
	int display_head = m_nDisplayHead;
	int display_tail = m_nDisplayTail;

	this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());
	
	if (!util_string_empty(text))
	{
		unsigned int fore_color = this->GetForeColorValue();
		int select_head = m_nSelectHead;
		int select_tail = m_nSelectTail;

		if ((select_tail <= display_head) || (select_head >= display_tail))
		{
			select_head = 0;
			select_tail = 0;
		}
		else
		{
			if (select_head < display_head)
			{
				select_head = display_head;
			}

			if (select_tail > display_tail)
			{
				select_tail = display_tail;
			}
		}

		if (select_tail > select_head)
		{
			int head_num = select_head - display_head;

			if (head_num > 0)
			{
				draw_width += canvas_draw_text_left(pCanvas, x3, y3, x4, y4,
					fore_color, text, display_head, head_num);
			}

			int select_num = select_tail - select_head;
			int select_width = canvas_get_text_width(pCanvas, text, 
				select_head, select_num);
			int x5 = x3 + draw_width;
			int x6 = x5 + select_width;

			canvas_draw_block(pCanvas, x5, y3, x6, y4, m_nSelectBackColor);
			draw_width += canvas_draw_text_left(pCanvas, x5, y3, x6, y4,
				m_nSelectForeColor, text, select_head, select_num);

			int tail_num = display_tail - select_tail;

			if (tail_num > 0)
			{
				draw_width += canvas_draw_text_left(pCanvas, x3 + draw_width,
					y3, x4, y4, fore_color, text, select_tail, tail_num);
			}
		}
		else
		{
			draw_width = canvas_draw_text_left(pCanvas, x3, y3, x4, y4,
				fore_color, text, display_head, display_tail - display_head);
		}
	}

	if (!m_bReadOnly)
	{
		FlatWidget* pRootDelegator = this->GetRootDelegator();

		if (this->GetFocused() || pRootDelegator->GetFocused())
		{
			int x5 = x3;

			if (m_nInputStart == display_tail)
			{
				x5 += draw_width;
			}
			else if (m_nInputStart > display_head)
			{
				x5 += canvas_get_text_width(pCanvas, text, display_head,
					m_nInputStart - display_head);
			}

			this->GetFlatUI()->DrawCaret(pCanvas, x5, y3, y4 - y3);
		}
	}
}

void FlatTextBox::Draw(flat_canvas_t* pCanvas)
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

	if (this->ExistBackImage())
	{
		this->DrawBackImage(pCanvas, x1, y1, x2, y2);
	}
	else
	{
		canvas_draw_block(pCanvas, x1, y1, x2, y2, this->GetBackColorValue());
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}

	int x3 = x1 + 4;
	int y3 = y1 + 4;
	int x4 = x2 - 4;
	int y4 = y2 - 4;

	if (!m_sPasswordChar.Empty())
	{
		TAutoBuffer<char, 256, FrameAlloc> auto_buf(m_nInputCount + 1);
		char* buffer = auto_buf.GetBuffer();

		for (int i = 0; i < m_nInputCount; ++i)
		{
			buffer[i] = m_sPasswordChar.CString()[0];
		}

		buffer[m_nInputCount] = 0;
		this->DrawContent(pCanvas, x3, y3, x4, y4, buffer);
	}
	else
	{
		this->DrawContent(pCanvas, x3, y3, x4, y4, m_sText.CString());
	}
}

void FlatTextBox::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_empty_string(DT_STRING, "");
	char buffer[32];
	FrameArgData def_select_fore_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_FORE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_select_back_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_BACK_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "Text", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "ReadOnly", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "DigitOnly", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "MaxLength", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "PasswordChar", FLAT_METATYPE_STRING,
		&def_empty_string);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_part_prop(pMD, "Font");
	flat_metadata_part_prop(pMD, "BackImage");
	flat_metadata_part_prop(pMD, "ImageLayout");
	flat_metadata_part_prop(pMD, "SelectForeColor");
	flat_metadata_part_prop(pMD, "SelectBackColor");
	flat_metadata_add_event(pMD, "changed");
	flat_metadata_add_event(pMD, "select");
	flat_metadata_add_event(pMD, "leave");
}

int FlatTextBox::GetInWhere(int x, int y)
{
	FlatUI* pFlatUI = this->GetFlatUI();
	const char* font = this->GetAvailableFont();
	const char* text = m_sText.CString();
	int x1 = this->GetGlobalLeft() + 4;

	for (int i = m_nDisplayHead; i < m_nDisplayTail; ++i)
	{
		int text_width = pFlatUI->GetTextWidth(font, text, m_nDisplayHead,
			i + 1 - m_nDisplayHead);

		if (x < (x1 + text_width))
		{
			return i;
		}
	}

	return m_nDisplayTail;
}

int FlatTextBox::OnLeftDown(int x, int y, unsigned int flags)
{
	if (m_bFirstAction)
	{
		m_bFirstAction = false;
		return 0;
	}
	
	int pos = this->GetInWhere(x, y);

	m_nDragBegin = pos;
	this->SetInputStart(pos);
	this->CancelSelect();
	return 1;
}

int FlatTextBox::OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags)
{
	//int beg = this->GetInWhere(cx, cy);
	int beg = m_nDragBegin;
	int end = this->GetInWhere(x, y);

	if (beg > m_nDisplayTail)
	{
		beg = m_nDisplayTail;
	}

	if (end > beg)
	{
		this->CancelSelect();
		this->ChangeSelect(beg, end - beg);
		this->SetInputStart(end);
	}
	else if (end < beg)
	{
		this->CancelSelect();
		this->ChangeSelect(end, beg - end);
		this->SetInputStart(end);
	}

	if (x > (this->GetGlobalLeft() + this->GetWidth()))
	{
		if (m_nSelectTail < m_nInputCount)
		{
			this->ChangeSelect(m_nSelectTail, 1);
			this->SetInputStart(m_nSelectTail);
		}
	}
	
	if (x < this->GetGlobalLeft())
	{
		if (m_nSelectHead > 0)
		{
			this->ChangeSelect(m_nSelectHead - 1, 1);
			this->SetInputStart(m_nSelectHead);
		}
	}
	
	return 1;
}

int FlatTextBox::KeyDownNormal(unsigned int key)
{
	switch (key)
	{
	case IInputService::KEY_UP:
	case IInputService::KEY_DOWN:
	{
		break;
	}
	case IInputService::KEY_LEFT:
	{
		if (m_nInputStart > 0)
		{
			this->SetInputStart(m_nInputStart - 1);
		}
		
		break;
	}
	case IInputService::KEY_RIGHT:
	{
		if (m_nInputStart < m_nInputCount)
		{
			this->SetInputStart(m_nInputStart + 1);
		}
		
		break;
	}
	case IInputService::KEY_HOME:
	{
		if (m_nInputStart > 0)
		{
			this->SetInputStart(0);
		}
		
		break;
	}
	case IInputService::KEY_END:
	{
		if (m_nInputStart < m_nInputCount)
		{
			this->SetInputStart(m_nInputCount);
		}

		break;
	}
	case IInputService::KEY_DELETE:
	{
		if (m_bReadOnly)
		{
			break;
		}

		if (m_nSelectTail > m_nSelectHead)
		{
			int head = m_nSelectHead;
			
			this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
			this->SetInputStart(head);
			break;
		}
		
		if (m_nInputStart < m_nInputCount)
		{
			this->DeleteInput(m_nInputStart, 1);
			break;
		}

		break;
	}
	case IInputService::KEY_BACK:
	{
		if (m_bReadOnly)
		{
			break;
		}

		if (m_nSelectTail > m_nSelectHead)
		{
			int head = m_nSelectHead;

			this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
			this->SetInputStart(head);
			break;
		}
		
		if (m_nInputStart > 0)
		{
			this->DeleteInput(m_nInputStart - 1, 1);
			this->SetInputStart(m_nInputStart - 1);
			break;
		}
		
		break;
	}
	case IInputService::KEY_TAB:
	{
		this->SelectAll();
		break;
	}
	case IInputService::KEY_RETURN:
	{
		if (this->GetEnabled())
		{
			frame_process_event(this, 0, "select", FrameArgList(), NULL);
		}

		break;
	}
	default:
		return 0;
	}

	if (m_nSelectTail > m_nSelectHead)
	{
		this->CancelSelect();
	}
	
	return 1;
}

int FlatTextBox::KeyDownControl(unsigned int key)
{
	switch (key)
	{
	case 'X': // Cut Ctrl-X.
	{
		this->Cut();
		break;
	}
	case 'C': // Copy Ctrl-C or Ctrl-Insert.
	case IInputService::KEY_INSERT:
	{
		this->Copy();
		break;
	}
	case 'V': // Paste Ctrl-V.
	{
		this->Paste();
		break;
	}
	case 'A': // Select all Ctrl-A.
	{
		this->SelectAll();
		break;
	}
	default:
		return 0;
	}
	
	return 1;
}

int FlatTextBox::KeyDownShift(unsigned int key)
{
	switch (key)
	{
	case IInputService::KEY_LEFT:
	{
		if (m_nInputStart == 0)
		{
			break;
		}

		this->ChangeSelect(m_nInputStart - 1, 1);
		this->SetInputStart(m_nInputStart - 1);
		break;
	}
	case IInputService::KEY_RIGHT:
	{
		if (m_nInputStart == m_nInputCount)
		{
			break;
		}
		
		this->ChangeSelect(m_nInputStart, 1);
		this->SetInputStart(m_nInputStart + 1);
		break;
	}
	case IInputService::KEY_HOME:
	{
		if (m_nInputStart == 0)
		{
			break;
		}

		this->ChangeSelect(0, m_nInputStart);
		this->SetInputStart(0);
		break;
	}
	case IInputService::KEY_END:
	{
		if (m_nInputStart == m_nInputCount)
		{
			break;
		}
		
		this->ChangeSelect(m_nInputStart, m_nInputCount - m_nInputStart);
		this->SetInputStart(m_nInputCount);
		break;
	}
	case IInputService::KEY_DELETE:
	{
		if (m_bReadOnly)
		{
			break;
		}
		
		if (m_nSelectHead == m_nSelectTail)
		{
			break;
		}

		int head = m_nSelectHead;

		this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
		this->SetInputStart(head);
		break;
	}
	case IInputService::KEY_INSERT:
	{
		if (!this->Paste())
		{
			break;
		}
		
		break;
	}
	default:
		return 0;
	}
	
	return 1;
}

int FlatTextBox::OnKeyDown(unsigned int key, unsigned int flags)
{
	m_bFirstAction = false;
	this->GetFlatUI()->ResetCaret();
	
	if (flags & IInputService::FLAG_CONTROL)
	{
		return this->KeyDownControl(key);
	}
	else if (flags & IInputService::FLAG_SHIFT)
	{
		return this->KeyDownShift(key);
	}
	else
	{
		return this->KeyDownNormal(key);
	}
}

int FlatTextBox::OnKeyUp(unsigned int key, unsigned int flags)
{
	// Processed.
	return 1;
}

int FlatTextBox::OnChar(unsigned int ch, unsigned int flags)
{
	m_bFirstAction = false;

	if (m_bReadOnly)
	{
		return 0;
	}

	if (m_bDigitOnly)
	{
		if (((ch < '0') || (ch > '9')) && (ch != '-') && (ch != '.'))
		{
			return 1;
		}
	}

	if (ch < 128)
	{
		if (!isprint(ch))
		{
			return 1;
		}
	}

	if (m_nMaxLength > 0)
	{
		if (m_nInputCount >= m_nMaxLength)
		{
			// Limit character number.
			return 1;
		}
	}
	
	if (m_nSelectTail > m_nSelectHead)
	{
		this->SetInputStart(m_nSelectHead);
		this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
	}
	
	int num = this->AddToInput(&ch, 1);

	if (num > 0)
	{
		this->SetInputStart(m_nInputStart + num);
	}

	return 1;
}

int FlatTextBox::OnGotFocus()
{
	m_bFirstAction = true;
	this->SelectAll();
	return 0;
}

int FlatTextBox::OnLostFocus()
{
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "leave", FrameArgList(), NULL);
	}

	return 0;
}

int FlatTextBox::OnContextMenu(const uniqid_t& menu_id)
{
	FlatMenu* pMenu = (FlatMenu*)frame_get_object(menu_id);

	if (NULL == pMenu)
	{
		return 0;
	}

	if (!pMenu->GetDesc()->DeriveFrom("FlatMenu"))
	{
		return 0;
	}

	if (this->CanCopy())
	{
		pMenu->EnableStrip("cut");
		pMenu->EnableStrip("copy");
	}
	else
	{
		pMenu->DisableStrip("cut");
		pMenu->DisableStrip("copy");
	}

	if (this->CanPaste())
	{
		pMenu->EnableStrip("paste");
	}
	else
	{
		pMenu->DisableStrip("paste");
	}

	return 1;
}

int FlatTextBox::OnSizeChanged()
{
	this->RecountDisplay();
	this->CancelSelect();
	return 1;
}

void FlatTextBox::SetText(const char* value)
{
	Assert(value != NULL);

	size_t ws_size = plat_utf8_to_unicode_size(value);
	size_t ws_len = ws_size - 1;
	
	if (m_nMaxLength > 0)
	{
		if (ws_len > m_nMaxLength)
		{
			// Exceed maximum character number.
			return;
		}
	}

	m_sText = value;

	// Update input buffer.
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf(ws_size);
	wchar_t* ws_text = auto_buf.GetBuffer();
	
	plat_utf8_to_unicode(ws_text, ws_size * sizeof(wchar_t), value);
	this->ExpandInputBuffer((int)ws_size);

	for (size_t i = 0; i < ws_size; ++i)
	{
		m_pInputBuffer[i] = ws_text[i];
	}

	this->SetInputCount((int)ws_len);
	this->SetInputStart((int)ws_len);
	this->CancelSelect();
}

const char* FlatTextBox::GetText() const
{
	return m_sText.CString();
}

void FlatTextBox::SetPasswordChar(const char* value)
{
	Assert(value != NULL);

	m_sPasswordChar = value;
}

const char* FlatTextBox::GetPasswordChar() const
{
	return m_sPasswordChar.CString();
}

void FlatTextBox::SetReadOnly(bool value)
{
	m_bReadOnly = true;
}

bool FlatTextBox::GetReadOnly() const
{
	return m_bReadOnly;
}

void FlatTextBox::SetDigitOnly(bool value)
{
	m_bDigitOnly = value;
}

bool FlatTextBox::GetDigitOnly() const
{
	return m_bDigitOnly;
}

void FlatTextBox::SetMaxLength(int value)
{
	m_nMaxLength = value;
}

int FlatTextBox::GetMaxLength() const
{
	return m_nMaxLength;
}

void FlatTextBox::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatTextBox::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatTextBox::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatTextBox::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}

int FlatTextBox::GetTextLength() const
{
	return m_nInputCount;
}

ArgString FlatTextBox::GetSelectedText() const
{
	if (m_nSelectHead == m_nSelectTail)
	{
		return "";
	}

	Assert(m_nSelectTail > m_nSelectHead);
	Assert(m_nSelectHead >= 0);
	Assert(m_nSelectTail <= m_nInputCount);
	
	int select_num = m_nSelectTail - m_nSelectHead;
	size_t ws_size = select_num + 1;
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf(ws_size);
	wchar_t* ws_text = auto_buf.GetBuffer();

	for (int i = 0; i < select_num; ++i)
	{
		ws_text[i] = (wchar_t)m_pInputBuffer[m_nSelectHead + i];
	}

	ws_text[select_num] = 0;

	size_t utf8_size = plat_unicode_to_utf8_size(ws_text);
	TAutoBuffer<char, 256, FrameAlloc> auto_buf2(utf8_size);
	char* utf8 = auto_buf2.GetBuffer();

	plat_unicode_to_utf8(utf8, utf8_size, ws_text);
	return ArgString(utf8);
}

bool FlatTextBox::SelectAll()
{
	this->CancelSelect();

	if (m_nInputCount == 0)
	{
		return false;
	}
	
	if (!this->ChangeSelect(0, m_nInputCount))
	{
		return false;
	}

	this->SetInputStart(m_nInputCount);
	return true;
}

bool FlatTextBox::CancelSelect()
{
	m_nSelectHead = 0;
	m_nSelectTail = 0;
	return true;
}

bool FlatTextBox::Cut()
{
	if (m_bReadOnly)
	{
		return false;
	}
	
	if (m_nSelectHead == m_nSelectTail)
	{
		return false;
	}

	this->Copy();

	int head = m_nSelectHead;

	this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
	this->SetInputStart(head);
	return false;
}

bool FlatTextBox::CanCopy()
{
	return m_nSelectTail > m_nSelectHead;
}

bool FlatTextBox::Copy()
{
	if (!m_sPasswordChar.Empty())
	{
		// Donot copy password!
		return false;
	}

	if (m_nSelectHead == m_nSelectTail)
	{
		return false;
	}
	
	int select_num = m_nSelectTail - m_nSelectHead;
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf1(select_num + 1);
	wchar_t* ws_text = auto_buf1.GetBuffer();

	for (int i = 0; i < select_num; ++i)
	{
		ws_text[i] = (wchar_t)m_pInputBuffer[m_nSelectHead + i];
	}

	ws_text[select_num] = 0;

	size_t utf8_size = plat_unicode_to_utf8_size(ws_text);
	TAutoBuffer<char, 256, FrameAlloc> auto_buf2(utf8_size);
	char* utf8 = auto_buf2.GetBuffer();

	plat_unicode_to_utf8(utf8, utf8_size, ws_text);
	return plat_copy_text_to_clipboard(utf8);
}

bool FlatTextBox::CanPaste()
{
	return plat_get_clipboard_text_size() > 0;
}

bool FlatTextBox::Paste()
{
	if (m_bReadOnly)
	{
		return false;
	}
	
	size_t text_size = plat_get_clipboard_text_size();

	if (0 == text_size)
	{
		return false;
	}
	
	TAutoBuffer<char, 256, FrameAlloc> auto_buf(text_size + 1);
	char* buffer = auto_buf.GetBuffer();

	text_size = plat_paste_text_from_clipboard(buffer, text_size + 1);

	if (0 == text_size)
	{
		return false;
	}

	buffer[text_size] = 0;

	size_t ws_size = plat_utf8_to_unicode_size(buffer);

	if (0 == ws_size)
	{
		return false;
	}

	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf1(ws_size);
	wchar_t* ws = auto_buf1.GetBuffer();

	plat_utf8_to_unicode(ws, ws_size * sizeof(wchar_t), buffer);

	TAutoBuffer<unsigned int, 256, FrameAlloc> auto_buf2(ws_size);
	unsigned int* data = auto_buf2.GetBuffer();

	for (size_t i = 0; i < ws_size; ++i)
	{
		data[i] = ws[i];
	}

	if (data[ws_size - 1] == 0)
	{
		// Ignore end symbol.
		ws_size -= 1;
	}

	if (m_nSelectTail > m_nSelectHead)
	{
		this->SetInputStart(m_nSelectHead);
		this->DeleteInput(m_nSelectHead, m_nSelectTail - m_nSelectHead);
	}
	
	int num = this->AddToInput(data, (int)ws_size);

	if (num > 0)
	{
		this->SetInputStart(m_nInputStart + num);
	}

	return num > 0;
}

void FlatTextBox::ExpandInputBuffer(int need_size)
{
	if (need_size < m_nInputBufferSize)
	{
		return;
	}

	int new_size = m_nInputBufferSize * 2;

	if (new_size < need_size)
	{
		new_size = need_size * 2;
	}

	unsigned int* p = (unsigned int*)K_ALLOC(new_size * sizeof(unsigned int));

	memcpy(p, m_pInputBuffer, m_nInputBufferSize * sizeof(unsigned int));

	if (m_pInputBuffer != m_InputStack)
	{
		K_FREE(m_pInputBuffer);
	}

	m_pInputBuffer = p;
	m_nInputBufferSize = new_size;
}

void FlatTextBox::RefreshText()
{
	size_t ws_size = m_nInputCount + 1;
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf(ws_size);
	wchar_t* ws_text = auto_buf.GetBuffer();

	for (int i = 0; i < m_nInputCount; ++i)
	{
		ws_text[i] = (wchar_t)m_pInputBuffer[i];
	}

	ws_text[m_nInputCount] = 0;

	size_t utf8_size = plat_unicode_to_utf8_size(ws_text);
	TAutoBuffer<char, 256, FrameAlloc> auto_buf2(utf8_size);
	char* utf8 = auto_buf2.GetBuffer();

	plat_unicode_to_utf8(utf8, utf8_size, ws_text);
	m_sText = utf8;
}

void FlatTextBox::RecountDisplay()
{
	if ((m_nInputCount > 0) && (m_nDisplayHead == m_nDisplayTail))
	{
		m_nDisplayHead = 0;
		m_nDisplayTail = m_nInputCount;
	}
	
	FlatUI* pFlatUI = this->GetFlatUI();
	const char* font = this->GetAvailableFont();
	const char* text = m_sText.CString();
	int total = this->GetWidth() - 8;

	if (m_nInputStart > m_nDisplayTail)
	{
		m_nDisplayTail = m_nInputStart;
		m_nDisplayHead = 0;

		for (int i = 0; i < m_nDisplayTail; ++i)
		{
			int w2 = pFlatUI->GetTextWidth(font, text, i, m_nDisplayTail - i);

			if (w2 <= total)
			{
				m_nDisplayHead = i;
				break;
			}
		}

		return;
	}

	if (m_nInputStart < m_nDisplayHead)
	{
		m_nDisplayHead = m_nInputStart;
		m_nDisplayTail = m_nInputCount;

		for (int i = m_nDisplayTail; i > m_nDisplayHead; --i)
		{
			int w2 = pFlatUI->GetTextWidth(font, text, m_nDisplayHead,
				i - m_nDisplayHead);

			if (w2 <= total)
			{
				m_nDisplayTail = i;
				break;
			}
		}

		return;
	}

	int w0 = pFlatUI->GetTextWidth(font, text, 0, 0);

	if (w0 <= total)
	{
		m_nDisplayHead = 0;
		m_nDisplayTail = m_nInputCount;
		return;
	}

	m_nDisplayTail = m_nInputCount;

	for (int i = m_nDisplayTail; i > m_nDisplayHead; --i)
	{
		int w2 = pFlatUI->GetTextWidth(font, text, m_nDisplayHead,
			i - m_nDisplayHead);

		if (w2 <= total)
		{
			m_nDisplayTail = i;
			break;
		}
	}

	/*
	int w1 = 0;
	
	if (m_nInputStart > m_nDisplayHead)
	{
		w1 = pFlatUI->GetTextWidth(font, text, m_nDisplayHead, 
			m_nInputStart - m_nDisplayHead);
	}

	if (w1 <= total)
	{
		m_nDisplayTail = m_nInputCount;

		for (int i = m_nDisplayTail; i > m_nDisplayHead; --i)
		{
			int w2 = pFlatUI->GetTextWidth(font, text, m_nDisplayHead,
				i - m_nDisplayHead);

			if (w2 <= total)
			{
				m_nDisplayTail = i;
				break;
			}
		}
		
		return;
	}

	m_nDisplayHead = 0;
	m_nDisplayTail = m_nInputCount;
	*/
}

void FlatTextBox::SetInputStart(int new_value)
{
//	if (new_value == m_nInputStart)
//	{
//		return;
//	}
	
	m_nInputStart = new_value;
	this->RecountDisplay();
	this->GetFlatUI()->ResetCaret();
}

void FlatTextBox::SetInputCount(int new_value)
{
	if (new_value == m_nInputCount)
	{
		return;
	}
	
	m_nInputCount = new_value;

	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList(), NULL);
	}
}

bool FlatTextBox::ChangeSelect(int start, int count)
{
	Assert((start >= 0) && (start < m_nInputCount));
	Assert((count > 0) && ((start + count) <= m_nInputCount));

	if (m_nSelectHead == m_nSelectTail)
	{
		m_nSelectHead = start;
		m_nSelectTail = start + count;
	}
	else
	{
		int end = start + count;

		if (start == m_nSelectTail)
		{
			m_nSelectTail += count;
		}
		else if ((end == m_nSelectTail) && (start >= m_nSelectHead))
		{
			m_nSelectTail = start;
		}
		else if (end == m_nSelectHead)
		{
			m_nSelectHead = start;
		}
		else if ((start == m_nSelectHead) && (end <= m_nSelectTail))
		{
			m_nSelectHead = end;
		}
		else
		{
			m_nSelectHead = start;
			m_nSelectTail = start + count;
		}
	}

	Assert((m_nSelectHead >= 0) && (m_nSelectHead <= m_nInputCount));
	Assert((m_nSelectTail >= 0) && (m_nSelectTail <= m_nInputCount));
	Assert(m_nSelectTail >= m_nSelectHead);

	return true;
}

bool FlatTextBox::DeleteInput(int start, int count)
{
	Assert((start >= 0) && (start < m_nInputCount));
	Assert((count > 0) && ((start + count) <= m_nInputCount));

	if (start < m_nInputCount)
	{
		memmove(m_pInputBuffer + start, m_pInputBuffer + start + count,
			(m_nInputCount - start - count) * sizeof(unsigned int));
	}

	this->SetInputCount(m_nInputCount - count);
	this->RefreshText();
	this->CancelSelect();
	return true;
}

int FlatTextBox::AddToInput(unsigned int* data, int count)
{
	Assert(data != NULL);
	Assert(count > 0);

	// Validate input data.
	for (int i = 0; i < count; ++i)
	{
		unsigned int ch = data[i];
		
		if (m_bDigitOnly)
		{
			if (((ch < '0') || (ch > '9')) && (ch != '-') && (ch != '.'))
			{
				return 0;
			}
		}
		
		if (ch < 128)
		{
			if (!isprint(ch))
			{
				return 0;
			}
		}
	}

	int num = count;

	if (m_nMaxLength > 0)
	{
		if (m_nInputCount >= m_nMaxLength)
		{
			// Limit character number.
			return 0;
		}

		if ((m_nInputCount + num) > m_nMaxLength)
		{
			num = m_nMaxLength - m_nInputCount;
		}
	}

	this->ExpandInputBuffer(m_nInputCount + num);

	if (m_nInputStart < m_nInputCount)
	{
		memmove(m_pInputBuffer + m_nInputStart + num, 
			m_pInputBuffer + m_nInputStart,
			(m_nInputCount - m_nInputStart) * sizeof(unsigned int));
	}

	memcpy(m_pInputBuffer + m_nInputStart, data, sizeof(unsigned int) * num);
	this->SetInputCount(m_nInputCount + num);
	this->RefreshText();
	this->CancelSelect();
	return num;
}
