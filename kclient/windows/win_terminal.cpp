// win_terminal.cpp
// Created by lulimin on 2019/2/19.

#include "win_terminal.h"
#include "../../inc/frame_types.h"
#include "../../inc/arg_list.h"
#include "../../inc/utility.h"
#include "../../inc/unicode.h"
#include "../../inc/platform.h"
#include "../../inc/auto_buffer.h"
#include <string.h>

/// \file win_terminal.cpp
/// \brief Windows terminal.

/// object: WinTerminal
/// \brief Windows terminal object.
DEF_OBJECT(WinTerminal, IObject);

/// property: string PromptString
/// \brief Command prompt information.
DEF_PROP(const char*, WinTerminal, PromptString);
/// property: string OutputFile
/// \brief Log file name.
DEF_PROP(const char*, WinTerminal, OutputFile);
/// property: string TextColor
/// \brief Color of text.
DEF_PROP(const char*, WinTerminal, TextColor);

/// function: void Clear()
/// \brief Clear terminal screen.
DEF_FUNC_0(bool, WinTerminal, Clear);
/// function: bool Input(string info)
/// \brief Analogue keyboard input.
/// \param info Input information.
DEF_FUNC_1(bool, WinTerminal, Input, const char*);
/// function: bool Output(string info, string color)
/// \brief Output information at top of screen use specified color.
/// \param info Output information.
/// \param color Specified color(white,gray,red,green,blue,yellow).
DEF_FUNC_2(bool, WinTerminal, Output, const char*, const char*);
/// function: bool Response(string info, string color)
/// \brief Output information at bottom of screen use specified color.
/// \param info Output information.
/// \param color Specified color(white,gray,red,green,blue,yellow).
DEF_FUNC_2(bool, WinTerminal, Response, const char*, const char*);
/// function: bool ClearOutputFile()
/// \brief Clear log file.
DEF_FUNC_0(bool, WinTerminal, ClearOutputFile);

// WinTerminal

// Text colors.
#define TEXT_GRAY (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define TEXT_WHITE (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | \
	FOREGROUND_INTENSITY)
#define TEXT_RED (FOREGROUND_RED)
#define TEXT_GREEN (FOREGROUND_GREEN)
#define TEXT_BLUE (FOREGROUND_BLUE)
#define TEXT_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define TEXT_PURPLE (FOREGROUND_RED | FOREGROUND_BLUE)
#define TEXT_TEAL (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define TEXT_LIGHTRED (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define TEXT_LIGHTGREEN (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define TEXT_LIGHTBLUE (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define TEXT_LIGHTYELLOW (FOREGROUND_RED | FOREGROUND_GREEN | \
	FOREGROUND_INTENSITY)
#define TEXT_LIGHTPURPLE (FOREGROUND_RED | FOREGROUND_BLUE | \
	FOREGROUND_INTENSITY)
#define TEXT_LIGHTTEAL (FOREGROUND_GREEN | FOREGROUND_BLUE | \
	FOREGROUND_INTENSITY)

// Convert string to color value.
static unsigned int color_to_int(const char* color)
{
	Assert(color != NULL);

	if (stricmp(color, "Gray") == 0)
	{
		return TEXT_GRAY;
	}

	if (stricmp(color, "White") == 0)
	{
		return TEXT_WHITE;
	}

	if (stricmp(color, "Red") == 0)
	{
		return TEXT_RED;
	}

	if (stricmp(color, "Green") == 0)
	{
		return TEXT_GREEN;
	}

	if (stricmp(color, "Blue") == 0)
	{
		return TEXT_BLUE;
	}

	if (stricmp(color, "Yellow") == 0)
	{
		return TEXT_YELLOW;
	}

	if (stricmp(color, "Purple") == 0)
	{
		return TEXT_PURPLE;
	}

	if (stricmp(color, "Teal") == 0)
	{
		return TEXT_TEAL;
	}

	if (stricmp(color, "LightRed") == 0)
	{
		return TEXT_LIGHTRED;
	}

	if (stricmp(color, "LightGreen") == 0)
	{
		return TEXT_LIGHTGREEN;
	}

	if (stricmp(color, "LightBlue") == 0)
	{
		return TEXT_LIGHTBLUE;
	}

	if (stricmp(color, "LightYellow") == 0)
	{
		return TEXT_LIGHTYELLOW;
	}

	if (stricmp(color, "LightPurple") == 0)
	{
		return TEXT_LIGHTPURPLE;
	}

	if (stricmp(color, "LightTeal") == 0)
	{
		return TEXT_LIGHTTEAL;
	}

	return 0;
}

// Convert color value to string.
static const char* int_to_color(unsigned int value)
{
	switch (value)
	{
	case TEXT_GRAY:
		return "Gray";
	case TEXT_WHITE:
		return "White";
	case TEXT_RED:
		return "Red";
	case TEXT_GREEN:
		return "Green";
	case TEXT_BLUE:
		return "Blue";
	case TEXT_YELLOW:
		return "Yellow";
	case TEXT_PURPLE:
		return "Purple";
	case TEXT_TEAL:
		return "Teal";
	case TEXT_LIGHTRED:
		return "LightRed";
	case TEXT_LIGHTGREEN:
		return "LightGreen";
	case TEXT_LIGHTBLUE:
		return "LightBlue";
	case TEXT_LIGHTYELLOW:
		return "LightYellow";
	case TEXT_LIGHTPURPLE:
		return "LightPurple";
	case TEXT_LIGHTTEAL:
		return "LightTeal";
	default:
		break;
	}

	return "";
}

// Convert to lower case.
static TempLongString to_lower(const char* str)
{
	Assert(str != NULL);

	size_t size = strlen(str) + 1;
	TAutoBuffer<char, 256, FrameAlloc> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();

	memcpy(buffer, str, size);
	strlwr(buffer);
	return TempLongString(buffer);
}

// Test is boolean value.
static bool is_boolean(const char* str)
{
	Assert(str != NULL);
	
	if (stricmp(str, "true") == 0)
	{
		return true;
	}
	
	if (stricmp(str, "false") == 0)
	{
		return true;
	}

	return false;
}

// Test is integer.
static bool is_int32(const char* str)
{
	Assert(str != NULL);

	for (const char* s = str; *s; ++s)
	{
		if (*s == '-')
		{
			// Negative sign must at first.
			if (s != str)
			{
				return false;
			}
		} 
		else if ((*s < '0') || (*s > '9'))
		{
			return false;
		}
	}

	return true;
}

// Test is number.
static bool is_number(const char* str)
{
	Assert(str != NULL);

	bool dot = false;
	
	for (const char* s = str; *s; ++s)
	{
		if (*s == '-')
		{
			// Negative sign must at first.
			if (s != str)
			{
				return false;
			}
		} 
		else if (*s == '.')
		{
			if (dot)
			{
				return false;
			}

			dot = true;
		} 
		else if ((*s < '0') || (*s > '9'))
		{
			return false;
		}
	}

	return true;
}

// Test is string.
static bool is_string(const char* str, TempLongString* res)
{
	Assert(str != NULL);
	
	if (str[0] != '\"')
	{
		return false;
	}

	for (const char* s = str + 1; *s; ++s)
	{
		if (*s == '\"')
		{
			return true;
		}
		else
		{
			*res += *s;
		}
	}

	return false;
}

// Test is unicode string.
static bool is_utf8(const char* str, TempLongString* res)
{
	Assert(str != NULL);
	
	if (str[0] != 'L')
	{
		return false;
	}

	if (str[1] != '\"')
	{
		return false;
	}
	
	TempLongString ls;
	
	for (const char* s = str + 2; *s; ++s)
	{
		if (*s == '\"')
		{
			wchar_t ws[256];
			char utf8[256];

			plat_local_to_unicode(ws, sizeof(ws), ls.CString());
			plat_unicode_to_utf8(utf8, sizeof(utf8), ws);
			*res = utf8;
			return true;
		}
		else
		{
			ls += *s;
		}
	}

	return false;
}

// Test is object id.
static bool is_uniqid(const char* str, uniqid_t* res)
{
	Assert(str != NULL);

	bool splitter = false;
	
	for (const char* s = str; *s; ++s)
	{
		if (*s == '#')
		{
			if (splitter)
			{
				return false;
			}
			
			splitter = true;
		}
		else if ((*s < '0') || (*s > '9'))
		{
			return false;
		}
	}

	// No split symbol.
	if (!splitter)
	{
		return false;
	}

	*res = util_string_to_uniqid(str);
	return true;
}

// Convert token data.
static bool convert_token(const char* tok, IArgList* res)
{
	Assert(tok != NULL);
	
	if (is_boolean(tok))
	{
		res->AddBoolean(stricmp(tok, "true") == 0);
		return true;
	}
	
	if (is_int32(tok))
	{
		res->AddInt32(atoi(tok));
		return true;
	}
	
	if (is_number(tok))
	{
		res->AddDouble(atof(tok));
		return true;
	}
	
	TempLongString utf8;
	
	if (is_utf8(tok, &utf8))
	{
		res->AddString(utf8.CString());
		return true;
	}

	uniqid_t id;
	
	if (is_uniqid(tok, &id))
	{
		res->AddUniqid(id);
		return true;
	}
	
	TempLongString s;

	if (is_string(tok, &s)) 
	{
		res->AddString(s.CString());
		return true;
	}
	
	res->AddString(tok);
	return true;
}

// Parse terminal input string.
static bool parse_input(const char* info, IArgList* res)
{
	Assert(info != NULL);
	Assert(res != NULL);
	
	bool quote = false;
	TempLongString tok; 
	
	for (const char* s = info; *s; ++s)
	{
		if ((*s == ' ') || (*s == '\t'))
		{
			if (!quote)
			{
				if (tok.Length() > 0)
				{
					convert_token(tok.CString(), res);
					tok = "";
				}
			} 
			else
			{
				tok += *s;
			}
		} 
		else if (*s == '\"')
		{
			quote = !quote;
			tok += *s;
		}
		else
		{
			tok += *s;
		}
	}
	
	if (!tok.Empty())
	{
		convert_token(tok.CString(), res);
	}
	
	return true;
}

// Flag of exited.
static bool s_bWinTerminalExit = false;

BOOL WINAPI terminal_ctrl_handler(DWORD type)
{
	switch (type)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		s_bWinTerminalExit = true;
		return TRUE;
	}

    return FALSE;
}

WinTerminal::WinTerminal()
{
	m_hStdOut = NULL;
	m_hStdIn = NULL;
	m_hWindow = NULL;
	m_bOpened = false;
	m_nWidth = 80;
	m_nHeight = 40;
	m_nOutputHeight = 25;
	m_nTextColor = TEXT_GRAY;
	m_szInputBuffer[0] = 0;
	m_nInputCount = 0;
	m_nRecoverIndex = -1;
	m_nCursorPos = 0;
	m_sPromptString = "input> ";
}

WinTerminal::~WinTerminal()
{
	for (size_t i = 0; i < m_Histories.Size(); ++i)
	{
		K_FREE(m_Histories[i]);
	}
}

bool WinTerminal::Startup(const IArgList& args)
{
	if (!AllocConsole())
	{
		return false;
	}
	
	m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_CONTINUOUS | 
		ES_DISPLAY_REQUIRED);
	SetConsoleCtrlHandler(terminal_ctrl_handler, TRUE);
	SetConsoleMode(m_hStdIn, 0);
	SetConsoleMode(m_hStdOut, 0);
	this->SetColor(m_nTextColor);
	this->SetSize(m_nHeight, m_nWidth, m_nOutputHeight);
	m_hWindow = GetConsoleWindow();
	//ShowWindow(m_hWindow, SW_MAXIMIZE);
	ShowWindow(m_hWindow, SW_SHOW);
	frame_add_work(this);
	m_bOpened = true;
	return true;
}

bool WinTerminal::Shutdown()
{
	frame_remove_work(this);
	
	if (m_bOpened)
	{
		FreeConsole();
		m_bOpened = false;
	}

	if (m_hWindow != NULL)
	{
		m_hWindow = NULL;
	}

	return true;
}

void WinTerminal::Work(double frame_time)
{
	if (s_bWinTerminalExit)
	{
		if (!frame_get_exit())
		{
			frame_process_event(this, 0, "close", FrameArgList(), NULL);
		}

		return;
	}
	
	DWORD dwRead;

	if (!GetNumberOfConsoleInputEvents(m_hStdIn, &dwRead))
	{
		return;
	}

	if (0 == dwRead)
	{
		return;
	}

	INPUT_RECORD record[128];

	if (!ReadConsoleInput(m_hStdIn, record, 128, &dwRead))
	{
		return;
	}

	for (DWORD i = 0; i < dwRead; i++)
	{
		if (record[i].EventType != KEY_EVENT)
		{
			continue;
		}

		if (!record[i].Event.KeyEvent.bKeyDown)
		{
			continue;
		}

		char ch = record[i].Event.KeyEvent.uChar.AsciiChar;

		if (ch == 0)
		{
			unsigned int vk = record[i].Event.KeyEvent.wVirtualKeyCode;

			if (vk == VK_LEFT)
			{
				if (m_nCursorPos > 0)
				{
					m_nCursorPos--;
				}
			}
			else if (vk == VK_RIGHT)
			{
				if (m_nCursorPos < m_nInputCount)
				{
					m_nCursorPos++;
				}
			}
			else if (vk == VK_UP)
			{
				if (m_nRecoverIndex == -1)
				{
					if (!m_Histories.Empty())
					{
						m_nRecoverIndex = (int)m_Histories.Size() - 1;
						this->RecoverInput();
					}
				}
				else if (m_nRecoverIndex > 0)
				{
					m_nRecoverIndex--;
					this->RecoverInput();
				}
			}
			else if (vk == VK_DOWN)
			{
				if (m_nRecoverIndex != -1)
				{
					if (m_nRecoverIndex < (int)(m_Histories.Size() - 1))
					{
						m_nRecoverIndex++;
						this->RecoverInput();
					}
					else
					{
						m_nRecoverIndex = -1;
						m_szInputBuffer[0] = 0;
						m_nInputCount = 0;
						m_nCursorPos = 0;
					}
				}
			}
			else if (vk == VK_DELETE)
			{
				this->DeleteChar();
			}

			continue;
		}

		if ('\r' == ch)
		{
			m_szInputBuffer[m_nInputCount] = 0;
			this->Input(m_szInputBuffer);
			m_nInputCount = 0;
			m_nCursorPos = 0;
		}
		else if ('\b' == ch)
		{
			this->BackspaceChar();
		}
		else
		{
			this->InputChar(ch);
			m_nRecoverIndex = -1;
		}
	}

	m_szInputBuffer[m_nInputCount] = 0;
	this->UpdateInput();
}

void WinTerminal::SetPromptString(const char* value)
{ 
	Assert(value != NULL);

	m_sPromptString = value; 
}

const char* WinTerminal::GetPromptString() const 
{ 
	return m_sPromptString.CString(); 
}

void WinTerminal::SetOutputFile(const char* value) 
{ 
	Assert(value != NULL);

	m_sOutputFile = value; 
}

const char* WinTerminal::GetOutputFile() const 
{ 
	return m_sOutputFile.CString(); 
}

void WinTerminal::SetTextColor(const char* value)
{
	Assert(value != NULL);

	unsigned int color = color_to_int(value);

	if (color != 0)
	{
		m_nTextColor = color;
	}
}

const char* WinTerminal::GetTextColor() const
{
	return int_to_color(m_nTextColor);
}

void WinTerminal::RecoverInput()
{
	Assert((m_nRecoverIndex >= 0) && 
		(m_nRecoverIndex < (int)(m_Histories.Size())));

	util_string_copy(m_szInputBuffer, sizeof(m_szInputBuffer), 
		m_Histories[m_nRecoverIndex]);
	m_nInputCount = (int)strlen(m_szInputBuffer);
	m_nCursorPos = m_nInputCount;
}

void WinTerminal::UpdateInput()
{
	char data[256];

	plat_safe_sprintf(data, sizeof(data), "%s%s", m_sPromptString.CString(),
		m_szInputBuffer);
	this->PrintLine(this->GetInputRow(), data, m_nTextColor);
	
	COORD coord;
	
	coord.X = (SHORT)((int)m_sPromptString.Length() + m_nCursorPos);
	coord.Y = this->GetInputRow();
	SetConsoleCursorPosition(m_hStdOut, coord);
}

void WinTerminal::InputChar(char ch)
{
	Assert(m_nCursorPos <= m_nInputCount);
	
	if (m_nInputCount < (sizeof(m_szInputBuffer) - 1))
	{
		if (m_nCursorPos != m_nInputCount)
		{
			memmove(m_szInputBuffer + m_nCursorPos + 1, 
				m_szInputBuffer + m_nCursorPos, m_nInputCount - m_nCursorPos);
		}
		
		m_szInputBuffer[m_nCursorPos++] = ch;
		m_szInputBuffer[++m_nInputCount] = 0;
	}
}

void WinTerminal::DeleteChar()
{
	Assert(m_nCursorPos <= m_nInputCount);
	
	if (m_nCursorPos != m_nInputCount)
	{
		memmove(m_szInputBuffer + m_nCursorPos, 
			m_szInputBuffer + m_nCursorPos + 1, 
			m_nInputCount - m_nCursorPos - 1);
		m_szInputBuffer[--m_nInputCount] = 0;
	}
}

void WinTerminal::BackspaceChar()
{
	Assert(m_nCursorPos <= m_nInputCount);
	
	if (m_nCursorPos == 0)
	{
		return;
	}

	if (m_nCursorPos != m_nInputCount)
	{
		memmove(m_szInputBuffer + m_nCursorPos - 1, 
			m_szInputBuffer + m_nCursorPos, m_nInputCount - m_nCursorPos);
	}
	
	m_nCursorPos--;
	m_szInputBuffer[--m_nInputCount] = 0;
}

void WinTerminal::SetSize(int height, int width, int output_height)
{
	Assert((height >= 10) && (height < 100));
	Assert((width >= 10) && (width < 256));
	Assert((output_height >= 2) && (output_height < height));

	m_nHeight = height;
	m_nWidth = width;
	m_nOutputHeight = output_height;
	
	SMALL_RECT rect;

	rect.Left = 0;
	rect.Top = 0;
	rect.Right = width - 1;
	rect.Bottom = height - 1;
	SetConsoleWindowInfo(m_hStdOut, FALSE, &rect);
	
	COORD coord = { 0, 0 };

	coord.X = width;
	coord.Y = height;
	SetConsoleScreenBufferSize(m_hStdOut, coord);
	this->Clear();
}

void WinTerminal::SetColor(int color)
{
	m_nTextColor = color;
	SetConsoleTextAttribute(m_hStdOut, m_nTextColor);
}

bool WinTerminal::Clear()
{
    DWORD dwWritten;
    COORD coord = { 0, 0 };

	for (DWORD i = 0; i < m_nHeight; i++)
	{
		coord.Y = (WORD)i;
		SetConsoleCursorPosition(m_hStdOut, coord);
		
		if (i == (m_nOutputHeight - 1))
		{
			char seper[256];

			memset(seper, '#', sizeof(seper));
			WriteConsole(m_hStdOut, seper, m_nWidth, &dwWritten, NULL);
		}
		else
		{
			char empty[256];

			memset(empty, ' ', sizeof(empty));
			WriteConsole(m_hStdOut, empty, m_nWidth, &dwWritten, NULL);
		}
	}

	this->UpdateInput();
	return true;
}

bool WinTerminal::Input(const char* info)
{
	Assert(info != NULL);

	//Output((m_sPromptString + info).CString(), "");

	if (strlen(info) > 0)
	{
		if (m_nRecoverIndex == -1)
		{
			size_t info_size = strlen(info) + 1;
			char* value = (char*)K_ALLOC(info_size);

			memcpy(value, info, info_size);
			m_Histories.PushBack(value);
		}
		else
		{
			m_nRecoverIndex = -1;
		}
	}
			
	this->ProcessInput(info);
	return true;
}

int WinTerminal::GetColor(const char* color) const
{
	Assert(color != NULL);
	
	if (util_string_empty(color))
	{
		return m_nTextColor;
	}

	unsigned int val = color_to_int(color);

	if (val != 0)
	{
		return val;
	}
	
	return m_nTextColor;
}

int WinTerminal::GetInputRow() const
{
	return m_nHeight - 2;
}

void WinTerminal::PrintLine(int line, const char* str, unsigned int color)
{
	Assert(str != NULL);

	char data[600];
	WORD attr[600];
	DWORD dwWritten;
    COORD coord = { 0, (SHORT)line };
	DWORD i;

    util_string_copy(data, sizeof(data), str);
	
	for (i = (DWORD)strlen(data); i < m_nWidth; i++)
	{
		data[i] = ' ';
	}
	
	data[m_nWidth] = '\0';
	
	for (i = 0; i < m_nWidth; i++)
	{
		attr[i] = color;
	}
	
	WriteConsoleOutputAttribute(m_hStdOut, attr, m_nWidth, coord, &dwWritten);
	WriteConsoleOutputCharacter(m_hStdOut, data, m_nWidth, coord, &dwWritten); 
}

bool WinTerminal::Output(const char* str, const char* color)
{
	Assert(str != NULL);
	Assert(color != NULL);
	Assert(m_bOpened);
	Assert(m_nOutputHeight >= 2);

	char data[512];
	SMALL_RECT rect;
	COORD coord = { 0, 0 };
	CHAR_INFO ciFill;

	rect.Left = 0;
	rect.Top = 1;
	rect.Right = m_nWidth - 1;
	rect.Bottom = m_nOutputHeight - 2;
	ciFill.Char.AsciiChar = ' ';
	ciFill.Attributes = 0;
	ScrollConsoleScreenBuffer(m_hStdOut, &rect, NULL, coord, &ciFill);

	SYSTEMTIME tm;

	GetLocalTime(&tm);
	plat_safe_sprintf(data, sizeof(data), "(%02d:%02d:%02d.%03d)%s", 
		tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds, str);
	this->PrintLine(m_nOutputHeight - 2, data, this->GetColor(color));
	this->WriteOutputFile(data);
	return true;
}

bool WinTerminal::Response(const char* str, const char* color)
{
	Assert(str != NULL);
	Assert(color != NULL);
	
	SMALL_RECT rect;
	COORD coord = { 0, (SHORT)m_nOutputHeight };
	CHAR_INFO ciFill;

	rect.Left = 0;
	rect.Top = m_nOutputHeight + 1;
	rect.Right = m_nWidth - 1;
	rect.Bottom = GetInputRow() - 2;
	ciFill.Char.AsciiChar = ' ';
	ciFill.Attributes = 0;
	ScrollConsoleScreenBuffer(m_hStdOut, &rect, NULL, coord, &ciFill);
	this->PrintLine(GetInputRow() - 2, str, this->GetColor(color));
	return true;
}

bool WinTerminal::WriteOutputFile(const char* info)
{
	if (m_sOutputFile.Empty())
	{
		return false;
	}

	char fname[256];

	plat_safe_sprintf(fname, sizeof(fname), "%s%s",
		frame_get_work_path(), m_sOutputFile.CString());

	FILE* fp = plat_file_open(fname, "ab");

	if (NULL == fp)
	{
		return false;
	}

	fprintf(fp, "%s\r\n", info);
	fclose(fp);
	return true;
}

bool WinTerminal::ClearOutputFile()
{
	if (m_sOutputFile.Empty())
	{
		return false;
	}
	
	char fname[256];

	plat_safe_sprintf(fname, sizeof(fname), "%s%s",
		frame_get_work_path(), m_sOutputFile.CString());

	FILE* fp = plat_file_open(fname, "wb");

	if (NULL == fp)
	{
		return false;
	}

	fclose(fp);
	return true;
}

bool WinTerminal::ProcessInput(const char* info)
{
	Assert(info != NULL);

	if (this->GetHook() == NULL)
	{
		return false;
	}
	
	FrameArgList msg;

	if (!parse_input(info, &msg))
	{
		return false;
	}
	
	if (msg.GetCount() == 0)
	{
		this->Response("invalidate input", "red");
		return false;
	}
	
	if (msg.GetType(0) != DT_STRING)
	{
		this->Response("input must string", "red");
		return false;
	}
	
	TempLongString cmd = to_lower(msg.GetString(0));
	char event[256];

	plat_safe_sprintf(event, sizeof(event), "input_%s", cmd.CString());

	if (!frame_find_event_hook(this, event))
	{
		this->Response("undefined input command", "red");
		return false;
	}

	FrameArgList args;

	args.Append(msg, 1, msg.GetCount() - 1);
	frame_process_event(this, 0, event, args, NULL);
	return true;
}
