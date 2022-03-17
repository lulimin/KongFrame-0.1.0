// gles3_depth_texture.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_DEPTH_TEXTURE_H
#define __GLES3_DEPTH_TEXTURE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3DepthTexture

class RenderService;

class GLES3DepthTexture : public RHIDepthTexture
{
public:
	// Create new instance.
	static GLES3DepthTexture* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3DepthTexture* pInstance);

public:
	GLES3DepthTexture(RenderService* pRS);
	virtual ~GLES3DepthTexture();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, int width, int height, int format);
	// Delete resource data.
	bool DeleteResource();

	// Get buffer identity.
	GLuint GetTexture() const { return m_nTexture; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	GLuint m_nTexture;
};

#endif // __GLES3_DEPTH_TEXTURE_H
