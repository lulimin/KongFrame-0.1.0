// gles3_dynamic_vertex_buffer.h
// Created by lulimin on 2020/10/20.

#ifndef __GLES3_DYNAMIC_VERTEX_BUFFER_H
#define __GLES3_DYNAMIC_VERTEX_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3DynamicVertexBuffer

class RenderService;

class GLES3DynamicVertexBuffer : public RHIDynamicVertexBuffer
{
public:
	// Create new instance.
	static GLES3DynamicVertexBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3DynamicVertexBuffer* pInstance);

public:
	GLES3DynamicVertexBuffer(RenderService* pRS);
	virtual ~GLES3DynamicVertexBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex;  }
	// Get buffer size.
	virtual unsigned int GetBufferSize() { return m_nSize; }

	// Get stride of vertex data.
	virtual unsigned int GetStride() { return m_nStride; }
	// Set stride of vertex data.
	virtual void SetStride(unsigned int stride) { m_nStride = stride; }
	// Offset bytes.
	virtual void SetOffset(unsigned int offset) { m_nOffset = offset; }
	virtual unsigned int GetOffset() { return m_nOffset; }

	// Create resource data.
	bool CreateResource(size_t index, unsigned int size, unsigned int stride);
	// Delete resource data.
	bool DeleteResource();
	
	// Get buffer identity.
	GLuint GetBuffer() const { return m_nBuffer; }
	// Get buffer size.
	unsigned int GetSize() const { return m_nSize; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	unsigned int m_nSize;
	unsigned int m_nStride;
	unsigned int m_nOffset;
	GLuint m_nBuffer;
};

#endif // __GLES3_DYNAMIC_VERTEX_BUFFER_H
