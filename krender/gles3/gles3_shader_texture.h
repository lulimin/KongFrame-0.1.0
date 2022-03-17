// gles3_shader_texture.h
// Created by lulimin on 2020/11/27.

#ifndef __GLES3_SHADER_TEXTURE_H
#define __GLES3_SHADER_TEXTURE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_shader_texture.h"
#include "gles3_header.h"

// GLES3ShaderTexture

class GLES3ShaderTexture : public RHIShaderTexture
{
public:
	// Create new instance.
	static GLES3ShaderTexture* CreateInstance();

public:
	GLES3ShaderTexture();
	virtual ~GLES3ShaderTexture();

	virtual void Release();

	// Create resource data.
	bool CreateResource(int width, int height, int format, int levels,
		const void* data, size_t size);
	// Delete resource data.
	bool DeleteResource();

	// Get texture identity.
	GLuint GetTexture() const { return m_nTexture; }

private:
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nLevels;
	GLuint m_nTexture;
};

#endif // __GLES3_SHADER_TEXTURE_H
