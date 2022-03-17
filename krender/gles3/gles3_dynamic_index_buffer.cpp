// gles3_dynamic_index_buffer.cpp
// Created by lulimin on 2020/10/20.

#include "gles3_dynamic_index_buffer.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DynamicIndexBuffer

GLES3DynamicIndexBuffer* GLES3DynamicIndexBuffer::CreateInstance(
	RenderService* pRS)
{
	GLES3DynamicIndexBuffer* p = (GLES3DynamicIndexBuffer*)K_ALLOC(sizeof(
		GLES3DynamicIndexBuffer));

	new (p) GLES3DynamicIndexBuffer(pRS);
	return p;
}

void GLES3DynamicIndexBuffer::DeleteInstance(GLES3DynamicIndexBuffer* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DynamicIndexBuffer::GLES3DynamicIndexBuffer(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSize = 0;
	m_nBuffer = 0;
}

GLES3DynamicIndexBuffer::~GLES3DynamicIndexBuffer()
{
	this->DeleteResource();
}

void GLES3DynamicIndexBuffer::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DynamicIndexBuffer::CreateResource(size_t index, unsigned int size)
{
	Assert(size > 0);

	m_nIndex = index;

	glGenBuffers(1, &m_nBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
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

bool GLES3DynamicIndexBuffer::DeleteResource()
{
	if (m_nBuffer != 0)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}

	return true;
}

/*
bool GLES3DynamicIndexBuffer::UpdateResource(unsigned int offset,
	const void* data, unsigned int size)
{
	Assert(data != NULL);
	Assert(m_nBuffer != 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nBuffer);
	
//	// Optimize performance.
//	if ((offset == 0) && (size == m_nSize))
//	{
//		// Discard all old data.
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
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

	void* pBuffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, size,
		flags);

	memcpy(pBuffer, data, size);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return true;
}
*/
