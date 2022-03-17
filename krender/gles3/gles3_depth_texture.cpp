// gles3_depth_texture.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_depth_texture.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DepthTexture

GLES3DepthTexture* GLES3DepthTexture::CreateInstance(RenderService* pRS)
{
	GLES3DepthTexture* p = (GLES3DepthTexture*)K_ALLOC(sizeof(GLES3DepthTexture));

	new (p) GLES3DepthTexture(pRS);
	return p;
}

void GLES3DepthTexture::DeleteInstance(GLES3DepthTexture* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DepthTexture::GLES3DepthTexture(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nTexture = 0;
}

GLES3DepthTexture::~GLES3DepthTexture()
{
	this->DeleteResource();
}

void GLES3DepthTexture::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DepthTexture::CreateResource(size_t index, int width, int height,
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

bool GLES3DepthTexture::DeleteResource()
{
	if (m_nTexture != 0)
	{
		glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}

	return true;
}
