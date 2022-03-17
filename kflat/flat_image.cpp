// flat_image.cpp
// Created by lulimin on 2020/12/4.

#include "flat_image.h"
#include "../kgraphic/rhi_resource.h"
#include "../kgraphic/i_texture.h"
#include "../kgraphic/i_render_service.h"
#include "../inc/frame_mem.h"

// User interface image.
struct flat_image_t
{
	char* pName;
	int nRectLeft;
	int nRectTop;
	int nRectRight;
	int nRectBottom;
	ITexture* pTexture;
};

static bool parse_rectangle(const char* s, int* left, int* top, int* right,
	int* bottom)
{
	const char* s1 = strchr(s, ',');

	if (NULL == s1)
	{
		return false;
	}

	const char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return false;
	}

	const char* s3 = strchr(s2 + 1, ',');

	if (NULL == s3)
	{
		return false;
	}

	*left = atoi(s);
	*top = atoi(s1 + 1);
	*right = atoi(s2 + 1);
	*bottom = atoi(s3 + 1);
	return true;
}

// Create image.
flat_image_t* flat_image_create(const char* resource)
{
	Assert(resource != NULL);

	flat_image_t* pImage = K_NEW(flat_image_t);

	const char* comma = strchr(resource, ',');

	if (comma)
	{
		size_t name_len = comma - resource;

		pImage->pName = (char*)K_ALLOC(name_len + 1);
		memcpy(pImage->pName, resource, name_len);
		pImage->pName[name_len] = 0;

		int left;
		int top;
		int right;
		int bottom;

		if (parse_rectangle(comma + 1, &left, &top, &right, &bottom))
		{
			pImage->nRectLeft = left;
			pImage->nRectTop = top;
			pImage->nRectRight = right;
			pImage->nRectBottom = bottom;
		}
		else
		{
			pImage->nRectLeft = 0;
			pImage->nRectTop = 0;
			pImage->nRectRight = 0;
			pImage->nRectBottom = 0;
		}
	}
	else
	{ 
		size_t name_size = strlen(resource) + 1;
		
		pImage->pName = (char*)K_ALLOC(name_size);
		memcpy(pImage->pName, resource, name_size);
		pImage->nRectLeft = 0;
		pImage->nRectTop = 0;
		pImage->nRectRight = 0;
		pImage->nRectBottom = 0;
	}

	pImage->pTexture = NULL;
	return pImage;
}

// Delete image.
void flat_image_delete(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	K_FREE(pImage->pName);
	K_DELETE(pImage);
}

// Load resource.
bool flat_image_load(flat_image_t* pImage, IRenderService* pRS, bool async)
{
	Assert(pImage != NULL);
	Assert(pRS != NULL);

	pImage->pTexture = pRS->LoadTexture(pImage->pName, async, 
		IRenderService::ASSET_INDICATOR_UI_IMAGE);

	if (NULL == pImage->pTexture)
	{
		return false;
	}

	if (pImage->nRectLeft == pImage->nRectRight)
	{
		pImage->nRectRight = pImage->pTexture->GetWidth();
		pImage->nRectBottom = pImage->pTexture->GetHeight();
	}

	return true;
}

// Unload resource.
bool flat_image_unload(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	SAFE_RELEASE(pImage->pTexture);
	return true;
}

// Whether image load succeed.
bool flat_image_load_succeed(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	if (NULL == pImage->pTexture)
	{
		return true;
	}

	return pImage->pTexture->GetLoadSucceed();
}

// Whether image load ended.
bool flat_image_load_ended(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	if (NULL == pImage->pTexture)
	{
		return true;
	}

	return pImage->pTexture->GetLoadEnded();
}

// Get image name.
const char* flat_image_get_name(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	return pImage->pName;
}

// Get image width.
int flat_image_get_width(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	if (NULL == pImage->pTexture)
	{
		return 0;
	}

	return pImage->pTexture->GetWidth();
}

// Get image height.
int flat_image_get_height(flat_image_t* pImage)
{
	Assert(pImage != NULL);

	if (NULL == pImage->pTexture)
	{
		return 0;
	}

	return pImage->pTexture->GetHeight();
}

// Get texture.
ITexture* flat_image_get_texture(flat_image_t* pImage)
{
	Assert(pImage != NULL);
	
	return pImage->pTexture;
}

// Set rectangle.
void flat_image_set_rectangle(flat_image_t* pImage, int rect_left,
	int rect_top, int rect_right, int rect_bottom)
{
	Assert(pImage != NULL);
	
	pImage->nRectLeft = rect_left;
	pImage->nRectTop = rect_top;
	pImage->nRectRight = rect_right;
	pImage->nRectBottom = rect_bottom;
}

// Get rectangle.
void flat_image_get_rectangle(flat_image_t* pImage, int* pRectLeft,
	int* pRectTop, int* pRectRight, int* pRectBottom)
{
	Assert(pImage != NULL);

	*pRectLeft = pImage->nRectLeft;
	*pRectTop = pImage->nRectTop;
	*pRectRight = pImage->nRectRight;
	*pRectBottom = pImage->nRectBottom;
}
