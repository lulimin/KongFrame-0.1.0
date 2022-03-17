// gles3_blend_state.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_BLEND_STATE_H
#define __GLES3_BLEND_STATE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3BlendState

class RenderService;

class GLES3BlendState : public RHIBlendState
{
public:
	// Create new instance.
	static GLES3BlendState* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3BlendState* pInstance);

public:
	GLES3BlendState(RenderService* pRS);
	virtual ~GLES3BlendState();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const rhi_blend_desc_t& desc);
	// Delete resource data.
	bool DeleteResource();

	// Get blend description.
	const rhi_blend_desc_t& GetDesc() const { return m_BlendDesc; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	rhi_blend_desc_t m_BlendDesc;
};

#endif // __GLES3_BLEND_STATE_H
