// script_extend.h
// Created by lulimin on 2019/2/18.

#ifndef __SCRIPT_EXTEND_H
#define __SCRIPT_EXTEND_H

#include "script_utility.h"

// Register lua script extension functions.
void register_extend_functions(lua_State* state);

#endif // __SCRIPT_EXTEND_H
