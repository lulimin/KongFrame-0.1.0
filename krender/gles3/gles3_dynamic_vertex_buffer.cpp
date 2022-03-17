// gles3_dynamic_vertex_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_dynamic_vertex_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DynamicVertexBuffer

GLES3DynamicVertexBuffer* GLES3DynamicVertexBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3DynamicVertexBuffer* p = (GLES3DynamicVertexBuffer*)K_ALLOC(sizeof(
		GLES3DynamicVertexBuffer));

	new (p) GLES3DynamicVertexBuffer(pRS);
	return p;
}

void GLES3DynamicVertexBuffer::DeleteInstance(GLES3DynamicVertexBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DynamicVertexBuffer::GLES3DynamicVertexBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nStride = 0;
	m_nOffset = 0;
	m_nBuffer = 0;
}

GLES3DynamicVertexBuffer::~GLES3DynamicVertexBuffer()
{
	this->DeleteResource();
}

void GLES3DynamicVertexBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DynamicVertexBuffer::CreateResource(size_t index, unsigned int size,
	unsigned int stride)
{
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

//	GLenum err = glGetError();
//
//	if (err != GL_NO_ERROR)
//	{
//		return false;
//	}

	m_nSize = size;
	m_nStride = stride;
	return true;
}

bool GLES3DynamicVertexBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}

/*
bool GLES3DynamicVertexBuffer::UpdateResource(unsigned int offset,
	const void* data, unsigned int size)
{
	Assert(data != NULL);
	Assert(m_nBuffer != 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	
//	// Optimize performance.
//	if ((offset == 0) && (size == m_nSize))
//	{
//		// Discard all old data.
//		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
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

	void* pBuffer = glMapBufferRange(GL_ARRAY_BUFFER, offset, size, flags);

	memcpy(pBuffer, data, size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
}
*/