// gles3_static_uniform_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_static_uniform_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3StaticUniformBuffer

GLES3StaticUniformBuffer* GLES3StaticUniformBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3StaticUniformBuffer* p = (GLES3StaticUniformBuffer*)K_ALLOC(sizeof(
		GLES3StaticUniformBuffer));

	new (p) GLES3StaticUniformBuffer(pRS);
	return p;
}

void GLES3StaticUniformBuffer::DeleteInstance(GLES3StaticUniformBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3StaticUniformBuffer::GLES3StaticUniformBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nBuffer = 0;
}

GLES3StaticUniformBuffer::~GLES3StaticUniformBuffer()
{
	this->DeleteResource();
}

void GLES3StaticUniformBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3StaticUniformBuffer::CreateResource(size_t index, const void* data,
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_nBuffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

//	GLenum err = glGetError();
//
//	if (err != GL_NO_ERROR)
//	{
//		return false;
//	}

	m_nSize = size;
	return true;
}

bool GLES3StaticUniformBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}
