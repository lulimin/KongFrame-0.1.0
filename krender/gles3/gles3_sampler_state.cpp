// gles3_sampler_state.cpp
// Created by lulimin on 2020/11/10.

#include "gles3_sampler_state.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3SamplerState

GLES3SamplerState* GLES3SamplerState::CreateInstance(RenderService* pRS)
{
	GLES3SamplerState* p = (GLES3SamplerState*)K_ALLOC(sizeof(GLES3SamplerState));

	new (p) GLES3SamplerState(pRS);
	return p;
}

void GLES3SamplerState::DeleteInstance(GLES3SamplerState* pInstance)
{
	K_DELETE(pInstance);
}

GLES3SamplerState::GLES3SamplerState(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nSampler = 0;
	memset(&m_SamplerDesc, 0, sizeof(m_SamplerDesc));
}

GLES3SamplerState::~GLES3SamplerState()
{
	this->DeleteResource();
}

void GLES3SamplerState::Release()
{
	m_pRenderService->ReleaseResource(this);
}

static inline GLenum get_filter_mode(int filter)
{
	switch (filter)
	{
	case RHI_TEXTURE_FILTER_NEAREST:
		return GL_NEAREST;
	case RHI_TEXTURE_FILTER_LINEAR:
		return GL_LINEAR;
	case RHI_TEXTURE_FILTER_ANISOTROPIC:
		return GL_LINEAR;
	default:
		Assert(0);
		break;
	}

	return GL_LINEAR;
}

static inline GLenum get_address_mode(int address)
{
	switch (address)
	{
	case RHI_TEXTURE_ADDRESS_WRAP:
		return GL_REPEAT;
	case RHI_TEXTURE_ADDRESS_MIRROR:
		return GL_MIRRORED_REPEAT;
	case RHI_TEXTURE_ADDRESS_CLAMP:
		return GL_CLAMP_TO_EDGE;
	default:
		Assert(0);
		break;
	}

	return GL_REPEAT;
}

static inline GLenum get_comp_func(int func)
{
	switch (func)
	{
	case RHI_TEXTURE_COMPARISON_NEVER:
		return GL_NEVER;
	case RHI_TEXTURE_COMPARISON_LESS:
		return GL_LESS;
	case RHI_TEXTURE_COMPARISON_EQUAL:
		return GL_EQUAL;
	case RHI_TEXTURE_COMPARISON_LESS_EQUAL:
		return GL_LEQUAL;
	case RHI_TEXTURE_COMPARISON_GREATER:
		return GL_GREATER;
	case RHI_TEXTURE_COMPARISON_NOT_EQUAL:
		return GL_NOTEQUAL;
	case RHI_TEXTURE_COMPARISON_GREATER_EQUAL:
		return GL_GEQUAL;
	case RHI_TEXTURE_COMPARISON_ALWAYS:
		return GL_ALWAYS;
	default:
		Assert(0);
		break;
	}

	return GL_LEQUAL;
}

bool GLES3SamplerState::CreateResource(size_t index,
	const rhi_sampler_desc_t& desc)
{
	m_nIndex = index;
	m_SamplerDesc = desc;

	glGenSamplers(1, &m_nSampler);

	GLenum min_filter = get_filter_mode(desc.nMinFilter);
	GLenum mag_filter = get_filter_mode(desc.nMagFilter);
	GLenum wrap_s = get_address_mode(desc.nAddressU);
	GLenum wrap_t = get_address_mode(desc.nAddressV);
	GLenum wrap_r = get_address_mode(desc.nAddressW);
	GLenum comp_func = get_comp_func(desc.nCompareFunc);
	GLenum comp_mode;

	if (desc.nCompareMode)
	{
		comp_mode = GL_COMPARE_REF_TO_TEXTURE;
	}
	else
	{
		comp_mode = GL_NONE;
	}

	glSamplerParameteri(m_nSampler, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_MAG_FILTER, mag_filter);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_WRAP_S, wrap_s);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_WRAP_T, wrap_t);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_WRAP_R, wrap_r);
	glSamplerParameterf(m_nSampler, GL_TEXTURE_MIN_LOD, desc.fMinLOD);
	glSamplerParameterf(m_nSampler, GL_TEXTURE_MAX_LOD, desc.fMaxLOD);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_COMPARE_MODE, comp_mode);
	glSamplerParameteri(m_nSampler, GL_TEXTURE_COMPARE_FUNC, comp_func);
	return true;
}

bool GLES3SamplerState::DeleteResource()
{
	if (m_nSampler != 0)
	{
		glDeleteSamplers(1, &m_nSampler);
		m_nSampler = 0;
	}
	
	return true;
}
