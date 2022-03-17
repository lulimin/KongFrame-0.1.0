// flat_canvas.h
// Created by lulimin on 2020/9/17.

#ifndef __FLAT_CANVAS_H
#define __FLAT_CANVAS_H

#include "../inc/common.h"

// User interface canvas.

// Image layout.
#define FLAT_LAYOUT_FIT 0
#define FLAT_LAYOUT_PATCH 1
#define FLAT_LAYOUT_HPATCH 2
#define FLAT_LAYOUT_VPATCH 3

// Font description.
struct flat_font_desc_t
{
	const char* pName;
	const char* pResource;
	int nImageSize;
	int nImageGroup;
	int nCharHeight;
	int nInterval;
	bool bFixedWidth;
	bool bSmooth;
	bool bItalic;
	bool bBold;
};

class IRenderService;
struct flat_image_t;
struct flat_canvas_t;
struct flat_font_type_t;

// Create user interface canvas render.
flat_canvas_t* canvas_create(IRenderService* pRenderService);
// Delete user interface canvas render.
void canvas_delete(flat_canvas_t* pCanvas);

// Load resource.
bool canvas_load(flat_canvas_t* pCanvas);
// Unload resource.
bool canvas_unload(flat_canvas_t* pCanvas);

// Render begin process.
bool canvas_render_begin(flat_canvas_t* pCanvas);
// Render end process.
bool canvas_render_end(flat_canvas_t* pCanvas);

// Add font.
flat_font_type_t* canvas_add_font(flat_canvas_t* pCanvas,
	const flat_font_desc_t& desc);
// Remove font.
bool canvas_remove_font(flat_canvas_t* pCanvas, const char* font_name);
// Set current font.
bool canvas_set_font(flat_canvas_t* pCanvas, flat_font_type_t* pFontType);

// Set view range.
bool canvas_set_range(flat_canvas_t* pCanvas, int left, int top, int width,
	int height);
// Reset view range.
bool canvas_reset_range(flat_canvas_t* pCanvas);
// Draw lines.
bool canvas_draw_lines(flat_canvas_t* pCanvas, int* coords, int line_num,
	unsigned int color);
// Draw line frame.
bool canvas_draw_frame(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color);
// Draw solid box.
bool canvas_draw_block(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color);
// Draw image.
bool canvas_draw_image(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	flat_image_t* pImage, int layout);
// Draw text.
int canvas_draw_text(flat_canvas_t* pCanvas, int x, int y, unsigned int color,
	const char* text, int start, int char_num);
int canvas_draw_text_left(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num);
int canvas_draw_text_center(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num);
int canvas_draw_text_right(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num);
// Get width for draw text.
int canvas_get_text_width(flat_canvas_t* pCanvas, const char* text, int start,
	int char_num);
// Get height for draw text.
int canvas_get_text_height(flat_canvas_t* pCanvas, const char* text);

// Draw image use float coordinate.
bool canvas_render_image(flat_canvas_t* pCanvas, float x1, float y1, float x2,
	float y2, float x3, float y3, float x4, float y4, flat_image_t* pImage);

#endif // __FLAT_CANVAS_H
