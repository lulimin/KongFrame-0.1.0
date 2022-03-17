// i_input_service.h
// Created by lulimin on 2020/9/23.

#ifndef __I_INPUT_SERVICE_H
#define __I_INPUT_SERVICE_H

#include "../inc/common.h"
#include "../inc/frame_def.h"

// IInputService

class IInputService : public IService
{
public:
	// Types of user input.
	enum INPUT_TYPE_ENUM
	{
		TYPE_UNKNOWN,
		TYPE_MOUSE_MOVE,
		TYPE_MOUSE_WHEEL,
		TYPE_MOUSE_LEAVE,
		TYPE_LEFT_DOWN,
		TYPE_LEFT_UP,
		TYPE_LEFT_DUAL,
		TYPE_RIGHT_DOWN,
		TYPE_RIGHT_UP,
		TYPE_RIGHT_DUAL,
		TYPE_MIDDLE_DOWN,
		TYPE_MIDDLE_UP,
		TYPE_MIDDLE_DUAL,
		TYPE_KEY_DOWN,
		TYPE_KEY_UP,
		TYPE_CHAR,
		TYPE_ACTIVE,
		TYPE_INACTIVE,
		TYPE_CURSOR,
		TYPE_CLOSE,
		TYPE_TOUCH,
		TYPE_SIZED,
	};

	// Flags of user input.
	enum INPUT_FLAG_ENUM
	{
		FLAG_SHIFT = 0x1,
		FLAG_ALTER = 0x2,
		FLAG_CONTROL = 0x4,
		FLAG_LEFT = 0x10,
		FLAG_RIGHT = 0x20,
		FLAG_MIDDLE = 0x40,
	};

	// Input key values.
	enum INPUT_KEY_ENUM
	{
		KEY_UP = 0x10000,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_PRIOR,
		KEY_NEXT,
		KEY_HOME,
		KEY_END,
		KEY_RETURN,
		KEY_INSERT,
		KEY_DELETE,
		KEY_BACK,
		KEY_TAB,
		KEY_ESCAPE,
		KEY_CAPITAL,
		KEY_SHIFT,
		KEY_CONTROL,
		KEY_MENU,
		KEY_ADD,
		KEY_SUBTRACT,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
	};

	// Input data.
	struct input_t
	{
		int nType;
		unsigned int nChar;
		unsigned int nFlags;
		double dPosX;
		double dPosY;
		double dValue;
	};

	// User input processor pointer.
	typedef int(*processor_func_t)(void* context, const input_t& msg);

public:
	// Add user input processor.
	virtual bool AddProcessor(processor_func_t func, void* context,
		const uniqid_t& id) = 0;
	// Remove user input processor.
	virtual bool RemoveProcessor(processor_func_t func, void* context,
		const uniqid_t& id) = 0;
	// Process user input.
	virtual bool ProcessInput(const input_t& msg) = 0;
};

#endif // __I_INPUT_SERVICE_H
