// gles3_sampler_state.h
// Created by lulimin on 2020/11/10.

#ifndef __GLES3_SAMPLER_STATE_H
#define __GLES3_SAMPLER_STATE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3SamplerState

class RenderService;

class GLES3SamplerState : public RHISamplerState
{
public:
	// Create new instance.
	static GLES3SamplerState* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3SamplerState* pInstance);

public:
	GLES3SamplerState(RenderService* pRS);
	virtual ~GLES3SamplerState();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const rhi_sampler_desc_t& desc);
	// Delete resource data.
	bool DeleteResource();

	// Get sampler identity.
	GLuint GetSampler() const { return m_nSampler; }
	// Get sampler description.
	const rhi_sampler_desc_t& GetDesc() const { return m_SamplerDesc; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	GLuint m_nSampler;
	rhi_sampler_desc_t m_SamplerDesc;
};

#endif // __GLES3_SAMPLER_STATE_H
