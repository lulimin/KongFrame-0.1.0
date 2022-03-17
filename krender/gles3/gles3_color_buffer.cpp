// gles3_color_buffer.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_color_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3ColorBuffer

GLES3ColorBuffer* GLES3ColorBuffer::CreateInstance(RenderService* pRS)
{
	GLES3ColorBuffer* p = (GLES3ColorBuffer*)K_ALLOC(sizeof(GLES3ColorBuffer));

	new (p) GLES3ColorBuffer(pRS);
	return p;
}

void GLES3ColorBuffer::DeleteInstance(GLES3ColorBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3ColorBuffer::GLES3ColorBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nSamples = 0;
	m_nBuffer = 0;
}

GLES3ColorBuffer::~GLES3ColorBuffer()
{
	this->DeleteResource();
}

void GLES3ColorBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3ColorBuffer::CreateResource(size_t index, int width, int height,
	int format, int samples)
{
	Assert(width > 0);
	Assert(height > 0);

	m_nIndex = index;
	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nSamples = samples;

	glGenRenderbuffers(1, &m_nBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nBuffer);

	int internalformat = GL_RGB8;

	if (samples == 0)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	}
	else
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
			internalformat, width, height);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

bool GLES3ColorBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteRenderbuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}
