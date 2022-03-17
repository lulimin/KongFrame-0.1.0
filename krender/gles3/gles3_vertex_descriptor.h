// gles3_vertex_descriptor.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_VERTEX_DESCRIPTOR_H
#define __GLES3_VERTEX_DESCRIPTOR_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3VertexDescriptor

class RenderService;

class GLES3VertexDescriptor : public RHIVertexDescriptor
{
public:
	// Create new instance.
	static GLES3VertexDescriptor* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3VertexDescriptor* pInstance);

public:
	GLES3VertexDescriptor(RenderService* pRS);
	virtual ~GLES3VertexDescriptor();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const rhi_vertex_attrib_t* pAttribs,
		int attrib_num, unsigned int stride);
	// Delete resource data.
	bool DeleteResource();

	// Get vertex description.
	const rhi_vertex_attrib_t* GetAttribs() const { return m_VertexAttribs; }
	int GetAttribNum() const { return m_nAttribNum; }
	unsigned int GetStride() const { return m_nStride; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	rhi_vertex_attrib_t m_VertexAttribs[RHI_VERTEX_ATTRIB_MAX];
	int m_nAttribNum;
	unsigned int m_nStride;
};

#endif // __GLES3_VERTEX_DESCRIPTOR_H
