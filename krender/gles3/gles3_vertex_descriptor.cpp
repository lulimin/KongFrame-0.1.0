// gles3_vertex_descriptor.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_vertex_descriptor.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3VertexDescriptor

GLES3VertexDescriptor* GLES3VertexDescriptor::CreateInstance(RenderService* pRS)
{
	GLES3VertexDescriptor* p = (GLES3VertexDescriptor*)K_ALLOC(
		sizeof(GLES3VertexDescriptor));

	new (p) GLES3VertexDescriptor(pRS);
	return p;
}

void GLES3VertexDescriptor::DeleteInstance(GLES3VertexDescriptor* pInstance)
{
	K_DELETE(pInstance);
}

GLES3VertexDescriptor::GLES3VertexDescriptor(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	memset(&m_VertexAttribs[0], 0, sizeof(m_VertexAttribs));
	m_nAttribNum = 0;
	m_nStride = 0;
}

GLES3VertexDescriptor::~GLES3VertexDescriptor()
{
	this->DeleteResource();
}

void GLES3VertexDescriptor::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3VertexDescriptor::CreateResource(size_t index,
	const rhi_vertex_attrib_t* pAttribs, int attrib_num, unsigned int stride)
{
	Assert((attrib_num > 0) && (attrib_num <= RHI_VERTEX_ATTRIB_MAX));

	m_nIndex = index;
	memcpy(m_VertexAttribs, pAttribs, attrib_num * sizeof(rhi_vertex_attrib_t));
	m_nAttribNum = attrib_num;
	m_nStride = stride;
	return true;
}

bool GLES3VertexDescriptor::DeleteResource()
{
	return true;
}
