// gles3_color_texture.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_COLOR_TEXTURE_H
#define __GLES3_COLOR_TEXTURE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3ColorTexture

class RenderService;

class GLES3ColorTexture : public RHIColorTexture
{
public:
	// Create new instance.
	static GLES3ColorTexture* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3ColorTexture* pInstance);

public:
	GLES3ColorTexture(RenderService* pRS);
	virtual ~GLES3ColorTexture();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, int width, int height, int format);
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
	GLuint m_nTexture;
};

#endif // __GLES3_COLOR_TEXTURE_H
