// flat_state.cpp
// Created by lulimin on 2020/12/21.

#include "flat_state.h"
#include "flat_ui.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_types.h"

// Create state machine.
flat_state_t* flat_state_create(FlatUI* pFlatUI)
{
	Assert(pFlatUI != NULL);
	
	flat_state_t* pState = K_NEW(flat_state_t);

	pState->pFlatUI = pFlatUI;
	pState->nState = 0;
	pState->nMouseX = 0;
	pState->nMouseY = 0;
	pState->nClickX = 0;
	pState->nClickY = 0;
	pState->fTimeCount = 0.0F;
	return pState;
}

// Delete state machine.
void flat_state_delete(flat_state_t* pState)
{
	Assert(pState != NULL);

	K_DELETE(pState);
}

// State function pointer.
typedef int(*state_func_t)(flat_state_t* pState, int event, 
	const IArgList& args, const void* pMsg);

// State data.
struct state_data_t
{
	int nIndex;
	state_func_t pFunc;
	const char* pName;
};

// Process default state.
int flat_state_default(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg);
// Process left-down state.
int flat_state_left_down(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg);
// Process left-hover state.
int flat_state_left_hover(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg);
// Process design mode pick-down state.
int flat_state_pick_down(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg);

// State-function table.
static state_data_t s_StateTable[] = 
{
	{ FLAT_STATE_DEFAULT, flat_state_default, "default" },
	{ FLAT_STATE_LEFT_DOWN, flat_state_left_down, "left_down" },
	{ FLAT_STATE_LEFT_HOVER, flat_state_left_hover, "left_hover" },
	{ FLAT_STATE_PICK_DOWN, flat_state_pick_down, "pick_down" },
};

// Switch state.
bool flat_state_switch(flat_state_t* pState, int new_state)
{
	Assert(pState != NULL);

	int old_state = pState->nState;

	// Leave old state.
	flat_state_machine(pState, FLAT_EVENT_LEAVE, FrameArgList() << new_state,
		NULL);
	// Set new state.
	pState->nState = new_state;
	// Enter new state.
	flat_state_machine(pState, FLAT_EVENT_ENTER, FrameArgList() << old_state,
		NULL);
	return true;
}

// Run state machine.
int flat_state_machine(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg)
{
	Assert(pState != NULL);

	int state = pState->nState;

	Assert((size_t)state < (sizeof(s_StateTable) / sizeof(state_data_t)));

	int processed = s_StateTable[state].pFunc(pState, event, args, pMsg);

	if (0 == processed)
	{
		// Not processed.
		char info[256];

		plat_safe_sprintf(info, sizeof(info), 
			"(flat_state_machine)State %d event %d not process.",
			state, event);
		K_LOG(info);
	}

	return processed;
}
