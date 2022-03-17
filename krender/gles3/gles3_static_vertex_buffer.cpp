// gles3_static_vertex_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_static_vertex_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3StaticVertexBuffer

GLES3StaticVertexBuffer* GLES3StaticVertexBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3StaticVertexBuffer* p = (GLES3StaticVertexBuffer*)K_ALLOC(sizeof(
		GLES3StaticVertexBuffer));

	new (p) GLES3StaticVertexBuffer(pRS);
	return p;
}

void GLES3StaticVertexBuffer::DeleteInstance(GLES3StaticVertexBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3StaticVertexBuffer::GLES3StaticVertexBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nStride = 0;
	m_nOffset = 0;
	m_nBuffer = 0;
}

GLES3StaticVertexBuffer::~GLES3StaticVertexBuffer()
{
	this->DeleteResource();
}

void GLES3StaticVertexBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3StaticVertexBuffer::CreateResource(size_t index, const void* data,
	unsigned int size, unsigned int stride)
{
	Assert(data != NULL);
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
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

bool GLES3StaticVertexBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}
