// flat_utility.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_UTILITY_H
#define __FLAT_UTILITY_H

#include "../inc/common.h"
#include "../inc/uniqid.h"

class FlatUI;
class FlatWidget;
struct flat_image_t;

// Get widget object.
FlatWidget* flat_get_widget(const uniqid_t& id);
// Load image.
flat_image_t* flat_load_image(FlatUI* pFlatUI, flat_image_t* pImage,
	const char* name, bool async);
// Convert color string to integer.
unsigned int flat_string_to_rgba(const char* color);
// Convert color integer to string.
const char* flat_rgba_to_string(unsigned int rgba, char* buffer, size_t size);
// Fit widget rectangle to safe area.
bool flat_fit_safe_area(FlatUI* pFlatUI, FlatWidget* pWidget);

// Test point in rectangle.
inline bool flat_in_rectangle(int x, int y, int x1, int y1, int x2, int y2)
{
	return (x >= x1) && (x < x2) && (y >= y1) && (y < y2);
}

#endif // __FLAT_UTILITY_H
