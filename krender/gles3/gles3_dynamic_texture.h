// gles3_dynamic_texture.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_DYNAMIC_TEXTURE_H
#define __GLES3_DYNAMIC_TEXTURE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3DynamicTexture

class RenderService;

class GLES3DynamicTexture : public RHIDynamicTexture
{
public:
	// Create new instance.
	static GLES3DynamicTexture* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3DynamicTexture* pInstance);

public:
	GLES3DynamicTexture(RenderService* pRS);
	virtual ~GLES3DynamicTexture();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, int width, int height, int format,
		int levels);
	// Delete resource data.
	bool DeleteResource();

	// Get texture size.
	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }

	// Get texture identity.
	GLuint GetTexture() const { return m_nTexture; }
	// Get texture format.
	GLint GetInternalFormat() const { return m_nInternalFormat; }
	GLenum GetTextureFormat() const { return m_nTextureFormat; }
	GLenum GetTextureType() const { return m_nTextureType; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nLevels;
	GLuint m_nTexture;
	GLint m_nInternalFormat;
	GLenum m_nTextureFormat;
	GLenum m_nTextureType;
};

#endif // __GLES3_DYNAMIC_TEXTURE_H
