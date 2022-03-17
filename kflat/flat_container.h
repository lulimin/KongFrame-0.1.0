// flat_container.h
// Created by lulimin on 2020/9/16.

#ifndef __FLAT_CONTAINER_H
#define __FLAT_CONTAINER_H

#include "../inc/common.h"

class FlatWidget;
struct flat_container_t;

// Create children container.
flat_container_t* flat_container_create(FlatWidget* pParent);
// Delete children container.
void flat_container_delete(flat_container_t* pContainer);

// Get children container size.
size_t flat_container_get_size(flat_container_t* pContainer);
// Get children container data.
FlatWidget** flat_container_get_data(flat_container_t* pContainer);
// Add child to container.
bool flat_container_add(flat_container_t* pContainer, FlatWidget* pWidget);
// Remove child from container.
bool flat_container_remove(flat_container_t* pContainer, FlatWidget* pWidget);
// Erase all children.
void flat_container_erase_all(flat_container_t* pContainer);
// Test container empty.
bool flat_container_empty(flat_container_t* pContainer);
// Find child in container.
bool flat_container_find(flat_container_t* pContainer, const char* name);
// Bring child to front.
bool flat_container_to_front(flat_container_t* pContainer, FlatWidget* pWidget);
// Send child to back.
bool flat_container_to_back(flat_container_t* pContainer, FlatWidget* pWidget);

#endif // __FLAT_CONTAINER_H
