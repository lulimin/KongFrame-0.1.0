// memory_list.h
// Created by lulimin on 2020/2/18.

#ifndef __MEMORY_LIST_H
#define __MEMORY_LIST_H

#include "../inc/common.h"

// Initialize memory list.
void memory_list_init();
// Shutdown memory list.
void memory_list_shut();
// Allocate memory in debug mode.
void* memory_list_debug_alloc(size_t size, const char* file, int line);
// Allocate memory.
void* memory_list_alloc(size_t size);
// Reallocate memory.
void* memory_list_realloc(void* ptr, size_t new_size);
// Free memory.
void memory_list_free(void* ptr);
// Dump memory informations.
bool memory_list_dump(const char* file_name);

#endif // __MEMORY_LIST_H
