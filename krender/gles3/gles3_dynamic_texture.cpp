// gles3_dynamic_texture.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_dynamic_texture.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DynamicTexture

GLES3DynamicTexture* GLES3DynamicTexture::CreateInstance(RenderService* pRS)
{
	GLES3DynamicTexture* p = (GLES3DynamicTexture*)K_ALLOC(
		sizeof(GLES3DynamicTexture));

	new (p) GLES3DynamicTexture(pRS);
	return p;
}

void GLES3DynamicTexture::DeleteInstance(GLES3DynamicTexture* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DynamicTexture::GLES3DynamicTexture(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nLevels = 0;
	m_nTexture = 0;
	m_nInternalFormat = 0;
	m_nTextureFormat = 0;
	m_nTextureType = 0;
}

GLES3DynamicTexture::~GLES3DynamicTexture()
{
	this->DeleteResource();
}

void GLES3DynamicTexture::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DynamicTexture::CreateResource(size_t index, int width, int height,
	int format, int levels)
{
	Assert(width > 0);
	Assert(height > 0);
	
	m_nIndex = index;
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nLevels = levels;

	int internal_format = GL_RGB8;
	int texture_format = GL_RGB;
	int texture_type = GL_UNSIGNED_BYTE;

	if (format == RHI_FORMAT_R8G8B8)
	{
		internal_format = GL_RGB8;
		texture_format = GL_RGB;
		texture_type = GL_UNSIGNED_BYTE;
	}
	else if (format == RHI_FORMAT_R8G8B8A8)
	{
		internal_format = GL_RGBA8;
		texture_format = GL_RGBA;
		texture_type = GL_UNSIGNED_BYTE;
	}
	else if (format == RHI_FORMAT_A8)
	{
		internal_format = GL_ALPHA;
		texture_format = GL_ALPHA;
		texture_type = GL_UNSIGNED_BYTE;
	}

	glGenTextures(1, &m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0,
		texture_format, texture_type, NULL);
	//glTexStorage2D(GL_TEXTURE_2D, levels, internal_format, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_nInternalFormat = internal_format;
	m_nTextureFormat = texture_format;
	m_nTextureType = texture_type;
	return true;
}

bool GLES3DynamicTexture::DeleteResource()
{
	if (m_nTexture != 0)
	{
		glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}

	return true;
}
