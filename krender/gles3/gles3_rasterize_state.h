// gles3_rasterize_state.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_RASTERIZE_STATE_H
#define __GLES3_RASTERIZE_STATE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3RasterizeState

class RenderService;

class GLES3RasterizeState : public RHIRasterizeState
{
public:
	// Create new instance.
	static GLES3RasterizeState* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3RasterizeState* pInstance);

public:
	GLES3RasterizeState(RenderService* pRS);
	virtual ~GLES3RasterizeState();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const rhi_rasterize_desc_t& desc);
	// Delete resource data.
	bool DeleteResource();

	// Get rasterize description.
	const rhi_rasterize_desc_t& GetDesc() const { return m_RasterizeDesc;  }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	rhi_rasterize_desc_t m_RasterizeDesc;
};

#endif // __GLES3_RASTERIZE_STATE_H
