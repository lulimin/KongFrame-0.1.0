// gles3_static_index_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_static_index_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3StaticIndexBuffer

GLES3StaticIndexBuffer* GLES3StaticIndexBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3StaticIndexBuffer* p = (GLES3StaticIndexBuffer*)K_ALLOC(sizeof(
		GLES3StaticIndexBuffer));

	new (p) GLES3StaticIndexBuffer(pRS);
	return p;
}

void GLES3StaticIndexBuffer::DeleteInstance(GLES3StaticIndexBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3StaticIndexBuffer::GLES3StaticIndexBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nBuffer = 0;
}

GLES3StaticIndexBuffer::~GLES3StaticIndexBuffer()
{
	this->DeleteResource();
}

void GLES3StaticIndexBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3StaticIndexBuffer::CreateResource(size_t index, const void* data,
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//	GLenum err = glGetError();
//
//	if (err != GL_NO_ERROR)
//	{
//		return false;
//	}

	m_nSize = size;
	return true;
}

bool GLES3StaticIndexBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}
