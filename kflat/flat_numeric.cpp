// flat_numeric.cpp
// Created by lulimin on 2020/9/11.

#include "flat_numeric.h"
#include "flat_utility.h"
#include "flat_ui.h"
#include "flat_menu.h"
#include "../kgraphic/i_input_service.h"
#include "../inc/auto_buffer.h"
#include "../inc/unicode.h"
#include <ctype.h>

/// \file flat_numeric.cpp
/// \brief User interface numeric.

/// object: FlatNumeric
/// \brief User interface numeric object.
DEF_OBJECT(FlatNumeric, FlatWidget);

/// property: string Text
/// \brief Content.
DEF_PROP(const char*, FlatNumeric, Text);
/// property: double Value
/// \brief Numeral value.
DEF_PROP(double, FlatNumeric, Value);
/// property: int IntValue
/// \brief Integer value.
DEF_PROP(int, FlatNumeric, IntValue);
/// property: double Minimum
/// \brief Minimum value.
DEF_PROP(double, FlatNumeric, Minimum);
/// property: double Maximum
/// \brief Maximum value.
DEF_PROP(double, FlatNumeric, Maximum);
/// property: double SmallStep
/// \brief Small step change.
DEF_PROP(double, FlatNumeric, SmallStep);
/// property: double LargeStep
/// \brief Large step change.
DEF_PROP(double, FlatNumeric, LargeStep);
/// property: bool ReadOnly
/// \brief Read only mode.
DEF_PROP(bool, FlatNumeric, ReadOnly);
/// property: int Fractions
/// \brief Number of fraction part.
DEF_PROP(int, FlatNumeric, Fractions);
/// property: int MaxLength
/// \brief Maximum character number.
DEF_PROP(int, FlatNumeric, MaxLength);
/// property: string SelectBackColor
/// \brief Back color of selected text.
DEF_PROP(ArgString, FlatNumeric, SelectBackColor);
/// property: string SelectForeColor
/// \brief Fore color of selected text.
DEF_PROP(ArgString, FlatNumeric, SelectForeColor);

#define FLOAT_EQUAL(v1, v2, ep) ((v1) <= ((v2) + ep)) && ((v1) >= ((v2) - ep))
#define INIT_SELECT_FORE_COLOR 0xFFFFFFFF
#define INIT_SELECT_BACK_COLOR 0xFFFF0000

// FlatNumeric

FlatNumeric::FlatNumeric()
{
	m_dValue = 0.0;
	m_dMinimum = 0.0;
	m_dMaximum = 0.0;
	m_dSmallStep = 0.0;
	m_dLargeStep = 0.0;
	m_bReadOnly = false;
	m_nFractions = 0;
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
}

FlatNumeric::~FlatNumeric()
{
	if (m_pInputBuffer != m_InputStack)
	{
		K_FREE(m_pInputBuffer);
	}
}

bool FlatNumeric::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	this->SetCanFocus(true);
	return true;
}

bool FlatNumeric::Shutdown()
{
	return FlatWidget::Shutdown();
}

void FlatNumeric::Draw(flat_canvas_t* pCanvas)
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

	if (canvas_set_range(pCanvas, x3, y3, x4 - x3, y4 - y3))
	{
		this->GetFlatUI()->SetCurrentFont(this->GetAvailableFont());

		const char* text = m_pInputBuffer;
		int draw_width = 0;

		if (!util_string_empty(text))
		{
			draw_width = canvas_draw_text_left(pCanvas, x3, y3, x4, y4,
				this->GetForeColorValue(), text, 0, 0);
		}

		if (this->GetFocused())
		{
			int x5 = x3;

			if (m_nInputStart == m_nInputCount)
			{
				x5 += draw_width;
			}
			else if (m_nInputStart > 0)
			{
				x5 += canvas_get_text_width(pCanvas, text, 0, m_nInputStart);
			}

			this->GetFlatUI()->DrawCaret(pCanvas, x5, y3, y4 - y3);
		}

		canvas_reset_range(pCanvas);
	}
}

void FlatNumeric::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);
	FrameArgData def_zero_int32(DT_INT32, 0);
	FrameArgData def_zero_double(DT_DOUBLE, 0.0);
	char buffer[32];
	FrameArgData def_select_fore_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_FORE_COLOR, buffer, sizeof(buffer)));
	FrameArgData def_select_back_color(DT_STRING,
		flat_rgba_to_string(INIT_SELECT_BACK_COLOR, buffer, sizeof(buffer)));

	flat_metadata_add_prop(pMD, "Value", FLAT_METATYPE_DOUBLE,
		&def_zero_double);
	flat_metadata_add_prop(pMD, "Minimum", FLAT_METATYPE_DOUBLE,
		&def_zero_double);
	flat_metadata_add_prop(pMD, "Maximum", FLAT_METATYPE_DOUBLE,
		&def_zero_double);
	flat_metadata_add_prop(pMD, "SmallStep", FLAT_METATYPE_DOUBLE,
		&def_zero_double);
	flat_metadata_add_prop(pMD, "LargeStep", FLAT_METATYPE_DOUBLE,
		&def_zero_double);
	flat_metadata_add_prop(pMD, "ReadOnly", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "Fractions", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "MaxLength", FLAT_METATYPE_INT32,
		&def_zero_int32);
	flat_metadata_add_prop(pMD, "SelectForeColor", FLAT_METATYPE_COLOR,
		&def_select_fore_color);
	flat_metadata_add_prop(pMD, "SelectBackColor", FLAT_METATYPE_COLOR,
		&def_select_back_color);
	flat_metadata_add_event(pMD, "changed");
}

int FlatNumeric::OnMouseWheel(int x, int y, double delta, unsigned int flags)
{
	if (m_bReadOnly)
	{
		return 0;
	}
	
	double step;
	
	if (flags & IInputService::FLAG_SHIFT)
	{
		step = m_dLargeStep;
	}
	else
	{
		step = m_dSmallStep;
	}

	if (step <= 0.0)
	{
		return 0;
	}

	double old_value = this->GetValue();
	double new_value = old_value + (-delta) * step;

	this->SetValue(new_value);
	return 1;
}

int FlatNumeric::KeyDownNormal(unsigned int key)
{
	switch (key)
	{
	case IInputService::KEY_LEFT:
		if (m_nInputStart > 0)
		{
			m_nInputStart -= 1;
		}
		break;
	case IInputService::KEY_RIGHT:
		if (m_nInputStart < m_nInputCount)
		{
			m_nInputStart += 1;
		}
		break;
	case IInputService::KEY_HOME:
		if (m_nInputStart > 0)
		{
			m_nInputStart = 0;
		}
		break;
	case IInputService::KEY_END:
		if (m_nInputStart < m_nInputCount)
		{
			m_nInputStart = m_nInputCount;
		}
		break;
	case IInputService::KEY_DELETE:
	{
		if (m_nInputStart < m_nInputCount)
		{
			memmove(m_pInputBuffer + m_nInputStart,
				m_pInputBuffer + m_nInputStart + 1,
				(m_nInputCount - m_nInputStart - 1) * sizeof(m_InputStack[0]));
			m_nInputCount--;
			this->RefreshText();
		}

		return 1;
	}
	case IInputService::KEY_BACK:
	{
		if (m_nInputStart > 0)
		{
			if (m_nInputStart < m_nInputCount)
			{
				memmove(m_pInputBuffer + m_nInputStart - 1,
					m_pInputBuffer + m_nInputStart,
					(m_nInputCount - m_nInputStart) * sizeof(m_InputStack[0]));
			}
			
			m_nInputStart--;
			m_nInputCount--;
			this->RefreshText();
		}
		
		return 1;
	}
	default:
		break;
	}
	
	return 0;
}

int FlatNumeric::KeyDownControl(unsigned int key)
{
	switch (key)
	{
	case 'A': // Select all Ctrl-A.
		break;
	case 'Z': // Undo Ctrl-Z.
		break;
	case 'Y': // Redo Ctrl-Y.
		break;
	default:
		break;
	}
	
	return 0;
}

int FlatNumeric::KeyDownShift(unsigned int key)
{
	switch (key)
	{
	case IInputService::KEY_LEFT:
		break;
	case IInputService::KEY_RIGHT:
		break;
	case IInputService::KEY_HOME:
		break;
	case IInputService::KEY_END:
		break;
	case IInputService::KEY_DELETE:
		break;
	default:
		break;
	}
	
	return 0;
}

int FlatNumeric::OnKeyDown(unsigned int key, unsigned int flags)
{
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

int FlatNumeric::OnKeyUp(unsigned int key, unsigned int flags)
{
	// Processed.
	return 1;
}

int FlatNumeric::OnChar(unsigned int ch, unsigned int flags)
{
	this->GetFlatUI()->ResetCaret();
	
	if (m_bReadOnly)
	{
		return 0;
	}

	if (0 == m_nFractions)
	{
		if (ch == '.')
		{
			return 1;
		}
	}

	if (((ch < '0') || (ch > '9')) && (ch != '-') && (ch != '.'))
	{
		return 1;
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

	this->ExpandInputBuffer(m_nInputCount + 2);
	
	if (m_nInputStart < m_nInputCount)
	{
		memmove(m_pInputBuffer + m_nInputStart + 1, 
			m_pInputBuffer + m_nInputStart, 
			(m_nInputCount - m_nInputStart) * sizeof(unsigned int));
	}

	m_pInputBuffer[m_nInputStart] = ch;
	m_nInputStart++;
	m_nInputCount++;
	this->RefreshText();
	return 1;
}

int FlatNumeric::OnGotFocus()
{
	return 0;
}

int FlatNumeric::OnLostFocus()
{
	double value = atof(m_pInputBuffer);

	this->SetValue(value);
	return 0;
}

void FlatNumeric::ExpandInputBuffer(int need_size)
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

	char* p = (char*)K_ALLOC(new_size * sizeof(char));

	memcpy(p, m_pInputBuffer, m_nInputBufferSize * sizeof(char));

	if (m_pInputBuffer != m_InputStack)
	{
		K_FREE(m_pInputBuffer);
	}

	m_pInputBuffer = p;
	m_nInputBufferSize = new_size;
}

void FlatNumeric::RefreshText()
{
	Assert(m_nInputCount < m_nInputBufferSize);

	m_pInputBuffer[m_nInputCount] = 0;
	this->ChangeValue(atof(m_pInputBuffer));
}

bool FlatNumeric::ChangeValue(double new_value)
{
	if (m_dMaximum > m_dMinimum)
	{
		if (new_value > m_dMaximum)
		{
			new_value = m_dMaximum;
		}

		if (new_value < m_dMinimum)
		{
			new_value = m_dMinimum;
		}
	}

	double old_value = m_dValue;

	if (FLOAT_EQUAL(new_value, old_value, DBL_EPSILON))
	{
		return false;
	}

	m_dValue = new_value;
	
	if (this->GetEnabled())
	{
		frame_process_event(this, 0, "changed", FrameArgList() << old_value,
			NULL);
	}

	return true;
}

void FlatNumeric::SetText(const char* value)
{
	Assert(value != NULL);

	size_t len = strlen(value);
	
	if (m_nMaxLength > 0)
	{
		if (len > m_nMaxLength)
		{
			// Exceed maximum character number.
			return;
		}
	}

	if (0 == m_nFractions)
	{
		int int_value = atoi(value);

		this->SetIntValue(int_value);
		return;
	}

	this->ExpandInputBuffer((int)(len + 1));
	memcpy(m_pInputBuffer, value, len + 1);
	m_nInputCount = (int)len;
	m_nInputStart = m_nInputCount;
	this->ChangeValue(atof(m_pInputBuffer));
}

const char* FlatNumeric::GetText() const
{
	return m_pInputBuffer;
}

void FlatNumeric::SetValue(double value)
{
	if (!this->ChangeValue(value))
	{
		if (m_nInputCount > 0)
		{
			return;
		}
	}

	char buffer[64];
	
	if (0 == m_nFractions)
	{
		plat_safe_sprintf(buffer, sizeof(buffer), "%d", (int)(m_dValue + 0.5));
	}
	else
	{
		char format[] = { '%', '.', '0', (char)('0' + m_nFractions), 'f', 0 };
		int num = snprintf(buffer, sizeof(buffer), format, value);
		char* p = buffer + num - 1;

		while (p > buffer)
		{
			if (*p == '0')
			{
				*p = 0;
			}
			else if (*p == '.')
			{
				++p;
				*p = '0';
				break;
			}
			else
			{
				break;
			}

			--p;
		}

		p[1] = 0;
	}

	size_t len = strlen(buffer);

	this->ExpandInputBuffer((int)(len + 1));
	memcpy(m_pInputBuffer, buffer, len + 1);
	m_nInputCount = (int)len;
	m_nInputStart = m_nInputCount;
}

double FlatNumeric::GetValue() const
{
	return m_dValue;
}

void FlatNumeric::SetIntValue(int value)
{
	this->SetValue((double)value);
}

int FlatNumeric::GetIntValue() const
{
	return (int)(m_dValue + 0.5);
}

void FlatNumeric::SetMinimum(double value)
{
	m_dMinimum = value;
}

double FlatNumeric::GetMinimum() const
{
	return m_dMinimum;
}

void FlatNumeric::SetMaximum(double value)
{
	m_dMaximum = value;
}

double FlatNumeric::GetMaximum() const
{
	return m_dMaximum;
}

void FlatNumeric::SetSmallStep(double value)
{
	m_dSmallStep = value;
}

double FlatNumeric::GetSmallStep() const
{
	return m_dSmallStep;
}

void FlatNumeric::SetLargeStep(double value)
{
	m_dLargeStep = value;
}

double FlatNumeric::GetLargeStep() const
{
	return m_dLargeStep;
}

void FlatNumeric::SetReadOnly(bool value)
{
	m_bReadOnly = true;
}

bool FlatNumeric::GetReadOnly() const
{
	return m_bReadOnly;
}

void FlatNumeric::SetFractions(int value)
{
	if ((value < 0) || (value > 9))
	{
		return;
	}
	
	m_nFractions = value;
}

int FlatNumeric::GetFractions() const
{
	return m_nFractions;
}

void FlatNumeric::SetMaxLength(int value)
{
	m_nMaxLength = value;
}

int FlatNumeric::GetMaxLength() const
{
	return m_nMaxLength;
}

void FlatNumeric::SetSelectBackColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectBackColor = flat_string_to_rgba(value);
}

ArgString FlatNumeric::GetSelectBackColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectBackColor, buffer, sizeof(buffer));
}

void FlatNumeric::SetSelectForeColor(const char* value)
{
	Assert(value != NULL);

	m_nSelectForeColor = flat_string_to_rgba(value);
}

ArgString FlatNumeric::GetSelectForeColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nSelectForeColor, buffer, sizeof(buffer));
}
