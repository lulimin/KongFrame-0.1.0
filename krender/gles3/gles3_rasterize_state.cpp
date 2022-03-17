// gles3_rasterize_state.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_rasterize_state.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3RasterizeState

GLES3RasterizeState* GLES3RasterizeState::CreateInstance(RenderService* pRS)
{
	GLES3RasterizeState* p = (GLES3RasterizeState*)K_ALLOC(
		sizeof(GLES3RasterizeState));

	new (p) GLES3RasterizeState(pRS);
	return p;
}

void GLES3RasterizeState::DeleteInstance(GLES3RasterizeState* pInstance)
{
	K_DELETE(pInstance);
}

GLES3RasterizeState::GLES3RasterizeState(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	memset(&m_RasterizeDesc, 0, sizeof(m_RasterizeDesc));
}

GLES3RasterizeState::~GLES3RasterizeState()
{
	this->DeleteResource();
}

void GLES3RasterizeState::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3RasterizeState::CreateResource(size_t index,
	const rhi_rasterize_desc_t& desc)
{
	m_nIndex = index;
	m_RasterizeDesc = desc;
	return true;
}

bool GLES3RasterizeState::DeleteResource()
{
	return true;
}
