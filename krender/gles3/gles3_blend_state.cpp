// gles3_blend_state.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_blend_state.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3BlendState

GLES3BlendState* GLES3BlendState::CreateInstance(RenderService* pRS)
{
	GLES3BlendState* p = (GLES3BlendState*)K_ALLOC(sizeof(GLES3BlendState));

	new (p) GLES3BlendState(pRS);
	return p;
}

void GLES3BlendState::DeleteInstance(GLES3BlendState* pInstance)
{
	K_DELETE(pInstance);
}

GLES3BlendState::GLES3BlendState(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	memset(&m_BlendDesc, 0, sizeof(m_BlendDesc));
}

GLES3BlendState::~GLES3BlendState()
{
	this->DeleteResource();
}

void GLES3BlendState::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3BlendState::CreateResource(size_t index, const rhi_blend_desc_t& desc)
{
	m_nIndex = index;
	m_BlendDesc = desc;
	return true;
}

bool GLES3BlendState::DeleteResource()
{
	return true;
}
