// gles3_dynamic_index_buffer.h
// Created by lulimin on 2020/10/20.

#ifndef __GLES3_DYNAMIC_INDEX_BUFFER_H
#define __GLES3_DYNAMIC_INDEX_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3DynamicIndexBuffer

class RenderService;

class GLES3DynamicIndexBuffer : public RHIDynamicIndexBuffer
{
public:
	// Create new instance.
	static GLES3DynamicIndexBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3DynamicIndexBuffer* pInstance);

public:
	GLES3DynamicIndexBuffer(RenderService* pRS);
	virtual ~GLES3DynamicIndexBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex;  }
	// Get buffer size.
	virtual unsigned int GetBufferSize() { return m_nSize; }

	// Create resource data.
	bool CreateResource(size_t index, unsigned int size);
	// Delete resource data.
	bool DeleteResource();

	// Get buffer identity.
	GLuint GetBuffer() const { return m_nBuffer;  }
	// Get buffer size.
	unsigned int GetSize() const { return m_nSize; }
	
private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	unsigned int m_nSize;
	GLuint m_nBuffer;
};

#endif // __GLES3_DYNAMIC_INDEX_BUFFER_H
