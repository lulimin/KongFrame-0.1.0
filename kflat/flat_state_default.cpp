// flat_state_default.cpp
// Created by lulimin on 2020/12/21.

#include "flat_state.h"
#include "flat_ui.h"
#include "flat_widget.h"
#include "flat_label.h"
#include "flat_menu.h"
#include "../kgraphic/i_input_service.h"
#include "../kgraphic/i_render_service.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_types.h"
#include "../inc/auto_buffer.h"

// Clear hint label.
bool flat_clear_hint(flat_state_t* pState);

// Change capture widget.
bool flat_change_capture(flat_state_t* pState, FlatWidget* pNewCapture)
{
	Assert(pState != NULL);
	
	FlatWidget* pOldCapture = flat_get_widget(pState->CaptureID);

	if (pNewCapture == pOldCapture)
	{
		return false;
	}

	flat_clear_hint(pState);

	if (pOldCapture)
	{
		pOldCapture->SetCapture(false);
		pOldCapture->OnLostCapture();
	}

	if (pNewCapture)
	{
		pState->CaptureID = pNewCapture->GetUID();
		pNewCapture->SetCapture(true);
		pNewCapture->OnGotCapture();
	}
	else
	{
		pState->CaptureID = uniqid_t();
	}
	
	return true;
}

// Change focus widget.
bool flat_change_focus(flat_state_t* pState, FlatWidget* pNewFocus)
{
	Assert(pState != NULL);
	
	FlatWidget* pOldFocus = flat_get_widget(pState->FocusedID);

	if (pNewFocus == pOldFocus)
	{
		return false;
	}

	if (pNewFocus)
	{
		if (!pNewFocus->GetEnabled())
		{
			// Disabled.
			return false;
		}

		if (!pNewFocus->GetCanFocus())
		{
			// Cannot focus.
			return false;
		}
	}

	if (pOldFocus)
	{
		pOldFocus->SetFocused(false);
		pOldFocus->OnLostFocus();
	}

	if (pNewFocus)
	{
		pState->pFlatUI->ResetCaret();
		pState->FocusedID = pNewFocus->GetUID();
		pNewFocus->SetFocused(true);
		pNewFocus->OnGotFocus();
	}
	else
	{
		pState->FocusedID = uniqid_t();
	}

	return true;
}

// Clear hint label.
bool flat_clear_hint(flat_state_t* pState)
{
	Assert(pState != NULL);

	//if (uniqid_is_null(pState->HintOwnID))
	if (uniqid_is_null(pState->HintLabelID))
	{
		return false;
	}

	FlatUI* pFlatUI = pState->pFlatUI;
	//FlatLabel* pHintLabel = pFlatUI->GetHintLabel();

	//pFlatUI->RemoveFloating(pHintLabel->GetUID());
	pFlatUI->RemoveFloating(pState->HintLabelID);
	pState->HintLabelID = uniqid_t();
	pState->HintOwnID = uniqid_t();
	pState->fTimeCount = 0.0F;
	return true;
}

// Split hint name and text.
const char* parse_hint_info(const char* hint_info, char* hint_name,
	size_t hint_name_size)
{
	Assert(hint_info != NULL);
	Assert(hint_name != NULL);
	Assert(hint_name_size > 0);

	if (hint_info[0] != '@')
	{
		hint_name[0] = 0;
		return hint_info;
	}

	const char* at = strchr(hint_info + 1, '@');

	if (NULL == at)
	{
		hint_name[0] = 0;
		return hint_info;
	}

	size_t size = at - hint_info - 1;

	if (size >= hint_name_size)
	{
		hint_name[0] = 0;
		return hint_info;
	}

	memcpy(hint_name, hint_info + 1, size);
	hint_name[size] = 0;
	return at + 1;
}

// Display hint label.
bool flat_display_hint(flat_state_t* pState, FlatWidget* pWidget, int x, int y)
{
	Assert(pState != NULL);
	Assert(pWidget != NULL);

	const char* hint_info = pWidget->OnDisplayHint(x, y);

	if (NULL == hint_info)
	{
		flat_clear_hint(pState);
		return false;
	}

	if (util_string_empty(hint_info))
	{
		flat_clear_hint(pState);
		return false;
	}

	FlatUI* pFlatUI = pState->pFlatUI;
	char hint_name[32];
	const char* hint_text = parse_hint_info(hint_info, hint_name,
		sizeof(hint_name));
	FlatLabel* pHintLabel = pFlatUI->GetHintLabel(hint_name);

	if (uniqid_not_null(pState->HintOwnID))
	{
		if (uniqid_equal(pState->HintOwnID, pWidget->GetUID()))
		{
			if (strcmp(pHintLabel->GetText(), hint_text) == 0)
			{
				// No any change.
				return true;
			}
		}

		flat_clear_hint(pState);
	}

	flat_canvas_t* pCanvas = pFlatUI->GetCanvas();
	const char* hint_font = pHintLabel->GetFont();

	if (!util_string_empty(hint_font))
	{
		pFlatUI->SetCurrentFont(hint_font);
	}

	int hint_width = canvas_get_text_width(pCanvas, hint_text, 0, 0);
	int hint_height = canvas_get_text_height(pCanvas, hint_text);

	pHintLabel->SetLeft(x);
	pHintLabel->SetTop(y);
	pHintLabel->SetSize(hint_width + 4, hint_height + 4);
	pHintLabel->SetText(hint_text);
	pFlatUI->AddFloating(pHintLabel->GetUID());
	pState->HintLabelID = pHintLabel->GetUID();
	pState->HintOwnID = pWidget->GetUID();
	return true;
}

// Clear context menu.
bool flat_clear_context_menu(flat_state_t* pState)
{
	Assert(pState != NULL);

	if (uniqid_is_null(pState->ContextMenuID))
	{
		return false;
	}

	FlatUI* pFlatUI = pState->pFlatUI;

	pFlatUI->RemoveFloating(pState->ContextMenuID);
	pState->ContextMenuID = uniqid_t();
	pState->ContextOwnID = uniqid_t();
	return true;
}

// Display context menu.
bool flat_display_context_menu(flat_state_t* pState, FlatWidget* pWidget,
	int x, int y)
{
	Assert(pState != NULL);
	Assert(pWidget != NULL);
	
	const char* context = pWidget->OnDisplayContextMenu(x, y);

	if (NULL == context)
	{
		flat_clear_context_menu(pState);
		return false;
	}

	if (util_string_empty(context))
	{
		flat_clear_context_menu(pState);
		return false;
	}

	FlatUI* pFlatUI = pState->pFlatUI;
	FlatMenu* pMenu = pFlatUI->GetContextMenu(context);

	if (NULL == pMenu)
	{
		flat_clear_context_menu(pState);
		return false;
	}

	if (uniqid_not_null(pState->ContextOwnID))
	{
		if (uniqid_equal(pState->ContextOwnID, pWidget->GetUID()))
		{
			if (strcmp(pMenu->GetName(), context) == 0)
			{
				// No any change.
				return true;
			}
		}

		flat_clear_hint(pState);
	}

	IRenderService* pRS = pFlatUI->GetRenderService();
	int render_width = pRS->GetRenderWidth();
	int render_height = pRS->GetRenderHeight();
	int menu_x = x;
	int menu_y = y;

	if ((x + pMenu->GetWidth()) > render_width)
	{
		menu_x = x - pMenu->GetWidth();
	}

	if ((y + pMenu->GetHeight()) > render_height)
	{
		menu_y = y - pMenu->GetHeight();
	}

	pMenu->SetLeft(menu_x);
	pMenu->SetTop(menu_y);
	pMenu->SetOwner(pWidget->GetUID());
	pFlatUI->AddFloating(pMenu->GetUID());
	pState->ContextMenuID = pMenu->GetUID();
	pState->ContextOwnID = pWidget->GetUID();
	pWidget->OnContextMenu(pMenu->GetUID());
	return true;
}

// Check floating widgets.
FlatWidget* flat_check_floating(flat_state_t* pState, int x, int y)
{
	Assert(pState != NULL);
	
	FlatUI* pFlatUI = pState->pFlatUI;
	size_t floating_size = pFlatUI->GetFloatingSize();
	const uniqid_t* floating_data = pFlatUI->GetFloatingData();
	TAutoBuffer<uniqid_t, 256, FrameAlloc> auto_buf(floating_size);
	uniqid_t* floatings = auto_buf.GetBuffer();

	memcpy(floatings, floating_data, sizeof(uniqid_t) * floating_size);

	FlatWidget* pInWidget = NULL;

	for (size_t i = 0; i < floating_size; ++i)
	{
		uniqid_t id = floatings[i];
		FlatWidget* pFloating = flat_get_widget(id);

		if (NULL == pFloating)
		{
			pFlatUI->RemoveFloating(id);
			continue;
		}

		FlatWidget* pWidget = pFloating->GetRegionOf(x, y);

		if (pWidget)
		{
			pInWidget = pWidget;
		}
		else
		{
			FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

			if (pTopDialog)
			{
				pWidget = pTopDialog->GetRegionOf(x, y);

				if (pWidget)
				{
					FlatWidget* pDelegator = pWidget->GetRootDelegator();

					if (pDelegator == pFloating->GetDelegator())
					{
						// Belong to same delegator.
						continue;
					}
				}
			}
			
			pFlatUI->RemoveFloating(id);
		}
	}
	
	return pInWidget;
}

// Process mouse move.
static int process_mouse_move(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	pState->nMouseX = x;
	pState->nMouseY = y;

	FlatUI* pFlatUI = pState->pFlatUI;
	
	if (!pFlatUI->GetVisible())
	{
		return 0;
	}

	size_t floating_size = pFlatUI->GetFloatingSize();
	const uniqid_t* floating_data = pFlatUI->GetFloatingData();
	FlatWidget* pWidget = NULL;

	for (size_t i = 0; i < floating_size; ++i)
	{
		FlatWidget* pFloating = flat_get_widget(floating_data[i]);

		pWidget = pFloating->GetRegionOf(x, y);

		if (pWidget)
		{
			break;
		}
	}

	if (NULL == pWidget)
	{
		FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

		if (NULL == pTopDialog)
		{
			return 0;
		}

		pWidget = pTopDialog->GetRegionOf(x, y);
	}

	flat_change_capture(pState, pWidget);

	if (pWidget)
	{
		pFlatUI->SetCurrentCursor(pWidget->GetCursor());
		pWidget->OnMouseMove(x, y, flags);
	}

	return 1;
}

// Process mouse wheel.
static int process_mouse_wheel(flat_state_t* pState, int x, int y,
	double delta, unsigned int flags)
{
	Assert(pState != NULL);

	FlatUI* pFlatUI = pState->pFlatUI;

	if (!pFlatUI->GetVisible())
	{
		return 0;
	}

	FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

	if (NULL == pTopDialog)
	{
		return 0;
	}

	FlatWidget* pWidget = pTopDialog->GetRegionOf(x, y);

	if (pWidget)
	{
		FlatWidget* pRootDelegator = pWidget->GetRootDelegator();
		
		if (pRootDelegator->OnMouseWheel(x, y, delta, flags) == 0)
		{
			pWidget->OnMouseWheel(x, y, delta, flags);
		}
	}
	
	return 1;
}

// Process mouse leave.
static int process_mouse_leave(flat_state_t* pState)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse left button down.
static int process_left_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	pState->nMouseX = x;
	pState->nMouseY = y;
	pState->nClickX = x;
	pState->nClickY = y;
	
	FlatUI* pFlatUI = pState->pFlatUI;

	if (!pFlatUI->GetVisible())
	{
		return 0;
	}

	FlatWidget* pWidget = flat_check_floating(pState, x, y);

	// Clear context menu after check.
	flat_clear_context_menu(pState);
	
	if (NULL == pWidget)
	{
		FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

		if (NULL == pTopDialog)
		{
			return 0;
		}

		pWidget = pTopDialog->GetRegionOf(x, y);
	}

	if (pWidget)
	{
		FlatWidget* pRootDelegator = pWidget->GetRootDelegator();
		
		flat_change_focus(pState, pRootDelegator);

		if (pWidget->GetDesignMode())
		{
			bool ctrl = (flags & IInputService::FLAG_CONTROL) != 0;
			bool shift = (flags & IInputService::FLAG_SHIFT) != 0;
			
			frame_process_event(pFlatUI, 0, "design_pick",
				FrameArgList() << x << y << pWidget->GetUID() << ctrl << shift,
				NULL);
			
			int res = pWidget->OnLeftDown(x, y, flags);
			
			pState->ClickedID = pWidget->GetUID();
			flat_state_switch(pState, FLAT_STATE_PICK_DOWN);
			return res;
		}
		
		flat_change_capture(pState, pWidget);

		int res = pWidget->OnLeftDown(x, y, flags);
		
		pState->ClickedID = pWidget->GetUID();
		flat_state_switch(pState, FLAT_STATE_LEFT_DOWN);
		return res;
	}
	else
	{
		flat_change_focus(pState, NULL);
	}

	return 0;
}

// Process mouse left button up.
static int process_left_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse left button dual click.
static int process_left_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	pState->nMouseX = x;
	pState->nMouseY = y;
	pState->nClickX = x;
	pState->nClickY = y;

	FlatUI* pFlatUI = pState->pFlatUI;

	if (!pFlatUI->GetVisible())
	{
		return 0;
	}

	FlatWidget* pWidget = flat_check_floating(pState, x, y);

	// Clear context menu after check.
	flat_clear_context_menu(pState);

	if (NULL == pWidget)
	{
		FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

		if (NULL == pTopDialog)
		{
			return 0;
		}

		pWidget = pTopDialog->GetRegionOf(x, y);
	}

	if (pWidget)
	{
		FlatWidget* pRootDelegator = pWidget->GetRootDelegator();

		flat_change_focus(pState, pRootDelegator);

		int res = pWidget->OnLeftDual(x, y, flags);

		pState->ClickedID = pWidget->GetUID();
		flat_state_switch(pState, FLAT_STATE_LEFT_DOWN);
		return res;
	}
	else
	{
		flat_change_focus(pState, NULL);
	}

	return 0;
}

// Process mouse right button down.
static int process_right_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	FlatUI* pFlatUI = pState->pFlatUI;

	if (!pFlatUI->GetVisible())
	{
		return 0;
	}

	//FlatWidget* pWidget = flat_check_floating(pState, x, y);

	flat_check_floating(pState, x, y);
	
	FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

	if (NULL == pTopDialog)
	{
		return 0;
	}

	FlatWidget* pWidget = pTopDialog->GetRegionOf(x, y);

	flat_clear_context_menu(pState);
	
	if (pWidget)
	{
		flat_display_context_menu(pState, pWidget, x, y);
	}

	return 0;
}

// Process mouse right button up.
static int process_right_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse right button dual click.
static int process_right_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse middle button down.
static int process_middle_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse middle buton up.
static int process_middle_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process mouse middle button dual click.
static int process_middle_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	Assert(pState != NULL);

	return 0;
}

// Process tab key.
static int process_tab(flat_state_t* pState)
{
	Assert(pState != NULL);
	
	FlatWidget* pTopDialog = pState->pFlatUI->GetTopDialog();

	if (NULL == pTopDialog)
	{
		return 0;
	}

	flat_container_t* pContainer = pTopDialog->GetContainer();

	if (NULL == pContainer)
	{
		return 0;
	}

	size_t child_size = flat_container_get_size(pContainer);
	FlatWidget** child_data = flat_container_get_data(pContainer);
	FlatWidget* pFocused = flat_get_widget(pState->FocusedID);
	int tab_index = 0;

	if (pFocused)
	{
		int focused_tab_index = pFocused->GetTabIndex();

		// Search tab index large than current focused.
		for (size_t k = 0; k < child_size; ++k)
		{
			FlatWidget* pChild = child_data[k];

			if (NULL == pChild)
			{
				continue;
			}

			if (!pChild->GetCanFocus())
			{
				continue;
			}

			if (pChild->GetTabIndex() > focused_tab_index)
			{
				tab_index = focused_tab_index;
				break;
			}
		}
	}

	for (size_t i = 0; i < child_size; ++i)
	{
		FlatWidget* pChild = child_data[i];

		if (NULL == pChild)
		{
			continue;
		}

		if (!pChild->GetCanFocus())
		{
			continue;
		}

		if (pChild->GetTabIndex() > tab_index)
		{
			flat_change_focus(pState, pChild);
			return 1;
		}
	}

	return 0;
}

// Process key down.
static int process_key_down(flat_state_t* pState, unsigned int key,
	unsigned int flags)
{
	Assert(pState != NULL);

	if (key == IInputService::KEY_TAB)
	{
		int res = process_tab(pState);
		
		if (res != 0)
		{
			return 1;
		}
	}
	
	FlatWidget* pFocused = flat_get_widget(pState->FocusedID);

	if (pFocused)
	{
		int res = pFocused->OnKeyDown(key, flags);

		return res;
	}

	return 0;
}

// Process key up.
static int process_key_up(flat_state_t* pState, unsigned int key,
	unsigned int flags)
{
	Assert(pState != NULL);

	FlatWidget* pFocused = flat_get_widget(pState->FocusedID);

	if (pFocused)
	{
		int res = pFocused->OnKeyUp(key, flags);

		return res;
	}

	return 0;
}

// Process input character.
static int process_char(flat_state_t* pState, unsigned int ch,
	unsigned int flags)
{
	Assert(pState != NULL);

	FlatWidget* pFocused = flat_get_widget(pState->FocusedID);

	if (pFocused)
	{
		int res = pFocused->OnChar(ch, flags);

		return res;
	}

	return 0;
}

// Process window active.
static int process_active(flat_state_t* pState)
{
	return 0;
}

// Process window inactive.
static int process_inactive(flat_state_t* pState)
{
	return 0;
}

// Process input message.
static int process_input(flat_state_t* pState, const void* pMsg)
{
	Assert(pState != NULL);
	Assert(pMsg != NULL);

	IInputService::input_t* pInput = (IInputService::input_t*)pMsg;

	switch (pInput->nType)
	{
	case IInputService::TYPE_MOUSE_MOVE:
		return process_mouse_move(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_MOUSE_WHEEL:
		return process_mouse_wheel(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->dValue, pInput->nFlags);
	case IInputService::TYPE_MOUSE_LEAVE:
		return process_mouse_leave(pState);
	case IInputService::TYPE_LEFT_DOWN:
		return process_left_down(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_LEFT_UP:
		return process_left_up(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_LEFT_DUAL:
		return process_left_dual(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_RIGHT_DOWN:
		return process_right_down(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_RIGHT_UP:
		return process_right_up(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_RIGHT_DUAL:
		return process_right_dual(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_MIDDLE_DOWN:
		return process_middle_down(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_MIDDLE_UP:
		return process_middle_up(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_MIDDLE_DUAL:
		return process_middle_dual(pState, (int)pInput->dPosX,
			(int)pInput->dPosY, pInput->nFlags);
	case IInputService::TYPE_KEY_DOWN:
		return process_key_down(pState, pInput->nChar, pInput->nFlags);
	case IInputService::TYPE_KEY_UP:
		return process_key_up(pState, pInput->nChar, pInput->nFlags);
	case IInputService::TYPE_CHAR:
		return process_char(pState, pInput->nChar, pInput->nFlags);
	case IInputService::TYPE_ACTIVE:
		return process_active(pState);
	case IInputService::TYPE_INACTIVE:
		return process_inactive(pState);
	default:
		break;
	}

	return 0;
}

// Process default state.
int flat_state_default(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg)
{
	Assert(pState != NULL);

	if (event == FLAT_EVENT_TIMER)
	{
		float frame_time = *(const float*)pMsg;

		pState->fTimeCount += frame_time;

		if (pState->fTimeCount > 0.5F)
		{
			FlatWidget* pWidget = flat_get_widget(pState->CaptureID);

			if (pWidget)
			{
				flat_display_hint(pState, pWidget, pState->nMouseX, 
					pState->nMouseY);
			}

			pState->fTimeCount = 0.0F;
		}

		return 1;
	}

	if (event == FLAT_EVENT_INPUT)
	{
		process_input(pState, pMsg);
		return 1;
	}

	if (event == FLAT_EVENT_ENTER)
	{
		FlatWidget* pClicked = flat_get_widget(pState->ClickedID);

		if (pClicked)
		{
			// Reset clicked widget.
			pClicked->OnClickReset();
			pState->ClickedID = uniqid_t();
		}
		
		pState->fTimeCount = 0.0F;
		return 1;
	}
	
	if (event == FLAT_EVENT_LEAVE)
	{
		return 1;
	}

	return 0;
}
