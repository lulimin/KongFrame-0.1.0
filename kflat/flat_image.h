// flat_image.h
// Created by lulimin on 2020/12/4.

#ifndef __FLAT_IMAGE_H
#define __FLAT_IMAGE_H

#include "../inc/common.h"

// User interface image.

class IRenderService;
class ITexture;
struct flat_image_t;

// Create user interface image.
flat_image_t* flat_image_create(const char* resource);
// Delete user interface image.
void flat_image_delete(flat_image_t* pImage);

// Load resource.
bool flat_image_load(flat_image_t* pImage, IRenderService* pRS, bool async);
// Unload resource.
bool flat_image_unload(flat_image_t* pImage);
// Whether image load succeed.
bool flat_image_load_succeed(flat_image_t* pImage);
// Whether image load ended.
bool flat_image_load_ended(flat_image_t* pImage);

// Get image name.
const char* flat_image_get_name(flat_image_t* pImage);
// Get image width.
int flat_image_get_width(flat_image_t* pImage);
// Get image height.
int flat_image_get_height(flat_image_t* pImage);
// Get texture.
ITexture* flat_image_get_texture(flat_image_t* pImage);
// Set rectangle.
void flat_image_set_rectangle(flat_image_t* pImage, int rect_left,
	int rect_top, int rect_right, int rect_bottom);
// Get rectangle.
void flat_image_get_rectangle(flat_image_t* pImage, int* pRectLeft,
	int* pRectTop, int* pRectRight, int* pRectBottom);

#endif // __FLAT_IMAGE_H
