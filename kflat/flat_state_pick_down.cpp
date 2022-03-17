// flat_state_pick_down.cpp
// Created by lulimin on 2021/3/1.

#include "flat_state.h"
#include "flat_ui.h"
#include "flat_widget.h"
#include "../kgraphic/i_input_service.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_types.h"

// Change capture widget.
bool flat_change_capture(flat_state_t* pState, FlatWidget* pNewCapture);

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

	if ((flags & IInputService::FLAG_LEFT) == 0)
	{
		flat_state_switch(pState, FLAT_STATE_DEFAULT);
		return 1;
	}

	FlatWidget* pTopDialog = pFlatUI->GetTopDialog();

	if (NULL == pTopDialog)
	{
		return 0;
	}

	FlatWidget* pWidget = pTopDialog->GetRegionOf(x, y);

	flat_change_capture(pState, pWidget);

	if (pWidget)
	{
		pWidget->OnMouseMove(x, y, flags);
	}

	FlatWidget* pClicked = flat_get_widget(pState->ClickedID);

	if (pClicked)
	{
		pClicked->OnLeftDrag(x, y, pState->nClickX, pState->nClickY, flags);
	}

	return 1;
}

static int process_mouse_wheel(flat_state_t* pState, int x, int y,
	double delta, unsigned int flags)
{
	return 0;
}

static int process_mouse_leave(flat_state_t* pState)
{
	Assert(pState != NULL);

	FlatWidget* pClicked = flat_get_widget(pState->ClickedID);

	if (pClicked)
	{
		// Reset clicked widget.
		pClicked->OnClickReset();
		pState->ClickedID = uniqid_t();
	}

	flat_state_switch(pState, FLAT_STATE_DEFAULT);
	return 0;
}

static int process_left_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
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

	return 0;
}

static int process_left_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
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

	FlatWidget* pClicked = flat_get_widget(pState->ClickedID);

	if (pClicked)
	{
		pClicked->OnLeftDragUp(x, y, pState->nClickX, pState->nClickY, flags);
	}

	FlatWidget* pWidget = pTopDialog->GetRegionOf(x, y);

	if ((pWidget != NULL) && (pWidget == pClicked))
	{
		pWidget->OnLeftUp(x, y, flags);
	}

	pState->ClickedID = uniqid_t();
	flat_state_switch(pState, FLAT_STATE_DEFAULT);
	return 0;
}

static int process_left_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_right_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_right_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_right_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_middle_down(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_middle_up(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_middle_dual(flat_state_t* pState, int x, int y,
	unsigned int flags)
{
	return 0;
}

static int process_key_down(flat_state_t* pState, unsigned int key,
	unsigned int flags)
{
	return 0;
}

static int process_key_up(flat_state_t* pState, unsigned int key,
	unsigned int flags)
{
	return 0;
}

static int process_char(flat_state_t* pState, unsigned int ch,
	unsigned int flags)
{
	return 0;
}

static int process_active(flat_state_t* pState)
{
	return 0;
}

static int process_inactive(flat_state_t* pState)
{
	return 0;
}

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

// Process design mode pick-down state.
int flat_state_pick_down(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg)
{
	Assert(pState != NULL);

	if (event == FLAT_EVENT_TIMER)
	{
		return 1;
	}

	if (event == FLAT_EVENT_INPUT)
	{
		process_input(pState, pMsg);
		return 1;
	}

	if (event == FLAT_EVENT_ENTER)
	{
		return 1;
	}

	if (event == FLAT_EVENT_LEAVE)
	{
		return 1;
	}

	return 0;
}
