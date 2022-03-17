// flat_state.h
// Created by lulimin on 2020/12/21.

#ifndef __FLAT_STATE_H
#define __FLAT_STATE_H

#include "../inc/common.h"
#include "../inc/i_arg_list.h"

// User interface states.
#define FLAT_STATE_DEFAULT 0
#define FLAT_STATE_LEFT_DOWN 1
#define FLAT_STATE_LEFT_HOVER 2
#define FLAT_STATE_PICK_DOWN 3

// User interface events.
#define FLAT_EVENT_ENTER 1
#define FLAT_EVENT_LEAVE 2
#define FLAT_EVENT_INPUT 3
#define FLAT_EVENT_TIMER 4

class FlatUI;

// User interface state.
struct flat_state_t
{
	FlatUI* pFlatUI;
	int nState;
	int nMouseX;
	int nMouseY;
	int nClickX;
	int nClickY;
	float fTimeCount;
	uniqid_t ClickedID;
	uniqid_t CaptureID;
	uniqid_t FocusedID;
	uniqid_t HintLabelID;
	uniqid_t HintOwnID;
	uniqid_t ContextMenuID;
	uniqid_t ContextOwnID;
};

// Create state machine.
flat_state_t* flat_state_create(FlatUI* pFlatUI);
// Delete state machine.
void flat_state_delete(flat_state_t* pState);

// Switch state.
bool flat_state_switch(flat_state_t* pState, int new_state);
// Run state machine.
int flat_state_machine(flat_state_t* pState, int event, const IArgList& args,
	const void* pMsg);

#endif // __FLAT_STATE_H
