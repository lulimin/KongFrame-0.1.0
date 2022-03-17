// script_manager.h
// Created by lulimin on 2019/2/18.

#ifndef __SCRIPT_MANAGER_H
#define __SCRIPT_MANAGER_H

#include "../inc/common.h"

struct scp_file_t;
struct scp_manager_t;

// Create script manager.
scp_manager_t* scp_manager_create();
// Delete script manager.
void scp_manager_delete(scp_manager_t* pScpManager);

// Add script file.
scp_file_t* scp_manager_add_script(scp_manager_t* pScpManager,
	const char* path, const char* name);
// Search script file.
scp_file_t* scp_manager_find_script(scp_manager_t* pScpManager,
	const char* name);
// Reload script file.
bool scp_manager_reload_script(scp_manager_t* pScpManager, 
	scp_file_t* pScpFile);
// Reload all script file.
bool scp_manager_reload_all_script(scp_manager_t* pScpManager);
// Dump all scripts.
bool scp_manager_dump(scp_manager_t* pScpManager, const char* name);

#endif // __SCRIPT_MANAGER_H
