// script_meta.h
// Created by lulimin on 2019/2/17.

#ifndef __SCRIPT_META_H
#define __SCRIPT_META_H

#include "../inc/common.h"

// User-data metatable.

struct lua_State;

// Get user-data metatable name.
const char* scp_get_meta_table_name();
// Add metatable to script VM.
void scp_add_meta_table(lua_State* state);

#endif // __SCRIPT_META_H
