// win_app.cpp
// Created by lulimin on 2020/2/18.

#include "../../inc/common.h"
#include "../../inc/frame_def.h"
#include "../../inc/auto_string.h"
#include "../../inc/arg_data.h"
#include "../../inc/arg_list.h"
#include "../../inc/utility.h"
#include "../../inc/auto_buffer.h"
#include "../../inc/time_counter.h"
#include "../../inc/memory_map.h"
#include "../../inc/platform.h"
#include "../../inc/thread_lock.h"
#include "../../kgraphic/i_input_service.h"
#include "../../kgraphic/i_render_service.h"
#include "../memory_link.h"
#include "../memory_list.h"
#pragma warning(disable:4200)
#include "../read_json.h"
#include "../link_creator.h"
#include "win_egl.h"
#pragma warning(disable:4091)
#include "../../ext/dbghelp/dbghelp.h"

DEFINE_ASSERTION_FAIL;

// Local data types.
typedef TAutoString<char, 128> TempString;
typedef TArgData<4> TempArgData;
typedef TArgList<8, 128> TempArgList;

// User input router.
static IInputService* s_pInputService = NULL;
// Render service.
static IRenderService* s_pRenderService = NULL;
// EGL device.
static egl_device_t* s_pEglDevice = NULL;

// Set display context function.
//static bool egl_set_context(void* param)
//{
//	return egl_device_set_context(s_pEglDevice);
//}

// Present per frame function.
static bool egl_present(void* param)
{
	return egl_device_present(s_pEglDevice);
}

// Get execute file name.
static TempString get_exe_name(const char* cmd)
{
	Assert(cmd != NULL);

	char buf[256] = { 0 };
	
	if (cmd[0] == '\"')
	{
		const char* beg = cmd + 1;
		const char* end = strchr(beg, '\"');

		if (NULL == end)
		{
			return TempString(buf);
		}

		const size_t SIZE1 = end - beg;
		
		if (SIZE1 >= sizeof(buf))
		{
			return TempString(buf);
		}

		memcpy(buf, beg, SIZE1);
		buf[SIZE1] = 0;
	} 
	else
	{
		util_string_copy(buf, sizeof(buf), cmd);

		for (char* s = buf; *s; ++s)
		{
			if ((*s == ' ') || (*s == '\t'))
			{
				*s = 0;
				break;
			}
		}
	}

	return TempString(buf);
}

// Get main configuration file name.
static TempString get_json_name(const char* exe_name)
{
	Assert(exe_name != NULL);

	char buf[256];
	char* file_part;

	plat_get_full_path(exe_name, buf, sizeof(buf) - 1, &file_part);

	if (NULL == file_part)
	{
		return TempString();
	}

	strlwr(file_part);

	char* p = strstr(file_part, ".exe");

	if (NULL == p)
	{
		return TempString();
	}

	strncpy(p, ".json", 6);
	return TempString(file_part);
}

// Split command line arguments.
static size_t get_cmd_args(const char* cmd, char* buf, size_t size,
	const char* args[], size_t arg_max)
{
	Assert(cmd != NULL);
	Assert(buf != NULL);
	Assert(size > 0);
	
	size_t argc = 0;
	
	util_string_copy(buf, size, cmd);

	char* s = buf;
	
	if (*s == '\"')
	{
		char* beg = ++s;
		
		s = strchr(beg, '\"');
		
		if (NULL == s)
		{
			return 0;
		}
		
		*s++ = 0;
		
		if (argc < arg_max)
		{
			args[argc++] = beg;
		}
	}
	else
	{
		for (; *s; ++s)
		{
			if ((*s == ' ') || (*s == '\t'))
			{
				*s++ = 0;
				break;
			}
		}
		
		if (argc < arg_max)
		{
			args[argc++] = buf;
		}
	}

	char* arg = NULL;
	
	for (; *s; ++s)
	{
		if ((*s == ' ') || (*s == '\t'))
		{
			*s = 0;
			
			if (arg != NULL)
			{
				if (argc < arg_max)
				{
					args[argc++] = arg;
				}

				arg = NULL;
			}
		}
		else
		{
			if (NULL == arg)
			{
				arg = s;
			}
		}
	}
	
	if (arg != NULL)
	{
		if (argc < arg_max)
		{
			args[argc++] = arg;
		}
	}

	return argc;
}

// Get current work path.
static TempString get_work_path(const char* exe)
{
	Assert(exe != NULL);

	char buf[256] = { 0 };
	const char* slash = strrchr(exe, '\\');

	if (NULL == slash)
	{
		slash = strrchr(exe, '/');

		if (NULL == slash)
		{
			return TempString(buf);
		}
	}
	
	const size_t SIZE1 = slash - exe + 1;

	if (SIZE1 >= sizeof(buf))
	{
		return TempString(buf);
	}

	memcpy(buf, exe, SIZE1);
	buf[SIZE1] = 0;
	return TempString(buf);
}

// Display error message box.
static void show_fatal_error(const char* info)
{
	plat_popup_error(info, "fatal error");
}

// Get key status when mouse click.
static void get_mouse_key(WPARAM wParam, unsigned int* pFlags)
{
	if (wParam & MK_CONTROL)
	{
		*pFlags |= IInputService::FLAG_CONTROL;
	}

	if (wParam & MK_SHIFT)
	{
		*pFlags |= IInputService::FLAG_SHIFT;
	}

	if (wParam & MK_LBUTTON)
	{
		*pFlags |= IInputService::FLAG_LEFT;
	}

	if (wParam & MK_MBUTTON)
	{
		*pFlags |= IInputService::FLAG_MIDDLE;
	}

	if (wParam & MK_RBUTTON)
	{
		*pFlags |= IInputService::FLAG_RIGHT;
	}
}

// Get key status.
static void get_key_state(unsigned int* pFlags)
{
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		*pFlags |= IInputService::FLAG_SHIFT;
	}

	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{
		*pFlags |= IInputService::FLAG_CONTROL;
	}

	if (GetAsyncKeyState(VK_MENU) < 0)
	{
		*pFlags |= IInputService::FLAG_ALTER;
	}
}

// Translate key value.
static unsigned int get_key_value(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_UP:
		return IInputService::KEY_UP;
	case VK_DOWN:
		return IInputService::KEY_DOWN;
	case VK_LEFT:
		return IInputService::KEY_LEFT;
	case VK_RIGHT:
		return IInputService::KEY_RIGHT;
	case VK_PRIOR:
		return IInputService::KEY_PRIOR;
	case VK_NEXT:
		return IInputService::KEY_NEXT;
	case VK_HOME:
		return IInputService::KEY_HOME;
	case VK_END:
		return IInputService::KEY_END;
	case VK_RETURN:
		return IInputService::KEY_RETURN;
	case VK_INSERT:
		return IInputService::KEY_INSERT;
	case VK_DELETE:
		return IInputService::KEY_DELETE;
	case VK_BACK:
		return IInputService::KEY_BACK;
	case VK_TAB:
		return IInputService::KEY_TAB;
	case VK_ESCAPE:
		return IInputService::KEY_ESCAPE;
	case VK_CAPITAL:
		return IInputService::KEY_CAPITAL;
	case VK_SHIFT:
		return IInputService::KEY_SHIFT;
	case VK_CONTROL:
		return IInputService::KEY_CONTROL;
	case VK_MENU:
		return IInputService::KEY_MENU;
	case VK_ADD:
		return IInputService::KEY_ADD;
	case VK_SUBTRACT:
		return IInputService::KEY_SUBTRACT;
	case VK_F1:
		return IInputService::KEY_F1;
	case VK_F2:
		return IInputService::KEY_F2;
	case VK_F3:
		return IInputService::KEY_F3;
	case VK_F4:
		return IInputService::KEY_F4;
	case VK_F5:
		return IInputService::KEY_F5;
	case VK_F6:
		return IInputService::KEY_F6;
	case VK_F7:
		return IInputService::KEY_F7;
	case VK_F8:
		return IInputService::KEY_F8;
	case VK_F9:
		return IInputService::KEY_F9;
	case VK_F10:
		return IInputService::KEY_F10;
	case VK_F11:
		return IInputService::KEY_F11;
	case VK_F12:
		return IInputService::KEY_F12;
	default:
		break;
	}

	return (unsigned int)wParam;
}

// Translate mouse message type.
static inline int get_mouse_message_type(UINT iMsg)
{
	switch (iMsg)
	{
	case WM_LBUTTONDOWN:
		return IInputService::TYPE_LEFT_DOWN;
	case WM_LBUTTONUP:
		return IInputService::TYPE_LEFT_UP;
	case WM_LBUTTONDBLCLK:
		return IInputService::TYPE_LEFT_DUAL;
	case WM_RBUTTONDOWN:
		return IInputService::TYPE_RIGHT_DOWN;
	case WM_RBUTTONUP:
		return IInputService::TYPE_RIGHT_UP;
	case WM_RBUTTONDBLCLK:
		return IInputService::TYPE_RIGHT_DUAL;
	case WM_MBUTTONDOWN:
		return IInputService::TYPE_MIDDLE_DOWN;
	case WM_MBUTTONUP:
		return IInputService::TYPE_MIDDLE_UP;
	case WM_MBUTTONDBLCLK:
		return IInputService::TYPE_MIDDLE_DUAL;
	default:
		break;
	}

	return 0;
}

// Windows message processor.
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Input code page.
	static unsigned int s_nCodePage = CP_ACP;
	// Input multi-byte character.
	static unsigned char s_nInputChar[8] = { 0 };
	static unsigned int s_nInputCount = 0;

	switch (iMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			unsigned int x = lParam & 0xFFFF;
			unsigned int y = (lParam >> 16) & 0xFFFF;
			IInputService::input_t msg;
			
			msg.nType = IInputService::TYPE_MOUSE_MOVE;
			msg.dPosX = (double)x;
			msg.dPosY = (double)y;
			msg.nFlags = 0;
			get_mouse_key(wParam, &msg.nFlags);
			s_pInputService->ProcessInput(msg);
		}
		
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			if ((iMsg == WM_LBUTTONDOWN) || (iMsg == WM_MBUTTONDOWN) ||
				(iMsg == WM_RBUTTONDOWN))
			{
				// Generate WM_MOUSELEAVE message if cursor leave client rectangle.
				TRACKMOUSEEVENT tme;

				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 0;
				TrackMouseEvent(&tme);
			}
			
			unsigned int x = lParam & 0xFFFF;
			unsigned int y = (lParam >> 16) & 0xFFFF;
			IInputService::input_t msg;

			msg.nType = get_mouse_message_type(iMsg);
			msg.dPosX = (double)x;
			msg.dPosY = (double)y;
			msg.nFlags = 0;
			get_mouse_key(wParam, &msg.nFlags);
			s_pInputService->ProcessInput(msg);
		}

		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			unsigned int sx = lParam & 0xFFFF;
			unsigned int sy = (lParam >> 16) & 0xFFFF;
			POINT sp;

			sp.x = sx;
			sp.y = sy;
			ScreenToClient(hwnd, &sp);
			
			unsigned int wheel = (wParam >> 16) & 0xFFFF;
			int delta = ((int)(short)(unsigned short)wheel) / 120;
			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_MOUSE_WHEEL;
			msg.dPosX = (double)sp.x;
			msg.dPosY = (double)sp.y;
			msg.dValue = (double)delta;
			msg.nFlags = 0;
			get_mouse_key(wParam, &msg.nFlags);
			s_pInputService->ProcessInput(msg);
		}

		break;
	}
	case WM_MOUSELEAVE:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_MOUSE_LEAVE;
			s_pInputService->ProcessInput(msg);
		}
		
		break;
	}
	case WM_KEYDOWN:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_KEY_DOWN;
			msg.nChar = get_key_value(wParam);
			msg.nFlags = 0;
			get_key_state(&msg.nFlags);
			
			if (s_pInputService->ProcessInput(msg))
			{
				return 0;
			}
		}
		
		break;
	}
	case WM_KEYUP:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_KEY_UP;
			msg.nChar = get_key_value(wParam);
			msg.nFlags = 0;
			get_key_state(&msg.nFlags);
			
			if (s_pInputService->ProcessInput(msg))
			{
				return 0;
			}
		}
		
		break;
	}
	case WM_CHAR:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			if (wParam < 256)
			{
				if ((IsDBCSLeadByteEx(s_nCodePage, (BYTE)wParam) == TRUE) &&
					(s_nInputCount == 0))
				{
					s_nInputChar[0] = (unsigned char)wParam;
					//s_nInputChar[1] = 0;
					s_nInputCount = 1;
					// Processed
					return 0;
				}
				else
				{
					s_nInputChar[s_nInputCount] = (unsigned char)wParam;
					//s_nInputChar[s_nInputCount + 1] = 0;
					s_nInputCount++;

					wchar_t wch;

					MultiByteToWideChar(s_nCodePage, 0, (LPCSTR)s_nInputChar,
						s_nInputCount, &wch, 1);
					//s_nInputChar[0] = 0;
					s_nInputCount = 0;

					IInputService::input_t msg;

					msg.nType = IInputService::TYPE_CHAR;
					msg.nChar = (unsigned int)wch;
					msg.nFlags = 0;
					get_key_state(&msg.nFlags);
					
					if (s_pInputService->ProcessInput(msg))
					{
						// Processed.
						return 0;
					}
				}
			}
			else
			{
				IInputService::input_t msg;

				msg.nType = IInputService::TYPE_CHAR;
				msg.nChar = (unsigned int)wParam;
				msg.nFlags = 0;
				get_key_state(&msg.nFlags);
				
				if (s_pInputService->ProcessInput(msg))
				{
					// Processed.
					return 0;
				}
			}
		}

		break;
	}
	case WM_SETCURSOR:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			unsigned int hit_test_code = lParam & 0xFFFF;

			if (hit_test_code != HTCLIENT)
			{
				// Not client area.
				break;
			}

			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_CURSOR;
			
			if (s_pInputService->ProcessInput(msg))
			{
				// Avoid default system processing.
				return TRUE;
			}
		}

		break;
	}
	case WM_ACTIVATE:
	{
		if (frame_get_exit())
		{
			break;
		}

		if (s_pInputService)
		{
			IInputService::input_t msg;
			unsigned int status = wParam & 0xFFFF;
			
			if ((status == WA_ACTIVE) || (status == WA_CLICKACTIVE))
			{
				msg.nType = IInputService::TYPE_ACTIVE;
			}
			else if (status == WA_INACTIVE)
			{
				msg.nType = IInputService::TYPE_INACTIVE;
			}
			else
			{
				break;
			}
			
			s_pInputService->ProcessInput(msg);
		}

		break;
	}
	case WM_INPUTLANGCHANGE:
	{
		CHARSETINFO csi;

		TranslateCharsetInfo((DWORD*)wParam, &csi, TCI_SRCCHARSET);
		s_nCodePage = csi.ciACP;
		break;
	}
	case WM_CLOSE:
	{
		if (s_pInputService)
		{
			IInputService::input_t msg;

			msg.nType = IInputService::TYPE_CLOSE;
			s_pInputService->ProcessInput(msg);
		}

		// Avoid window close automatic.
		return 0;
	}
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// Retrieve full path and validate it.
static const char* parse_full_path(const char* src_path, const char* def_path,
	char* buffer, size_t buffer_size)
{
	Assert(src_path != NULL);
	Assert(def_path != NULL);
	Assert(buffer != NULL);
	
	if (util_string_empty(src_path))
	{
		return def_path;
	}
	
	char* file_part = NULL;

	plat_get_full_path(src_path, buffer, buffer_size - 1, &file_part);

	if (!plat_find_dir(buffer))
	{
		char info[256];

		plat_safe_sprintf(info, sizeof(info), 
			"(parse_full_path)Path %s not found.", buffer);
		show_fatal_error(info);
		abort();
		return "";
	}

	char* p = buffer + strlen(buffer);

	p[0] = PLAT_PATH_SEPARATOR;
	p[1] = 0;
	return buffer;
}

// Trim string space.
static size_t trim_string(char* dst, size_t dst_size, const char* src,
	size_t src_len)
{
	const char* beg = src;
	const char* end = src + src_len - 1;

	for (; beg <= end; beg++)
	{
		if ((*beg != ' ') && (*beg != '\t'))
		{
			break;
		}
	}

	for (; end >= beg; end--)
	{
		if ((*end != ' ') && (*end != '\t'))
		{
			break;
		}
	}

	size_t size = end - beg + 1;

	if (size >= dst_size)
	{
		size = dst_size - 1;
	}

	memcpy(dst, beg, size);
	dst[size] = 0;
	return size;
}

// Create frame and main window.
static void create_frame(HINSTANCE hInstance, bool* pProfiler, int* pSleep)
{
	Assert(pProfiler != NULL);
	Assert(pSleep != NULL);
	
	// Command line.
	const char* cmd = GetCommandLineA();
	char cmd_line[256];

	trim_string(cmd_line, sizeof(cmd_line), cmd, strlen(cmd));
	
	// Execute file name.
	TempString exe_name = get_exe_name(cmd_line);
	// Current work path.
	TempString work_path = get_work_path(exe_name.CString());

	if (work_path.Empty())
	{
		char path[256];

		plat_get_current_dir(path, sizeof(path));
		work_path = TempString(path) + "/";
	}

	plat_set_current_dir(work_path.CString());

	// Parse arguments.
	char buf[256];
	const char* argv[32];
	size_t argc = get_cmd_args(cmd_line, buf, sizeof(buf), argv, 32);
	
	// Get configuration file name.
	TempString json_name = get_json_name(exe_name.CString());

	if (json_name.Empty())
	{
		json_name = "kclient.json";
	}

	if (argc > 1)
	{
		json_name = argv[1];
	}

	// Startup arguments.
	TempArgList args;

	if (argc > 1)
	{
		for (size_t i = 1; i < argc; ++i)
		{
			args.AddString(argv[i]);
		}
	}

	FILE* fp = fopen(json_name.CString(), "rb");

	if (NULL == fp)
	{
		show_fatal_error("Open json file failed.");
		abort();
		return;
	}

	fseek(fp, 0, SEEK_END);
	long json_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	TAutoBuffer<char, 1024> auto_buf(json_size);
	char* json_data = auto_buf.GetBuffer();

	if (fread(json_data, 1, json_size, fp) != json_size)
	{
		fclose(fp);
		show_fatal_error("Read json file error.");
		abort();
		return;
	}

	fclose(fp);

	TAutoBuffer<size_t, 1024> auto_buf1(json_size);
	size_t* pJsonMem = auto_buf1.GetBuffer();
	const sajson::document& doc = sajson::parse(
		sajson::bounded_allocation(pJsonMem, json_size),
		sajson::mutable_string_view(json_size, json_data));

	if (!doc.is_valid())
	{
		show_fatal_error("Parse json file error.");
		abort();
		return;
	}

	const sajson::value& root = doc.get_root();
	frame_preset_t preset;
	// Load paths.
	char script_path[256];
	char coder_path[256];
	char asset_path[256];
	char output_path[256];
	TempString s1 = read_json_string(root, "ScriptPath");
	TempString s2 = read_json_string(root, "CoderPath");
	TempString s3 = read_json_string(root, "AssetPath");
	TempString s4 = read_json_string(root, "OutputPath");

	// Set paths.
	preset.szWorkPath = work_path.CString();
	preset.szScriptPath = parse_full_path(s1.CString(), work_path.CString(),
		script_path, sizeof(script_path));
	preset.szCoderPath = parse_full_path(s2.CString(), work_path.CString(),
		coder_path, sizeof(coder_path));
	preset.szAssetPath = parse_full_path(s3.CString(), work_path.CString(),
		asset_path, sizeof(asset_path));
	preset.szOutputPath = parse_full_path(s4.CString(), work_path.CString(),
		output_path, sizeof(output_path));

	// Component configuration.
	IServiceCreator* pSvcCreator = NULL;
	IObjectCreator* pObjCreator = NULL;

	link_creators(&pSvcCreator, &pObjCreator);
	LINK_OBJECT(&pObjCreator, WinTerminal);

	// Name of main object.
	preset.szMainObject = read_json_string(root, "MainObject");
	// File access interface.
	preset.pFileAccess = NULL;
	// Service creators.
	preset.pServiceCreator = pSvcCreator;
	// Object creators.
	preset.pObjectCreator = pObjCreator;
	// Reload script file when changed.
	preset.bScriptReload = read_json_boolean(root, "ScriptReload");
	// Profile script performance.
	preset.nScriptProfile = 0;

	// Performance profiler.
	*pProfiler = read_json_boolean(root, "Profiler");
	// Sleep time per frame.
	*pSleep = read_json_int32(root, "Sleep");

	// Hide main window.
	bool hide_window = read_json_boolean(root, "HideWindow");
	// Memory validate.
	int memory_check = read_json_int32(root, "MemoryCheck");

	if (memory_check == 1)
	{
		memory_link_init();
		// Create frame.
		frame_create(memory_link_debug_alloc, memory_link_alloc,
			memory_link_realloc, memory_link_free, memory_link_dump);
	}
	else if (memory_check == 2)
	{
		memory_list_init();
		// Create frame.
		frame_create(memory_list_debug_alloc, memory_list_alloc,
			memory_list_realloc, memory_list_free, memory_list_dump);
	}
	else
	{
		// Create frame.
		frame_create(NULL, NULL, NULL, NULL, NULL);
	}

	frame_clear_log();
	// Avoid process message at starting.
	frame_set_exit(true);

	// Create main window.
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = wnd_proc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(WORD);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "kclient";
	wndclass.hIconSm = NULL;

	RegisterClassEx(&wndclass);

	// Initialize window size.
	const DWORD WIN_WIDTH = 1440;
	const DWORD WIN_HEIGHT = 900;
	HWND hwnd = CreateWindowA("kclient", "kclient", WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2,
		WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hide_window)
	{
		ShowWindow(hwnd, SW_HIDE);
	}
	else
	{
		ShowWindow(hwnd, SW_NORMAL);
	}

	s_pEglDevice = egl_device_create();
	
	if (!egl_device_initialize(s_pEglDevice, hwnd))
	{
		show_fatal_error("EGL initialize failed.");
		abort();
	}

	if (!egl_device_set_context(s_pEglDevice))
	{
		show_fatal_error("EGL set context failed.");
		abort();
	}

	// Record application and main window handle.
	frame_set_global_value("_AppInstance", TempArgData(DT_POINTER, hInstance));
	frame_set_global_value("_AppWindow", TempArgData(DT_POINTER, hwnd));
	// Can process message now.
	frame_set_exit(false);

	// Start frame
	if (!frame_startup(preset))
	{
		show_fatal_error("Frame startup failed.");
		abort();
	}

	// Get user input service.
	s_pInputService = (IInputService*)frame_get_service("FlatInputService");
	
	if (NULL == s_pInputService)
	{
		show_fatal_error("No input service.");
		abort();
	}

	// Get render service.
	s_pRenderService = (IRenderService*)frame_get_service("RenderService");

	if (s_pRenderService)
	{
		RECT rect;

		GetClientRect(hwnd, &rect);

		int client_width = rect.right - rect.left;
		int client_height = rect.bottom - rect.top;

		s_pRenderService->SetWindowSize(client_width, client_height);
		s_pRenderService->SetEglFunction(egl_present, NULL);
		//s_pRenderService->SetSafeArea(16, 16, 16, 16);
		//s_pRenderService->SetEglFunction(egl_set_context, egl_present, NULL);
		//s_pRenderService->SetEglSetContextFunc(egl_set_context);
		//s_pRenderService->SetEglPresentFunc(egl_present);
	}
	else
	{
		show_fatal_error("No render service.");
		abort();
	}

	// Name of main script.
	const char* main_script = read_json_string(root, "MainScript");

	// Execute main script.
	if (!util_string_empty(main_script))
	{
		IObject* pMainObj = frame_get_main_object();

		if (NULL == pMainObj)
		{
			show_fatal_error("No main object.");
			abort();
		}
		
		bool succeed = frame_run_routine(main_script, "main",
			pMainObj->GetUID(), args, NULL);

		if (!succeed)
		{
			show_fatal_error("Run main script failed.");
			abort();
		}
	}
}

// Delete frame.
static void delete_frame()
{
	if (s_pInputService)
	{
		frame_release_service(s_pInputService);
		s_pInputService = NULL;
	}

	if (s_pRenderService)
	{
		frame_release_service(s_pRenderService);
		s_pRenderService = NULL;
	}

	// Shutdown frame.
	frame_shutdown();
	// Delete frame.
	frame_delete();
	// Record memory leak.
	memory_list_dump("memory_list_leak.log");
	memory_list_shut();
	memory_link_dump("memory_link_leak.log");
	memory_link_shut();

	if (s_pEglDevice)
	{
		egl_device_shutdown(s_pEglDevice);
		egl_device_delete(s_pEglDevice);
		s_pEglDevice = NULL;
	}
}

// Exception filter.
long WINAPI ca_unhandled_exception_filter(EXCEPTION_POINTERS* ExceptionInfo)
{
	SYSTEMTIME tm;

	GetLocalTime(&tm);

	char fname[64] = { 0 };

	_snprintf_s(fname, sizeof(fname) - 1,
		"memory%04d%02d%02d%02d%02d%02d%03d.dmp", tm.wYear, tm.wMonth, tm.wDay,
		tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

	HANDLE hFile = CreateFile(fname, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mei;

		mei.ThreadId = GetCurrentThreadId();
		mei.ExceptionPointers = ExceptionInfo;
		mei.ClientPointers = TRUE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hFile, MiniDumpNormal, &mei, NULL, NULL);
		CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef K_DEBUG
#define LEAK_DETECTOR_FILE "memory_kclient.log"

// Memory information output function.
static int ReportingFunction(int reportType, char* userMessage, int* retVal)
{
	struct leak_info_t
	{
		char szInfo[128];
	};

	// Data cache boost write file speed.
	static leak_info_t s_LeakInfos[256];
	static size_t s_nInfoCount = 0;

	size_t msg_len = strlen(userMessage);

	if (msg_len < 128)
	{
		memcpy(s_LeakInfos[s_nInfoCount].szInfo, userMessage, msg_len + 1);
	}
	else
	{
		memcpy(s_LeakInfos[s_nInfoCount].szInfo, userMessage, 127);
		s_LeakInfos[s_nInfoCount].szInfo[127] = 0;
	}

	++s_nInfoCount;

	if ((s_nInfoCount == 256) ||
		(strcmp(userMessage, "Object dump complete.\n") == 0))
	{
		try
		{
			HANDLE handle = CreateFile(LEAK_DETECTOR_FILE, GENERIC_WRITE,
				FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);

			if (handle != INVALID_HANDLE_VALUE)
			{
				SetFilePointer(handle, 0, NULL, FILE_END);

				DWORD writen = 0;

				for (size_t i = 0; i < s_nInfoCount; ++i)
				{
					char* info = s_LeakInfos[i].szInfo;
					size_t len = strlen(info);

					if (0 == len)
					{
						continue;
					}

					WriteFile(handle, info, DWORD(len - 1), &writen, NULL);

					if (info[len - 1] == '\n')
					{
						WriteFile(handle, "\r\n", 2, &writen, NULL);
					}
				}

				CloseHandle(handle);
			}
		}
		catch (...)
		{
			s_nInfoCount = 0;
			return FALSE;
		}

		s_nInfoCount = 0;
	}

	retVal = 0;

	if (reportType == _CRT_ASSERT)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif // K_DEBUG

// Register process to share memory.
static bool register_process(MemoryMap* mm)
{
	Assert(mm != NULL);
	
	bool exists = false;

	mm->Create("kclient_share_process_count", sizeof(unsigned int), &exists);

	unsigned int* pCount = (unsigned int*)mm->GetMemory();
	unsigned int index = exists ? *pCount : 0;

	*pCount = index + 1;

	SYSTEM_INFO si;

	GetSystemInfo(&si);
	// Main process thread bind CPU core guarantee timer correct.
	SetThreadAffinityMask(GetCurrentThread(), 
		DWORD_PTR(0x1) << (index % si.dwNumberOfProcessors));
	return true;
}

// Windows main entry point.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)
{
	if (IsDebuggerPresent() == FALSE)
	{
		SetUnhandledExceptionFilter(ca_unhandled_exception_filter);
	}

#ifdef K_DEBUG
	_CrtMemState old_mem_state;

	_CrtMemCheckpoint(&old_mem_state);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CRT_REPORT_HOOK old_mem_hook = _CrtSetReportHook(ReportingFunction);
#endif // K_DEBUG

	// Performance profiler.
	bool profiler = false;
	// Sleep milliseconds.
	int sleep_ms = 0;

	// Setup frame thread key if in multi-threaded mode.
	frame_multithread_setup();
	// Create frame and main window.
	create_frame(hInstance, &profiler, &sleep_ms);

	// Register process.
	MemoryMap process_mmap;

	register_process(&process_mmap);

	// Main timer.
	TimeCounter timer;

	timer.SetHighPrecision(true);
	timer.Create();

	// Main loop cycle.
	if (profiler)
	{
		// Time counters.
		IArgData* total_val = NULL;
		IArgData* message_val = NULL;
		IArgData* routine_val = NULL;
		IArgData* work_val = NULL;

		frame_set_global_value("_TotalTime", TempArgData(DT_DOUBLE, 0.0));
		frame_set_global_value("_MessageTime", TempArgData(DT_DOUBLE, 0.0));
		frame_set_global_value("_RoutineTime", TempArgData(DT_DOUBLE, 0.0));
		frame_set_global_value("_WorkTime", TempArgData(DT_DOUBLE, 0.0));
		total_val = frame_get_global_value("_TotalTime");
		message_val = frame_get_global_value("_MessageTime");
		routine_val = frame_get_global_value("_RoutineTime");
		work_val = frame_get_global_value("_WorkTime");

		for (;;)
		{
			double t0 = plat_get_time_count();
			MSG	msg;

			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (WM_QUIT == msg.message)
				{
					// Close frame.
					frame_set_exit(true);
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}
			}

			double message_time = plat_get_time_count() - t0;

			if (frame_get_exit())
			{
				break;
			}

			frame_period_set_time(timer.GetElapseSeconds());

			double t1 = plat_get_time_count();

			frame_period_check_routine();

			double routine_time = plat_get_time_count() - t1;
			double t2 = plat_get_time_count();

			frame_period_work();

			double work_time = plat_get_time_count() - t2;
			double total_time = plat_get_time_count() - t0;

			// Update performance profiler.
			total_val->SetDouble(total_val->GetDouble() + total_time);
			message_val->SetDouble(message_val->GetDouble() + message_time);
			routine_val->SetDouble(routine_val->GetDouble() + routine_time);
			work_val->SetDouble(work_val->GetDouble() + work_time);

			if (sleep_ms >= 0)
			{
				plat_sleep(sleep_ms);
			}
		}
	}
	else
	{
		for (;;)
		{
			MSG	msg;

			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (WM_QUIT == msg.message)
				{
					// Close frame.
					frame_set_exit(true);
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}
			}

			if (frame_get_exit())
			{
				break;
			}

			frame_period_set_time(timer.GetElapseSeconds());
			frame_period_check_routine();
			frame_period_work();

			if (sleep_ms >= 0)
			{
				plat_sleep(sleep_ms);
			}
		}
	}
	
	// Delete frame at ended.
	delete_frame();
	// Clean frame thread key if in multi-threaded mode.
	frame_multithread_clean();

#ifdef K_DEBUG
	// Dump memory heap status.
	_CrtMemState mem_state;
	_CrtMemState diff;

	_CrtMemCheckpoint(&mem_state);
	_CrtMemDifference(&diff, &old_mem_state, &mem_state);
	DeleteFile(LEAK_DETECTOR_FILE);
	_CrtMemDumpStatistics(&diff);
	_CrtMemDumpAllObjectsSince(&diff);
	_CrtSetReportHook(old_mem_hook);
#endif // K_DEBUG
	return 1;
}
