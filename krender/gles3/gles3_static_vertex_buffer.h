// gles3_static_vertex_buffer.h
// Created by lulimin on 2020/10/20.

#ifndef __GLES3_STATIC_VERTEX_BUFFER_H
#define __GLES3_STATIC_VERTEX_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3StaticVertexBuffer

class RenderService;

class GLES3StaticVertexBuffer : public RHIStaticVertexBuffer
{
public:
	// Create new instance.
	static GLES3StaticVertexBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3StaticVertexBuffer* pInstance);

public:
	GLES3StaticVertexBuffer(RenderService* pRS);
	virtual ~GLES3StaticVertexBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex;  }
	// Get buffer size.
	virtual unsigned int GetBufferSize() { return m_nSize; }

	// Get stride of vertex data.
	virtual unsigned int GetStride() { return m_nStride; }
	// Offset bytes.
	virtual void SetOffset(unsigned int offset) { m_nOffset = offset; }
	virtual unsigned int GetOffset() { return m_nOffset; }

	// Create resource data.
	bool CreateResource(size_t index, const void* data, unsigned int size,
		unsigned int stride);
	// Delete resource data.
	bool DeleteResource();

	// Get buffer identity.
	GLuint GetBuffer() const { return m_nBuffer; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	unsigned int m_nSize;
	unsigned int m_nStride;
	unsigned int m_nOffset;
	GLuint m_nBuffer;
};

#endif // __GLES3_STATIC_VERTEX_BUFFER_H
