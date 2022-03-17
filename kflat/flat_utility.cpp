// flat_utility.cpp
// Created by lulimin on 2020/9/11.

#include "flat_utility.h"
#include "flat_image.h"
#include "flat_ui.h"
#include "flat_widget.h"
#include "../inc/utility.h"

// Get widget object.
FlatWidget* flat_get_widget(const uniqid_t& id)
{
	FlatWidget* pWidget = (FlatWidget*)frame_get_object(id);

	if (NULL == pWidget)
	{
		return NULL;
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatWidget"))
	{
		return NULL;
	}

	return pWidget;
}

// Load image.
flat_image_t* flat_load_image(FlatUI* pFlatUI, flat_image_t* pImage,
	const char* name, bool async)
{
	Assert(pFlatUI != NULL);
	Assert(name != NULL);
	
	if (pImage)
	{
		flat_image_delete(pImage);
	}

	if (util_string_empty(name))
	{
		return NULL;
	}

	flat_image_t* pImg = flat_image_create(name);

	if (!flat_image_load(pImg, pFlatUI->GetRenderService(), async))
	{
		flat_image_delete(pImg);
		return NULL;
	}

	return pImg;
}

// Convert color string to integer.
unsigned int flat_string_to_rgba(const char* color)
{
	Assert(color != NULL);

	const char* s1 = strchr(color, ',');

	if (NULL == s1)
	{
		return 0;
	}

	const char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return 0;
	}

	const char* s3 = strchr(s2 + 1, ',');

	if (NULL == s3)
	{
		return 0;
	}

	unsigned int red = atoi(color);
	unsigned int green = atoi(s1 + 1);
	unsigned int blue = atoi(s2 + 1);
	unsigned int alpha = atoi(s3 + 1);

	return (red & 0xFF) + ((green & 0xFF) << 8) + ((blue & 0xFF) << 16) +
		((alpha & 0xFF) << 24);
}

// Convert color integer to string.
const char* flat_rgba_to_string(unsigned int rgba, char* buffer, size_t size)
{
	Assert(buffer != NULL);
	Assert(size >= 16);

	unsigned int red = rgba & 0xFF;
	unsigned int green = (rgba >> 8) & 0xFF;
	unsigned int blue = (rgba >> 16) & 0xFF;
	unsigned int alpha = (rgba >> 24) & 0xFF;

	plat_safe_sprintf(buffer, size, "%d,%d,%d,%d", red, green, blue, alpha);
	return buffer;
}

// Fit widget rectangle to safe area.
bool flat_fit_safe_area(FlatUI* pFlatUI, FlatWidget* pWidget)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);

	int safe_left = pFlatUI->GetSafeAreaLeft();
	int safe_top = pFlatUI->GetSafeAreaTop();
	int safe_right = pFlatUI->GetSafeAreaRight();
	int safe_bottom = pFlatUI->GetSafeAreaBottom();
	int global_left = pWidget->GetGlobalLeft();
	int global_top = pWidget->GetGlobalTop();

	if (global_left < safe_left)
	{
		pWidget->SetGlobalLeft(safe_left);
		global_left = safe_left;
	}

	if (global_top < safe_top)
	{
		pWidget->SetGlobalTop(safe_top);
		global_top = safe_top;
	}

	int old_width = pWidget->GetWidth();
	int old_height = pWidget->GetHeight();
	int global_right = global_left + old_width;
	int global_bottom = global_top + old_height;
	int ui_width = pFlatUI->GetWidth();
	int ui_height = pFlatUI->GetHeight();

	if (global_right > (ui_width - safe_right))
	{
		pWidget->SetWidth(ui_width - safe_right - global_left);
	}

	if (global_bottom > (ui_height - safe_bottom))
	{
		pWidget->SetHeight(ui_height - safe_bottom - global_top);
	}
	
	return true;
}
