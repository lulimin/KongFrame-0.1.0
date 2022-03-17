// gles3_depth_stencil_state.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_depth_stencil_state.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3DepthStencilState

GLES3DepthStencilState* GLES3DepthStencilState::CreateInstance(RenderService* pRS)
{
	GLES3DepthStencilState* p = (GLES3DepthStencilState*)K_ALLOC(
		sizeof(GLES3DepthStencilState));

	new (p) GLES3DepthStencilState(pRS);
	return p;
}

void GLES3DepthStencilState::DeleteInstance(GLES3DepthStencilState* pInstance)
{
	K_DELETE(pInstance);
}

GLES3DepthStencilState::GLES3DepthStencilState(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	memset(&m_DepthStencilDesc, 0, sizeof(m_DepthStencilDesc));
}

GLES3DepthStencilState::~GLES3DepthStencilState()
{
	this->DeleteResource();
}

void GLES3DepthStencilState::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3DepthStencilState::CreateResource(size_t index, 
	const rhi_depth_stencil_desc_t& desc)
{
	m_nIndex = index;
	m_DepthStencilDesc = desc;
	return true;
}

bool GLES3DepthStencilState::DeleteResource()
{
	return true;
}
