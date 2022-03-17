// gles3_static_uniform_buffer.h
// Created by lulimin on 2020/10/20.

#ifndef __GLES3_STATIC_UNIFORM_BUFFER_H
#define __GLES3_STATIC_UNIFORM_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3StaticUniformBuffer

class RenderService;

class GLES3StaticUniformBuffer : public RHIStaticUniformBuffer
{
public:
	// Create new instance.
	static GLES3StaticUniformBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3StaticUniformBuffer* pInstance);

public:
	GLES3StaticUniformBuffer(RenderService* pRS);
	virtual ~GLES3StaticUniformBuffer();

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

#endif // __GLES3_STATIC_UNIFORM_BUFFER_H
