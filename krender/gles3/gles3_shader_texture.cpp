// gles3_shader_texture.cpp
// Created by lulimin on 2020/11/27.

#include "gles3_shader_texture.h"
#include "../../kgraphic/rhi_resource.h"
#include "../../inc/frame_mem.h"

// GLES3ShaderTexture

GLES3ShaderTexture* GLES3ShaderTexture::CreateInstance()
{
	return K_NEW(GLES3ShaderTexture);
}

GLES3ShaderTexture::GLES3ShaderTexture()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nLevels = 0;
	m_nTexture = 0;
}

GLES3ShaderTexture::~GLES3ShaderTexture()
{
	this->DeleteResource();
}

void GLES3ShaderTexture::Release()
{
	K_DELETE(this);
}

bool GLES3ShaderTexture::CreateResource(int width, int height, int format,
	int levels, const void* data, size_t size)
{
	Assert(width > 0);
	Assert(height > 0);
	Assert(data != NULL);
	Assert(size > 0);

	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nLevels = levels;
	
	int internalformat = GL_RGB8;
	int tex_format = GL_RGB;
	int tex_type = 0;

	if (format == RHI_FORMAT_R8G8B8)
	{
		internalformat = GL_RGB8;
		tex_format = GL_RGB;
		tex_type = GL_UNSIGNED_BYTE;
	}
	else if (format == RHI_FORMAT_R8G8B8A8)
	{
		internalformat = GL_RGBA8;
		tex_format = GL_RGBA;
		tex_type = GL_UNSIGNED_BYTE;
	}
	else
	{
		Assert(0);
		return false;
	}

	glGenTextures(1, &m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, 
		tex_format, tex_type, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLES3ShaderTexture::DeleteResource()
{
	if (m_nTexture != 0)
	{
		glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}

	return true;
}
