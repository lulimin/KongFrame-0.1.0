// gles3_static_texture.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_STATIC_TEXTURE_H
#define __GLES3_STATIC_TEXTURE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3StaticTexture

class RenderService;

class GLES3StaticTexture : public RHIStaticTexture
{
public:
	// Create new instance.
	static GLES3StaticTexture* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3StaticTexture* pInstance);

public:
	GLES3StaticTexture(RenderService* pRS);
	virtual ~GLES3StaticTexture();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, int width, int height, int format,
		int levels, const void* data, size_t size);
	// Delete resource data.
	bool DeleteResource();

	// Get texture identity.
	GLuint GetTexture() const { return m_nTexture; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nLevels;
	GLuint m_nTexture;
};

#endif // __GLES3_STATIC_TEXTURE_H
