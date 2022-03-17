// gles3_depth_stencil_state.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_DEPTH_STENCIL_STATE_H
#define __GLES3_DEPTH_STENCIL_STATE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3DepthStencilState

class RenderService;

class GLES3DepthStencilState : public RHIDepthStencilState
{
public:
	// Create new instance.
	static GLES3DepthStencilState* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3DepthStencilState* pInstance);

public:
	GLES3DepthStencilState(RenderService* pRS);
	virtual ~GLES3DepthStencilState();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const rhi_depth_stencil_desc_t& desc);
	// Delete resource data.
	bool DeleteResource();

	// Get depth & stencil description.
	const rhi_depth_stencil_desc_t& GetDesc() const
	{ 
		return m_DepthStencilDesc; 
	}

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	rhi_depth_stencil_desc_t m_DepthStencilDesc;
};

#endif // __GLES3_DEPTH_STENCIL_STATE_H
