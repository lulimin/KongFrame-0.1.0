// gles3_dynamic_uniform_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_dynamic_uniform_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DynamicUniformBuffer

GLES3DynamicUniformBuffer* GLES3DynamicUniformBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3DynamicUniformBuffer* p = (GLES3DynamicUniformBuffer*)K_ALLOC(sizeof(
		GLES3DynamicUniformBuffer));

	new (p) GLES3DynamicUniformBuffer(pRS);
	return p;
}

void GLES3DynamicUniformBuffer::DeleteInstance(
	GLES3DynamicUniformBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DynamicUniformBuffer::GLES3DynamicUniformBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nBuffer = 0;
}

GLES3DynamicUniformBuffer::~GLES3DynamicUniformBuffer()
{
	this->DeleteResource();
}

void GLES3DynamicUniformBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DynamicUniformBuffer::CreateResource(size_t index, unsigned int size)
{
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_nBuffer);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
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

bool GLES3DynamicUniformBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}

/*
bool GLES3DynamicUniformBuffer::UpdateResource(unsigned int offset,
	const void* data, unsigned int size)
{
	Assert(data != NULL);
	Assert(m_nBuffer != 0);
	
	glBindBuffer(GL_UNIFORM_BUFFER, m_nBuffer);
	
//	// Optimize performance.
//	if ((offset == 0) && (size == m_nSize))
//	{
//		// Discard all old data.
//		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
//	}

	GLbitfield flags = GL_MAP_WRITE_BIT;

	if ((offset == 0) && (size == m_nSize))
	{
		flags |= GL_MAP_INVALIDATE_BUFFER_BIT;
	}
	else
	{
		flags |= GL_MAP_INVALIDATE_RANGE_BIT;
	}

	void* pBuffer = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, flags);

	memcpy(pBuffer, data, size);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return true;
}
*/