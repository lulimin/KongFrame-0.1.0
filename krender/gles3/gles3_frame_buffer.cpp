// gles3_frame_buffer.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_frame_buffer.h"
#include "gles3_utility.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3FrameBuffer

GLES3FrameBuffer* GLES3FrameBuffer::CreateInstance(RenderService* pRS)
{
	GLES3FrameBuffer* p = (GLES3FrameBuffer*)K_ALLOC(sizeof(GLES3FrameBuffer));

	new (p) GLES3FrameBuffer(pRS);
	return p;
}

void GLES3FrameBuffer::DeleteInstance(GLES3FrameBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3FrameBuffer::GLES3FrameBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nFrame = 0;
}

GLES3FrameBuffer::~GLES3FrameBuffer()
{
	this->DeleteResource();
}

void GLES3FrameBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3FrameBuffer::CreateResource(size_t index, GLuint color_buffer,
	GLuint depth_buffer)
{
	m_nIndex = index;

	glGenFramebuffers(1, &m_nFrame);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFrame);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
		GL_RENDERBUFFER, color_buffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, depth_buffer);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		const char* info = gles3_framebuffer_status_string(status);

		K_LOG("(GLES3FrameBuffer::CreateResource)Frame buffer not complete.");
		K_LOG(info);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool GLES3FrameBuffer::DeleteResource()
{
	if (m_nFrame != 0)
	{
		glDeleteFramebuffers(1, &m_nFrame);
		m_nFrame = 0;
	}

	return true;
}
