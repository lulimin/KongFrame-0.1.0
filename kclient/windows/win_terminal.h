// win_terminal.h
// Created by lulimin on 2019/2/19.

#ifndef __WIN_TERMINAL_H
#define __WIN_TERMINAL_H

#include "../../inc/frame_def.h"
#include "../../inc/frame_types.h"
#include "../../inc/pod_array.h"
#include <windows.h>

typedef TAutoString<char, 128, TCharTraits<char>, FrameAlloc> TempLongString;

// Windows terminal.

class WinTerminal : public IObject
{
public:
	WinTerminal();
	virtual ~WinTerminal();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();
	virtual void Work(double frame_time);
	
	// Command prompt symbols.
	void SetPromptString(const char* value);
	const char* GetPromptString() const;

	// Screen content save to log file.
	void SetOutputFile(const char* value);
	const char* GetOutputFile() const;

	// Color of text.
	void SetTextColor(const char* value);
	const char* GetTextColor() const;

	// Clear screen.
	bool Clear();
	// Analogue keyboard input.
	bool Input(const char* info);
	// Output information to top of screen.
	bool Output(const char* str, const char* color);
	// Output information to bottom of screen.
	bool Response(const char* str, const char* color);
	// Clear log file.
	bool ClearOutputFile();

private:
	// Set screen size and layout.
	void SetSize(int height, int width, int logheight);
	// Set default color.
	void SetColor(int color);
	// Output line to screen.
	void PrintLine(int line, const char* string, unsigned int color);
	// Write information to log file.
	bool WriteOutputFile(const char* info);
	// Get output character color.
	int GetColor(const char* color) const;
	// Get input line row number.
	int GetInputRow() const;
	// Process input information.
	bool ProcessInput(const char* info);
	// Reover history input information.
	void RecoverInput();
	// Update input line.
	void UpdateInput();
	// Analogue input character.
	void InputChar(char ch);
	// Delete character.
	void DeleteChar();
	// Delete last input character.
	void BackspaceChar();

private:
	HANDLE m_hStdOut;
	HANDLE m_hStdIn;
	HWND m_hWindow;
	bool m_bOpened;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nOutputHeight;
	unsigned int m_nTextColor;
	char m_szInputBuffer[256];
	int m_nInputCount;
	int m_nRecoverIndex;
	int m_nCursorPos;
	TempLongString m_sPromptString;
	TempLongString m_sOutputFile;
	TPodArray<char*, 1, FrameAlloc> m_Histories;
};

#endif // __WIN_TERMINAL_H
