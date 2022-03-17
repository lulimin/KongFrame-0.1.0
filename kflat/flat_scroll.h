// flat_scroll.h
// Created by lulimin on 2020/9/17.

#ifndef __FLAT_SCROLL_H
#define __FLAT_SCROLL_H

#include "../inc/common.h"
#include "../inc/uniqid.h"

struct flat_scroll_t;
struct flat_canvas_t;
class FlatUI;
class FlatWidget;

// Create scrollable.
flat_scroll_t* flat_scroll_create(FlatWidget* pParent);
// Delete scrollable.
void flat_scroll_delete(flat_scroll_t* pScroll);

// Initialize scrollable.
bool flat_scroll_initialize(flat_scroll_t* pScroll, FlatUI* pFlatUI,
	bool vertical, bool horizontal);
// Set vertical scroll bar exists.
bool flat_scroll_set_vscroll_exists(flat_scroll_t* pScroll, FlatUI* pFlatUI, 
	bool exists);
// Set horizontal scroll bar exists.
bool flat_scroll_set_hscroll_exists(flat_scroll_t* pScroll, FlatUI* pFlatUI, 
	bool exists);
// Get vertical scroll bar.
bool flat_scroll_get_vscroll_exists(flat_scroll_t* pScroll);
// Get horizontal scroll bar.
bool flat_scroll_get_hscroll_exists(flat_scroll_t* pScroll);

// Get vertical scroll bar object id.
uniqid_t flat_scroll_vscroll_id(flat_scroll_t* pScroll);
// Get horizontal scroll bar object id.
uniqid_t flat_scroll_hscroll_id(flat_scroll_t* pScroll);
// Get cross label object id.
uniqid_t flat_scroll_cross_label_id(flat_scroll_t* pScroll);

// Set scroll bar size.
void flat_scroll_set_bar_size(flat_scroll_t* pScroll, int value);
// Get scroll bar size.
int flat_scroll_get_bar_size(flat_scroll_t* pScroll);
// Set always show vertical scroll bar.
void flat_scroll_set_vscroll_always(flat_scroll_t* pScroll, bool value);
// Get always show vertical scroll bar.
bool flat_scroll_get_vscroll_always(flat_scroll_t* pScroll);
// Set always show horizontal scroll bar.
void flat_scroll_set_hscroll_always(flat_scroll_t* pScroll, bool value);
// Get always show horizontal scroll bar.
bool flat_scroll_get_hscroll_always(flat_scroll_t* pScroll);
// Set dynamic size of slide bar.
void flat_scroll_set_dynamic_slide(flat_scroll_t* pScroll, bool value);
// Get dynamic size of slide bar.
bool flat_scroll_get_dynamic_slide(flat_scroll_t* pScroll);

// Get reveal width.
int flat_scroll_reveal_width(flat_scroll_t* pScroll);
// Get reveal height.
int flat_scroll_reveal_height(flat_scroll_t* pScroll);

// Perform layout.
void flat_scroll_perform_layout(flat_scroll_t* pScroll);

// Update scroll bar.
bool flat_scroll_update(flat_scroll_t* pScroll);

// Draw scroll.
void flat_scroll_draw(flat_scroll_t* pScroll, flat_canvas_t* pCanvas);

// Scroll value changed.
void flat_scroll_changed(flat_scroll_t* pScroll, FlatWidget* pPartWidget,
	int new_value, int old_value);

// Adjust vertical scroll position.
bool flat_scroll_adjust_vscroll(flat_scroll_t* pScroll);
// Adjust horizontal scroll position.
bool flat_scroll_adjust_hscroll(flat_scroll_t* pScroll);

// Clone scroll style.
bool flat_scroll_clone_style(flat_scroll_t* pScroll, flat_scroll_t* pSrc);

#endif // __FLAT_SCROLL_H
