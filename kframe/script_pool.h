// script_pool.h
// Created by lulimin on 2019/2/18.

#ifndef __SCRIPT_POOL_H
#define __SCRIPT_POOL_H

#include "../inc/common.h"
#include "../inc/uniqid.h"

// Script coroutine management.

class IArgList;
struct scp_routine_t;
struct scp_pool_t;

// Create script coroutine pool.
scp_pool_t* scp_pool_create();
// Delete script coroutine pool.
void scp_pool_delete(scp_pool_t* pScpPool);

// Add coroutine.
bool scp_pool_add(scp_pool_t* pScpPool, scp_routine_t* pRoutine);
// Find coroutine.
scp_routine_t* scp_pool_find(scp_pool_t* pScpPool, const char* script,
	const char* func, const uniqid_t& id);
// Kill coroutine.
bool scp_pool_kill(scp_pool_t* pScpPool, const char* script, const char* func,
	const uniqid_t& id);
// Check coroutine time out or invalidated.
void scp_pool_check(scp_pool_t* pScpPool, double elapse);
// Send signal to coroutine who wait specified signal.
size_t scp_pool_send_signal(scp_pool_t* pScpPool, const uniqid_t& id,
	const char* signal, const IArgList* args);
// Dump all coroutines.
bool scp_pool_dump(scp_pool_t* pScpPool, const char* name);

#endif // __SCRIPT_POOL_H
