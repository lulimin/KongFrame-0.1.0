// flat_parts.h
// Created by lulimin on 2020/9/16.

#ifndef __FLAT_PARTS_H
#define __FLAT_PARTS_H

#include "../inc/common.h"

class FlatWidget;
struct flat_parts_t;

// Create part container.
flat_parts_t* flat_parts_create(FlatWidget* pParent);
// Delete part container.
void flat_parts_delete(flat_parts_t* pParts);

// Get part container size.
size_t flat_parts_get_size(flat_parts_t* pParts);
// Get part container data.
FlatWidget** flat_parts_get_data(flat_parts_t* pParts);
// Add child to container.
bool flat_parts_add(flat_parts_t* pParts, FlatWidget* pWidget);
// Remove child from container.
bool flat_parts_remove(flat_parts_t* pParts, FlatWidget* pWidget);
// Erase all parts.
void flat_parts_erase_all(flat_parts_t* pParts);

#endif // __FLAT_PARTS_H
