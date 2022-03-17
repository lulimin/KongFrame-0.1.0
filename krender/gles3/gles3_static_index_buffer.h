// gles3_static_index_buffer.h
// Created by lulimin on 2020/10/20.

#ifndef __GLES3_STATIC_INDEX_BUFFER_H
#define __GLES3_STATIC_INDEX_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3StaticIndexBuffer

class RenderService;

class GLES3StaticIndexBuffer : public RHIStaticIndexBuffer
{
public:
	// Create new instance.
	static GLES3StaticIndexBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3StaticIndexBuffer* pInstance);

public:
	GLES3StaticIndexBuffer(RenderService* pRS);
	virtual ~GLES3StaticIndexBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex;  }
	// Get buffer size.
	virtual unsigned int GetBufferSize() { return m_nSize; }

	// Create resource data.
	bool CreateResource(size_t index, const void* data, unsigned int size);
	// Delete resource data.
	bool DeleteResource();

	// Get buffer identity.
	GLuint GetBuffer() const { return m_nBuffer; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	unsigned int m_nSize;
	GLuint m_nBuffer;
};

#endif // __GLES3_STATIC_INDEX_BUFFER_H
