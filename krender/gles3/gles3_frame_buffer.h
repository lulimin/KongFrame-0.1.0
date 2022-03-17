// gles3_frame_buffer.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_FRAME_BUFFER_H
#define __GLES3_FRAME_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3FrameBuffer

class RenderService;

class GLES3FrameBuffer : public RHIFrameBuffer
{
public:
	// Create new instance.
	static GLES3FrameBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3FrameBuffer* pInstance);

public:
	GLES3FrameBuffer(RenderService* pRS);
	virtual ~GLES3FrameBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, GLuint color_buffer, GLuint depth_buffer);
	// Delete resource data.
	bool DeleteResource();

	// Get frame buffer identity.
	GLuint GetFrame() const { return m_nFrame;  }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	GLuint m_nFrame;
};

#endif // __GLES3_FRAME_BUFFER_H
