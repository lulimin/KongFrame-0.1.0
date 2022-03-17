// memory_link.h
// Created by lulimin on 2020/2/18.

#ifndef __MEMORY_LINK_H
#define __MEMORY_LINK_H

#include "../inc/common.h"

// Initialize memory link.
void memory_link_init();
// Shutdown memory link.
void memory_link_shut();
// Allocate memory in debug mode.
void* memory_link_debug_alloc(size_t size, const char* file, int line);
// Allocate memory.
void* memory_link_alloc(size_t size);
// Reallocate memory
void* memory_link_realloc(void* ptr, size_t new_size);
// Free memory.
void memory_link_free(void* ptr);
// Dump memory informations.
bool memory_link_dump(const char* file_name);

#endif // __MEMORY_LINK_H
