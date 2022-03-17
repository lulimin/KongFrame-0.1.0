// gles3_color_texture.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_color_texture.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3ColorTexture

GLES3ColorTexture* GLES3ColorTexture::CreateInstance(RenderService* pRS)
{
	GLES3ColorTexture* p = (GLES3ColorTexture*)K_ALLOC(sizeof(GLES3ColorTexture));

	new (p) GLES3ColorTexture(pRS);
	return p;
}

void GLES3ColorTexture::DeleteInstance(GLES3ColorTexture* pInstance)
{
	K_DELETE(pInstance);
}

GLES3ColorTexture::GLES3ColorTexture(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nTexture = 0;
}

GLES3ColorTexture::~GLES3ColorTexture()
{
	this->DeleteResource();
}

void GLES3ColorTexture::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3ColorTexture::CreateResource(size_t index, int width, int height,
	int format)
{
	Assert(width > 0);
	Assert(height > 0);
	
	m_nIndex = index;
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	
	int internalformat = GL_RGB8;

	glGenTextures(1, &m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, internalformat, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLES3ColorTexture::DeleteResource()
{
	if (m_nTexture != 0)
	{
		glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}

	return true;
}
