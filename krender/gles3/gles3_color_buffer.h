// gles3_color_buffer.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_COLOR_BUFFER_H
#define __GLES3_COLOR_BUFFER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3ColorBuffer

class RenderService;

class GLES3ColorBuffer : public RHIColorBuffer
{
public:
	// Create new instance.
	static GLES3ColorBuffer* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3ColorBuffer* pInstance);

public:
	GLES3ColorBuffer(RenderService* pRS);
	virtual ~GLES3ColorBuffer();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, int width, int height, int format, 
		int samples);
	// Delete resource data.
	bool DeleteResource();

	// Get buffer identity.
	GLuint GetBuffer() const { return m_nBuffer;  }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nSamples;
	GLuint m_nBuffer;
};

#endif // __GLES3_COLOR_BUFFER_H
