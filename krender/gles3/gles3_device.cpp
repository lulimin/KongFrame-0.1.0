// gles3_device.cpp
// Created by lulimin on 2020/10/16.

#include "gles3_device.h"
#include "gles3_frame_buffer.h"
#include "gles3_color_buffer.h"
#include "gles3_depth_buffer.h"
#include "gles3_color_texture.h"
#include "gles3_depth_texture.h"
#include "gles3_vertex_shader.h"
#include "gles3_pixel_shader.h"
#include "gles3_shader_program.h"
#include "gles3_vertex_descriptor.h"
#include "gles3_static_vertex_buffer.h"
#include "gles3_static_index_buffer.h"
#include "gles3_static_uniform_buffer.h"
#include "gles3_static_texture.h"
#include "gles3_dynamic_vertex_buffer.h"
#include "gles3_dynamic_index_buffer.h"
#include "gles3_dynamic_uniform_buffer.h"
#include "gles3_dynamic_texture.h"
#include "gles3_sampler_state.h"
#include "gles3_rasterize_state.h"
#include "gles3_depth_stencil_state.h"
#include "gles3_blend_state.h"
#include "gles3_utility.h"
#include "gles3_shader_texture.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"
#include <string.h>

// Create OpenGLES 3.0 render device.
RHIDevice* create_gles3_device(RenderService* pRS)
{
	GLES3Device* p = (GLES3Device*)K_ALLOC(sizeof(GLES3Device));

	new (p) GLES3Device(pRS);
	return p;
}

// GLES3Device

GLES3Device::GLES3Device(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_bSaveShader = true;
	m_pCurrentProgram = NULL;
	m_pCurrentRasterizeState = NULL;
	m_pCurrentDepthStencilState = NULL;
	m_nCurrentStencilRef = 0;
	m_pCurrentBlendState = NULL;
	m_fCurrentBlendFactor[0] = 1.0F;
	m_fCurrentBlendFactor[1] = 1.0F;
	m_fCurrentBlendFactor[2] = 1.0F;
	m_fCurrentBlendFactor[3] = 1.0F;
	m_nCurrentIndexFormat = 0;
	m_nCurrentIndexOffset = 0;
}

GLES3Device::~GLES3Device()
{
}

void GLES3Device::Release()
{
	K_DELETE(this);
}

bool GLES3Device::Startup()
{
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* shading = glGetString(GL_SHADING_LANGUAGE_VERSION);
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);

	K_LOG((const char*)vendor);
	K_LOG((const char*)renderer);
	K_LOG((const char*)version);
	K_LOG((const char*)shading);
	K_LOG((const char*)extensions);

	GLint format_num;
	
	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &format_num);
	m_bSaveShader = (format_num > 0);
	return true;
}

bool GLES3Device::Cleanup()
{
	return true;
}

bool GLES3Device::SetFrameBuffer(RHIFrameBuffer* pFB)
{
	if (NULL == pFB)
	{
		// Use system default back buffer.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	GLES3FrameBuffer* pFBuffer = (GLES3FrameBuffer*)pFB;
	GLuint frame_buffer = pFBuffer->GetFrame();

	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	return true;
}

bool GLES3Device::ClearFrame(float color_r, float color_g, float color_b,
	float color_a, float depth, int stencil)
{
	glClearColor(color_r, color_g, color_b, color_a);
	glClearDepthf(depth);
	glClearStencil(stencil);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	return true;
}

bool GLES3Device::SetViewport(int left, int top, int width, int height)
{
	int back_buffer_height = m_pRenderService->GetBackBufferHeight();
	int bottom = back_buffer_height - top - height;
	
	glViewport(left, bottom, width, height);
	return true;
}

bool GLES3Device::SetScissorRect(int left, int top, int width, int height)
{
	int back_buffer_height = m_pRenderService->GetBackBufferHeight();
	int bottom = back_buffer_height - top - height;
	
	glScissor(left, bottom, width, height);
	return true;
}

bool GLES3Device::SetShaderProgram(RHIShaderProgram* pSP)
{
	Assert(pSP != NULL);

	GLES3ShaderProgram* p = (GLES3ShaderProgram*)pSP;

	if (m_pCurrentProgram == p)
	{
		// No change.
		return true;
	}

	GLuint program = p->GetProgram();

	glUseProgram(program);
	m_pCurrentProgram = p;
	return true;
}

static inline GLenum get_attrib_type(int attrib_type)
{
	switch (attrib_type)
	{
	case RHI_VERTEX_DATA_BYTE:
		return GL_BYTE;
	case RHI_VERTEX_DATA_UNSIGNED_BYTE:
		return GL_UNSIGNED_BYTE;
	case RHI_VERTEX_DATA_SHORT:
		return GL_SHORT;
	case RHI_VERTEX_DATA_UNSIGNED_SHORT:
		return GL_UNSIGNED_SHORT;
	case RHI_VERTEX_DATA_INT:
		return GL_INT;
	case RHI_VERTEX_DATA_UNSIGNED_INT:
		return GL_UNSIGNED_INT;
	case RHI_VERTEX_DATA_HALF_FLOAT:
		return GL_HALF_FLOAT;
	case RHI_VERTEX_DATA_FLOAT:
		return GL_FLOAT;
	default:
		Assert(0);
		break;
	}

	return GL_BYTE;
}

//bool GLES3Device::SetVertexDescriptor(RHIVertexDescriptor* pVD)
//{
//	Assert(pVD != NULL);
//
//	GLES3VertexDescriptor* p = (GLES3VertexDescriptor*)pVD;
//	const rhi_vertex_desc_t& desc = p->GetDesc();
//	int attrib_num = desc.nAttribNum;
//
//	for (int i = 0; i < attrib_num; ++i)
//	{
//		const rhi_vertex_attrib_t& attrib = desc.Attribs[i];
//		GLenum type = get_attrib_type(attrib.nType);
//		GLboolean normalized = attrib.bNormalized ? GL_TRUE : GL_FALSE;
//
//		glVertexAttribPointer(i, attrib.nSize, type, normalized,
//			desc.nStride, (const void*)(size_t)attrib.nOffset);
//		glEnableVertexAttribArray(i);
//	}
//
//	if (attrib_num < RHI_VERTEX_ATTRIB_MAX)
//	{
//		glDisableVertexAttribArray(attrib_num);
//	}
//
//	return true;
//}

//bool GLES3Device::SetStaticVertexBuffer(RHIStaticVertexBuffer* pVB)
//{
//	Assert(pVB != NULL);
//
//	GLES3StaticVertexBuffer* p = (GLES3StaticVertexBuffer*)pVB;
//	GLuint buffer = p->GetBuffer();
//	
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	return true;
//}

//bool GLES3Device::SetDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB)
//{
//	Assert(pVB != NULL);
//
//	GLES3DynamicVertexBuffer* p = (GLES3DynamicVertexBuffer*)pVB;
//	GLuint buffer = p->GetBuffer();
//	
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	return true;
//}

bool GLES3Device::SetVertexArray(int vertex_buffer_num, RHIVertexBuffer** vbs,
	RHIVertexDescriptor* pVD)
{
	Assert(vertex_buffer_num > 0);
	Assert(vbs != NULL);
	Assert(pVD != NULL);

	GLES3VertexDescriptor* p = (GLES3VertexDescriptor*)pVD;
	const rhi_vertex_attrib_t* pAttribs = p->GetAttribs();
	int attrib_num = p->GetAttribNum();
	unsigned int stride = p->GetStride();
	int index = 0;

	for (int slot = 0; slot < vertex_buffer_num; ++slot)
	{
		RHIVertexBuffer* pVB = vbs[slot];
		int res_type = pVB->GetResourceType();
		
		if (res_type == RHI_RESOURCE_STATIC_VERTEX_BUFFER)
		{
			GLES3StaticVertexBuffer* pSVB = (GLES3StaticVertexBuffer*)pVB;

			glBindBuffer(GL_ARRAY_BUFFER, pSVB->GetBuffer());
		}
		else if (res_type == RHI_RESOURCE_DYNAMIC_VERTEX_BUFFER)
		{
			GLES3DynamicVertexBuffer* pDVB = (GLES3DynamicVertexBuffer*)pVB;

			glBindBuffer(GL_ARRAY_BUFFER, pDVB->GetBuffer());
		}
		else
		{
			Assert(0);
			K_LOG("(GLES3Device::SetVertexArray)Not vertex buffer.");
			abort();
			return false;
		}

		for (; index < attrib_num; ++index)
		{
			const rhi_vertex_attrib_t& attrib = pAttribs[index];

			if (attrib.nInputSlot < slot)
			{
				continue;
			}
			else if (attrib.nInputSlot > slot)
			{
				break;
			}

			GLenum type = get_attrib_type(attrib.nType);
			GLboolean normalized = attrib.bNormalized ? GL_TRUE : GL_FALSE;

			glVertexAttribPointer(index, attrib.nSize, type, normalized, stride,
				(const void*)(size_t)(pVB->GetOffset() + attrib.nOffset));
			glEnableVertexAttribArray(index);
		}
	}

	if (attrib_num < RHI_VERTEX_ATTRIB_MAX)
	{
		glDisableVertexAttribArray(attrib_num);
	}

	return true;
}

bool GLES3Device::SetStaticIndexBuffer(RHIStaticIndexBuffer* pIB, int format,
	unsigned int offset)
{
	Assert(pIB != NULL);

	GLES3StaticIndexBuffer* p = (GLES3StaticIndexBuffer*)pIB;
	GLuint buffer = p->GetBuffer();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	m_nCurrentIndexFormat = format;
	m_nCurrentIndexOffset = offset;
	return true;
}

bool GLES3Device::SetDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB, int format,
	unsigned int offset)
{
	Assert(pIB != NULL);

	GLES3DynamicIndexBuffer* p = (GLES3DynamicIndexBuffer*)pIB;
	GLuint buffer = p->GetBuffer();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	m_nCurrentIndexFormat = format;
	m_nCurrentIndexOffset = offset;
	return true;
}

static inline bool set_uniform_buffer(GLES3ShaderProgram* pProgram,
	unsigned int slot, GLuint buffer)
{
	if (NULL == pProgram)
	{
		return false;
	}

	int block_index = pProgram->GetUniformBlockIndex(slot);

	if (block_index < 0)
	{
		return false;
	}

	GLuint program = pProgram->GetProgram();

	glBindBufferBase(GL_UNIFORM_BUFFER, slot, buffer);
	glUniformBlockBinding(program, block_index, slot);
	return false;
}

bool GLES3Device::SetStaticUniformBuffer(unsigned int slot,
	RHIStaticUniformBuffer* pUB)
{
	Assert(slot < UNIFORM_BLOCK_MAX);
	Assert(pUB != NULL);

	GLES3StaticUniformBuffer* p = (GLES3StaticUniformBuffer*)pUB;
	GLuint buffer = p->GetBuffer();

	return set_uniform_buffer(m_pCurrentProgram, slot, buffer);
}

bool GLES3Device::SetDynamicUniformBuffer(unsigned int slot, 
	RHIDynamicUniformBuffer* pUB)
{
	Assert(slot < UNIFORM_BLOCK_MAX);
	Assert(pUB != NULL);

	GLES3DynamicUniformBuffer* p = (GLES3DynamicUniformBuffer*)pUB;
	GLuint buffer = p->GetBuffer();

	return set_uniform_buffer(m_pCurrentProgram, slot, buffer);
}

static inline bool set_texture(GLES3ShaderProgram* pProgram,
	unsigned int slot, GLuint texture)
{
	if (NULL == pProgram)
	{
		return false;
	}

	int uniform_index = pProgram->GetSamplerUniformIndex(slot);

	if (uniform_index < 0)
	{
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0 + slot);
	glUniform1i(uniform_index, slot);
	return true;
}

bool GLES3Device::SetStaticTexture(unsigned int slot, RHIStaticTexture* pST)
{
	Assert(slot < SHADER_SAMPLER_MAX);
	Assert(pST != NULL);

	GLES3StaticTexture* p = (GLES3StaticTexture*)pST;
	GLuint texture = p->GetTexture();

	return set_texture(m_pCurrentProgram, slot, texture);
}

bool GLES3Device::SetDynamicTexture(unsigned int slot, RHIDynamicTexture* pDT)
{
	Assert(slot < SHADER_SAMPLER_MAX);
	Assert(pDT != NULL);

	GLES3DynamicTexture* p = (GLES3DynamicTexture*)pDT;
	GLuint texture = p->GetTexture();

	return set_texture(m_pCurrentProgram, slot, texture);
}

bool GLES3Device::SetShaderTexture(unsigned int slot, RHIShaderTexture* pST)
{
	Assert(slot < SHADER_SAMPLER_MAX);
	Assert(pST != NULL);

	GLES3ShaderTexture* p = (GLES3ShaderTexture*)pST;
	GLuint texture = p->GetTexture();

	return set_texture(m_pCurrentProgram, slot, texture);
}

bool GLES3Device::SetColorTexture(unsigned int slot, RHIColorTexture* pCT)
{
	Assert(slot < SHADER_SAMPLER_MAX);
	Assert(pCT != NULL);

	GLES3ColorTexture* p = (GLES3ColorTexture*)pCT;
	GLuint texture = p->GetTexture();

	return set_texture(m_pCurrentProgram, slot, texture);
}

bool GLES3Device::SetDepthTexture(unsigned int slot, RHIDepthTexture* pDT)
{
	Assert(slot < SHADER_SAMPLER_MAX);
	Assert(pDT != NULL);

	GLES3DepthTexture* p = (GLES3DepthTexture*)pDT;
	GLuint texture = p->GetTexture();

	return set_texture(m_pCurrentProgram, slot, texture);
}

bool GLES3Device::SetSamplerState(unsigned int slot, RHISamplerState* pSS)
{
	if (NULL == pSS)
	{
		glBindSampler(slot, 0);
		return true;
	}

	GLES3SamplerState* p = (GLES3SamplerState*)pSS;
	GLuint sampler = p->GetSampler();

	glBindSampler(slot, sampler);
	return true;
}

bool GLES3Device::SetRasterizeState(RHIRasterizeState* pRS)
{
	Assert(pRS != NULL);

	GLES3RasterizeState* p = (GLES3RasterizeState*)pRS;
	const rhi_rasterize_desc_t& desc = p->GetDesc();

	// Cull face.
	switch (desc.nCullMode)
	{
	case RHI_CULL_NONE:
		glDisable(GL_CULL_FACE);
		break;
	case RHI_CULL_FRONT:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	case RHI_CULL_BACK:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	default:
		Assert(0);
		break;
	}

	// Font face.
	if (desc.bFrontCounterClockwise)
	{
		glFrontFace(GL_CCW);
	}
	else
	{
		glFrontFace(GL_CW);
	}

	// Depth bias.
	if (desc.nDepthBias != 0)
	{
		//glPolygonOffset(desc.fSlopeScaledDepthBias, desc.nDepthBias * 0.001F);
		glPolygonOffset(desc.fSlopeScaledDepthBias, desc.fDepthBiasClamp);
		glEnable(GL_POLYGON_OFFSET_FILL);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	// Scissor test.
	if (desc.bScissorEnable)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}

	// Sample coverage.
	if (desc.bMultisampleEnable)
	{
		glEnable(GL_SAMPLE_COVERAGE);
	}
	else
	{
		glDisable(GL_SAMPLE_COVERAGE);
	}

	return true;
}

static inline GLenum get_compare_func(int func)
{
	switch (func)
	{
	case RHI_COMPARISON_NEVER:
		return GL_NEVER;
	case RHI_COMPARISON_LESS:
		return GL_LESS;
	case RHI_COMPARISON_EQUAL:
		return GL_EQUAL;
	case RHI_COMPARISON_LESS_EQUAL:
		return GL_LEQUAL;
	case RHI_COMPARISON_GREATER:
		return GL_GREATER;
	case RHI_COMPARISON_NOT_EQUAL:
		return GL_NOTEQUAL;
	case RHI_COMPARISON_GREATER_EQUAL:
		return GL_GEQUAL;
	case RHI_COMPARISON_ALWAYS:
		return GL_ALWAYS;
	default:
		Assert(0);
		break;
	}

	return GL_LEQUAL;
}

static inline GLenum get_stencil_op(int op)
{
	switch (op)
	{
	case RHI_STENCIL_OP_KEEP:
		return GL_KEEP;
	case RHI_STENCIL_OP_ZERO:
		return GL_ZERO;
	case RHI_STENCIL_OP_REPLACE:
		return GL_REPLACE;
	case RHI_STENCIL_OP_INCR_SAT:
		return GL_INCR_WRAP;
	case RHI_STENCIL_OP_DECR_SAT:
		return GL_DECR_WRAP;
	case RHI_STENCIL_OP_INVERT:
		return GL_INVERT;
	case RHI_STENCIL_OP_INCR:
		return GL_INCR;
	case RHI_STENCIL_OP_DECR:
		return GL_DECR;
	default:
		Assert(0);
		break;
	}

	return GL_KEEP;
}

bool GLES3Device::SetDepthStencilState(RHIDepthStencilState* pDSS,
	unsigned int stencil_ref)
{
	Assert(pDSS != NULL);

	GLES3DepthStencilState* p = (GLES3DepthStencilState*)pDSS;
	const rhi_depth_stencil_desc_t& desc = p->GetDesc();

	// Depth test.
	if (desc.bDepthEnable)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	// Depth mask.
	if (desc.nDepthWriteMask == 0)
	{
		glDepthMask(GL_FALSE);
	}
	else
	{
		glDepthMask(GL_TRUE);
	}

	// Depth func.
	GLenum depth_func = get_compare_func(desc.nDepthFunc);

	glDepthFunc(depth_func);

	// Stencil test.
	if (desc.bStencilEnable)
	{
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}

	// Stencil mask.
	glStencilMask((GLuint)desc.nStencilWriteMask);

	// Stencil func.
	GLenum front_stencil_func = get_compare_func(desc.nFrontStencilFunc);
	GLenum back_stencil_func = get_compare_func(desc.nBackStencilFunc);

	glStencilFuncSeparate(GL_FRONT, front_stencil_func, stencil_ref,
		desc.nStencilReadMask);
	glStencilFuncSeparate(GL_BACK, back_stencil_func, stencil_ref,
		desc.nStencilReadMask);

	// Stencil op.
	GLenum front_fail_op = get_stencil_op(desc.nFrontStencilFailOp);
	GLenum front_zfail_op = get_stencil_op(desc.nFrontStencilDepthFailOp);
	GLenum front_pass_op = get_stencil_op(desc.nFrontStencilPassOp);
	GLenum back_fail_op = get_stencil_op(desc.nBackStencilFailOp);
	GLenum back_zfail_op = get_stencil_op(desc.nBackStencilDepthFailOp);
	GLenum back_pass_op = get_stencil_op(desc.nBackStencilPassOp);

	glStencilOpSeparate(GL_FRONT, front_fail_op, front_zfail_op, front_pass_op);
	glStencilOpSeparate(GL_BACK, back_fail_op, back_zfail_op, back_pass_op);
	return true;
}

static inline GLenum get_blend_coeff(int coeff)
{
	switch (coeff)
	{
	case RHI_BLEND_ZERO:
		return GL_ZERO;
	case RHI_BLEND_ONE:
		return GL_ONE;
	case RHI_BLEND_SRC_COLOR:
		return GL_SRC_COLOR;
	case RHI_BLEND_INV_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case RHI_BLEND_DEST_COLOR:
		return GL_DST_COLOR;
	case RHI_BLEND_INV_DEST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case RHI_BLEND_SRC_ALPHA:
		return GL_SRC_ALPHA;
	case RHI_BLEND_INV_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case RHI_BLEND_DEST_ALPHA:
		return GL_DST_ALPHA;
	case RHI_BLEND_INV_DEST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case RHI_BLEND_CONST_COLOR:
		return GL_CONSTANT_COLOR;
	case RHI_BLEND_INV_CONST_COLOR:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case RHI_BLEND_CONST_ALPHA:
		return GL_CONSTANT_ALPHA;
	case RHI_BLEND_INV_CONST_ALPHA:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	case RHI_BLEND_SRC_ALPHA_SAT:
		return GL_SRC_ALPHA_SATURATE;
	default:
		Assert(0);
		break;
	}

	return GL_ZERO;
}

static inline GLenum get_blend_op(int op)
{
	switch (op)
	{
	case RHI_BLEND_OP_ADD:
		return GL_FUNC_ADD;
	case RHI_BLEND_OP_SUBTRACT:
		return GL_FUNC_SUBTRACT;
	case RHI_BLEND_OP_REV_SUBTRACT:
		return GL_FUNC_REVERSE_SUBTRACT;
	case RHI_BLEND_OP_MIN:
		return GL_MIN;
	case RHI_BLEND_OP_MAX:
		return GL_MAX;
	default:
		Assert(0);
		break;
	}

	return GL_FUNC_ADD;
}

bool GLES3Device::SetBlendState(RHIBlendState* pBS, const float* blend_factor,
	unsigned int sample_mask)
{
	Assert(pBS != NULL);
	
	GLES3BlendState* p = (GLES3BlendState*)pBS;
	const rhi_blend_desc_t& desc = p->GetDesc();

	// Alpha to coverage.
	if (desc.bAlphaToCoverage)
	{
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}
	else
	{
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}

	// Blend enable.
	if (desc.Blends[0].bBlendEnable)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	// Blend func.
	GLenum color_src_blend = get_blend_coeff(desc.Blends[0].nSrcBlend);
	GLenum color_dst_blend = get_blend_coeff(desc.Blends[0].nDestBlend);
	GLenum alpha_src_blend = get_blend_coeff(desc.Blends[0].nSrcBlendAlpha);
	GLenum alpha_dst_blend = get_blend_coeff(desc.Blends[0].nDestBlendAlpha);

	glBlendFuncSeparate(color_src_blend, color_dst_blend, alpha_src_blend, 
		alpha_dst_blend);
	
	// Blend op.
	GLenum color_blend_op = get_blend_op(desc.Blends[0].nBlendOp);
	GLenum alpha_blend_op = get_blend_op(desc.Blends[0].nBlendOpAlpha);
	
	glBlendEquationSeparate(color_blend_op, alpha_blend_op);

	// Blend color.
	if (blend_factor)
	{
		glBlendColor(blend_factor[0], blend_factor[1], blend_factor[2],
			blend_factor[3]);
	}

	return true;
}

static inline GLenum get_draw_mode(int primitive_type)
{
	switch (primitive_type)
	{
	case RHI_PRIMITIVE_POINTS:
		return GL_POINTS;
	case RHI_PRIMITIVE_LINE_LIST:
		return GL_LINES;
	case RHI_PRIMITIVE_LINE_STRIP:
		return GL_LINE_STRIP;
	case RHI_PRIMITIVE_TRIANGLE_LIST:
		return GL_TRIANGLES;
	case RHI_PRIMITIVE_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	default:
		Assert(0);
		break;
	}

	return GL_TRIANGLES;
}

static inline GLenum get_index_type(int index_type)
{
	switch (index_type)
	{
	case RHI_INDEX_UNSIGNED_BYTE:
		return GL_UNSIGNED_BYTE;
	case RHI_INDEX_UNSIGNED_SHORT:
		return GL_UNSIGNED_SHORT;
	case RHI_INDEX_UNSIGNED_INT:
		return GL_UNSIGNED_INT;
	default:
		Assert(0);
		break;
	}

	return GL_UNSIGNED_SHORT;
}

bool GLES3Device::Draw(int primitive_type, unsigned int first,
	unsigned int count)
{
	GLenum mode = get_draw_mode(primitive_type);

	glDrawArrays(mode, first, count);
	return true;
}

bool GLES3Device::DrawIndexed(int primitive_type, unsigned int count)
{
	GLenum mode = get_draw_mode(primitive_type);
	GLenum type = get_index_type(m_nCurrentIndexFormat);

//	if (start_index > 0)
//	{
//		glDrawRangeElements(mode, start_index, start_index + count - 1, count,
//			type, NULL);
//	}
//	else
//	{
//		glDrawElements(mode, count, type, NULL);
//	}

	glDrawElements(mode, count, type, 
		(const void*)(size_t)m_nCurrentIndexOffset);
	return true;
}

static bool update_buffer(int target, GLuint buffer, unsigned int offset, 
	const void* data, unsigned int size, unsigned int buffer_size)
{
	Assert(buffer != 0);
	Assert(data != NULL);

	glBindBuffer(target, buffer);

//	// Optimize performance.
//	if ((offset == 0) && (size == size))
//	{
//		// Discard all old data.
//		glBufferData(target, size, NULL, GL_DYNAMIC_DRAW);
//	}

	GLbitfield flags = GL_MAP_WRITE_BIT;

	if ((offset == 0) && (size == buffer_size))
	{
		flags |= GL_MAP_INVALIDATE_BUFFER_BIT;
	}
	else
	{
		flags |= GL_MAP_INVALIDATE_RANGE_BIT;
	}

	void* pBuffer = glMapBufferRange(target, offset, size, flags);

	if (NULL == pBuffer)
	{
		K_LOG("(update_buffer)glMapBufferRange error.");
		return false;
	}

	memcpy(pBuffer, data, size);
	glUnmapBuffer(target);
	glBindBuffer(target, 0);
	return true;
}

bool GLES3Device::UpdateDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB,
	unsigned int offset, const void* data, unsigned int size)
{
	Assert(pVB != NULL);

	GLES3DynamicVertexBuffer* p = (GLES3DynamicVertexBuffer*)pVB;
	GLuint buffer = p->GetBuffer();
	unsigned int buffer_size = p->GetSize();
		
	return update_buffer(GL_ARRAY_BUFFER, buffer, offset, data, size,
		buffer_size);
}

bool GLES3Device::UpdateDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB,
	unsigned int offset, const void* data, unsigned int size)
{
	Assert(pIB != NULL);

	GLES3DynamicIndexBuffer* p = (GLES3DynamicIndexBuffer*)pIB;
	GLuint buffer = p->GetBuffer();
	unsigned int buffer_size = p->GetSize();

	return update_buffer(GL_ELEMENT_ARRAY_BUFFER, buffer, offset, data, size,
		buffer_size);
}

bool GLES3Device::UpdateDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB,
	unsigned int offset, const void* data, unsigned int size)
{
	Assert(pUB != NULL);

	GLES3DynamicUniformBuffer* p = (GLES3DynamicUniformBuffer*)pUB;
	GLuint buffer = p->GetBuffer();
	unsigned int buffer_size = p->GetSize();

	return update_buffer(GL_UNIFORM_BUFFER, buffer, offset, data, size,
		buffer_size);
}

bool GLES3Device::UpdateDynamicTexture(RHIDynamicTexture* pDT, int level,
	const void* data, unsigned int size)
{
	Assert(pDT != NULL);

	GLES3DynamicTexture* p = (GLES3DynamicTexture*)pDT;
	GLuint texture = p->GetTexture();
	GLenum tex_format = p->GetTextureFormat();
	GLenum tex_type = p->GetTextureType();
	int tex_width = p->GetWidth();
	int tex_height = p->GetHeight();

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, tex_width, tex_height,
		tex_format, tex_type, data);
	gles3_check_error_return_false(
		"(GLES3Device::UpdateDynamicTexture)glTexSubImage2D error");
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLES3Device::UpdateDynamicTextureRegion(RHIDynamicTexture* pDT, int level,
	int x_offset, int y_offset, int width, int height, const void* data,
	unsigned int size)
{
	Assert(pDT != NULL);

	GLES3DynamicTexture* p = (GLES3DynamicTexture*)pDT;
	GLuint texture = p->GetTexture();
	GLenum tex_format = p->GetTextureFormat();
	GLenum tex_type = p->GetTextureType();

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, level, x_offset, y_offset, width, height,
		tex_format, tex_type, data);
	gles3_check_error_return_false(
		"(GLES3Device::UpdateDynamicTextureRegion)glTexSubImage2D error");
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

RHIFrameBuffer* GLES3Device::NewFrameBuffer(size_t index, RHIColorBuffer* pCB,
	RHIDepthBuffer* pDB)
{
	GLES3FrameBuffer* p = GLES3FrameBuffer::CreateInstance(m_pRenderService);
	GLES3ColorBuffer* pCBuffer = (GLES3ColorBuffer*)pCB;
	GLES3DepthBuffer* pDBuffer = (GLES3DepthBuffer*)pDB;
	GLuint color_buffer = pCBuffer->GetBuffer();
	GLuint depth_buffer = pDBuffer->GetBuffer();

	if (!p->CreateResource(index, color_buffer, depth_buffer))
	{
		GLES3FrameBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteFrameBuffer(RHIFrameBuffer* pFB)
{
	Assert(pFB != NULL);

	GLES3FrameBuffer* p = (GLES3FrameBuffer*)pFB;

	GLES3FrameBuffer::DeleteInstance(p);
}

RHIColorBuffer* GLES3Device::NewColorBuffer(size_t index, int width, int height,
	int format, int samples)
{
	GLES3ColorBuffer* p = GLES3ColorBuffer::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, width, height, format, samples))
	{
		GLES3ColorBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteColorBuffer(RHIColorBuffer* pCB)
{
	Assert(pCB != NULL);

	GLES3ColorBuffer* p = (GLES3ColorBuffer*)pCB;

	GLES3ColorBuffer::DeleteInstance(p);
}

RHIDepthBuffer* GLES3Device::NewDepthBuffer(size_t index, int width, int height,
	int format, int samples)
{
	GLES3DepthBuffer* p = GLES3DepthBuffer::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, width, height, format, samples))
	{
		GLES3DepthBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDepthBuffer(RHIDepthBuffer* pDB)
{
	Assert(pDB != NULL);

	GLES3DepthBuffer* p = (GLES3DepthBuffer*)pDB;

	GLES3DepthBuffer::DeleteInstance(p);
}

RHIColorTexture* GLES3Device::NewColorTexture(size_t index, int width,
	int height, int format)
{
	GLES3ColorTexture* p = GLES3ColorTexture::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, width, height, format))
	{
		GLES3ColorTexture::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteColorTexture(RHIColorTexture* pCT)
{
	Assert(pCT != NULL);

	GLES3ColorTexture* p = (GLES3ColorTexture*)pCT;

	GLES3ColorTexture::DeleteInstance(p);
}

RHIDepthTexture* GLES3Device::NewDepthTexture(size_t index, int width,
	int height, int format)
{
	GLES3DepthTexture* p = GLES3DepthTexture::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, width, height, format))
	{
		GLES3DepthTexture::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDepthTexture(RHIDepthTexture* pDT)
{
	Assert(pDT != NULL);

	GLES3DepthTexture* p = (GLES3DepthTexture*)pDT;

	GLES3DepthTexture::DeleteInstance(p);
}

RHIVertexShader* GLES3Device::NewVertexShader(size_t index, const void* data,
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	GLES3VertexShader* p = GLES3VertexShader::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, data, size))
	{
		GLES3VertexShader::DeleteInstance(p);
		return NULL;
	}

	return p;
}

RHIVertexShader* GLES3Device::NewVertexShaderFromFile(size_t index,
	const char* file, const char* func, const char* def)
{
	Assert(file != NULL);
	Assert(func != NULL);
	Assert(def != NULL);

	GLES3VertexShader* p = GLES3VertexShader::CreateInstance(m_pRenderService);

	if (!p->CreateFromFile(index, file, func, def))
	{
		GLES3VertexShader::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteVertexShader(RHIVertexShader* pVS)
{
	Assert(pVS != NULL);

	GLES3VertexShader* p = (GLES3VertexShader*)pVS;

	GLES3VertexShader::DeleteInstance(p);
}

RHIPixelShader* GLES3Device::NewPixelShader(size_t index, const void* data,
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	GLES3PixelShader* p = GLES3PixelShader::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, data, size))
	{
		GLES3PixelShader::DeleteInstance(p);
		return NULL;
	}

	return p;
}

RHIPixelShader* GLES3Device::NewPixelShaderFromFile(size_t index,
	const char* file, const char* func, const char* def)
{
	Assert(file != NULL);
	Assert(func != NULL);
	Assert(def != NULL);

	GLES3PixelShader* p = GLES3PixelShader::CreateInstance(m_pRenderService);

	if (!p->CreateFromFile(index, file, func, def))
	{
		GLES3PixelShader::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeletePixelShader(RHIPixelShader* pPS)
{
	Assert(pPS != NULL);

	GLES3PixelShader* p = (GLES3PixelShader*)pPS;

	GLES3PixelShader::DeleteInstance(p);
}

RHIShaderProgram* GLES3Device::NewShaderProgram(size_t index,
	RHIVertexShader* pVS, RHIPixelShader* pPS)
{
	Assert(pVS != NULL);
	Assert(pPS != NULL);

	GLES3VertexShader* vs = (GLES3VertexShader*)pVS;
	GLES3PixelShader* ps = (GLES3PixelShader*)pPS;
	GLES3ShaderProgram* p = GLES3ShaderProgram::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, vs->GetShader(), ps->GetShader()))
	{
		GLES3ShaderProgram::DeleteInstance(p);
		return NULL;
	}

	return p;
}

RHIShaderProgram* GLES3Device::NewShaderProgramEmpty(size_t index)
{
	GLES3ShaderProgram* p = GLES3ShaderProgram::CreateInstance(m_pRenderService);

	if (!p->CreateEmpty(index))
	{
		GLES3ShaderProgram::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteShaderProgram(RHIShaderProgram* pProgram)
{
	Assert(pProgram != NULL);

	GLES3ShaderProgram* p = (GLES3ShaderProgram*)pProgram;

	GLES3ShaderProgram::DeleteInstance(p);
}

RHIVertexDescriptor* GLES3Device::NewVertexDescriptor(size_t index,
	const rhi_vertex_attrib_t* pAttribs, int attrib_num,
	unsigned int stride, RHIShaderProgram* pSP)
{
	GLES3VertexDescriptor* p = GLES3VertexDescriptor::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, pAttribs, attrib_num, stride))
	{
		GLES3VertexDescriptor::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteVertexDescriptor(RHIVertexDescriptor* pVD)
{
	Assert(pVD != NULL);

	GLES3VertexDescriptor* p = (GLES3VertexDescriptor*)pVD;

	GLES3VertexDescriptor::DeleteInstance(p);
}

RHIStaticVertexBuffer* GLES3Device::NewStaticVertexBuffer(size_t index,
	const void* data, unsigned int size, unsigned int stride)
{
	GLES3StaticVertexBuffer* p = GLES3StaticVertexBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, data, size, stride))
	{
		GLES3StaticVertexBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteStaticVertexBuffer(RHIStaticVertexBuffer* pVB)
{
	Assert(pVB != NULL);

	GLES3StaticVertexBuffer* p = (GLES3StaticVertexBuffer*)pVB;

	GLES3StaticVertexBuffer::DeleteInstance(p);
}

RHIStaticIndexBuffer* GLES3Device::NewStaticIndexBuffer(size_t index,
	const void* data, unsigned int size)
{
	GLES3StaticIndexBuffer* p = GLES3StaticIndexBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, data, size))
	{
		GLES3StaticIndexBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteStaticIndexBuffer(RHIStaticIndexBuffer* pIB)
{
	Assert(pIB != NULL);

	GLES3StaticIndexBuffer* p = (GLES3StaticIndexBuffer*)pIB;

	GLES3StaticIndexBuffer::DeleteInstance(p);
}

RHIStaticUniformBuffer* GLES3Device::NewStaticUniformBuffer(size_t index,
	const void* data, unsigned int size)
{
	GLES3StaticUniformBuffer* p = GLES3StaticUniformBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, data, size))
	{
		GLES3StaticUniformBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteStaticUniformBuffer(RHIStaticUniformBuffer* pUB)
{
	Assert(pUB != NULL);

	GLES3StaticUniformBuffer* p = (GLES3StaticUniformBuffer*)pUB;

	GLES3StaticUniformBuffer::DeleteInstance(p);
}

RHIStaticTexture* GLES3Device::NewStaticTexture(size_t index, int width,
	int height, int format, int levels, const void* data, size_t size)
{
	GLES3StaticTexture* p = GLES3StaticTexture::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, width, height, format, levels, data, size))
	{
		GLES3StaticTexture::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteStaticTexture(RHIStaticTexture* pST)
{
	Assert(pST != NULL);

	GLES3StaticTexture* p = (GLES3StaticTexture*)pST;

	GLES3StaticTexture::DeleteInstance(p);
}

RHIDynamicVertexBuffer* GLES3Device::NewDynamicVertexBuffer(size_t index,
	unsigned int size, unsigned int stride)
{
	GLES3DynamicVertexBuffer* p = GLES3DynamicVertexBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, size, stride))
	{
		GLES3DynamicVertexBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB)
{
	Assert(pVB != NULL);

	GLES3DynamicVertexBuffer* p = (GLES3DynamicVertexBuffer*)pVB;

	GLES3DynamicVertexBuffer::DeleteInstance(p);
}

RHIDynamicIndexBuffer* GLES3Device::NewDynamicIndexBuffer(size_t index,
	unsigned int size)
{
	GLES3DynamicIndexBuffer* p = GLES3DynamicIndexBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, size))
	{
		GLES3DynamicIndexBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB)
{
	Assert(pIB != NULL);

	GLES3DynamicIndexBuffer* p = (GLES3DynamicIndexBuffer*)pIB;

	GLES3DynamicIndexBuffer::DeleteInstance(p);
}

RHIDynamicUniformBuffer* GLES3Device::NewDynamicUniformBuffer(size_t index,
	unsigned int size)
{
	GLES3DynamicUniformBuffer* p = GLES3DynamicUniformBuffer::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, size))
	{
		GLES3DynamicUniformBuffer::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB)
{
	Assert(pUB != NULL);

	GLES3DynamicUniformBuffer* p = (GLES3DynamicUniformBuffer*)pUB;

	GLES3DynamicUniformBuffer::DeleteInstance(p);
}

RHIDynamicTexture* GLES3Device::NewDynamicTexture(size_t index, int width, 
	int height, int format, int levels)
{
	GLES3DynamicTexture* p = GLES3DynamicTexture::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, width, height, format, levels))
	{
		GLES3DynamicTexture::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDynamicTexture(RHIDynamicTexture* pDT)
{
	Assert(pDT != NULL);

	GLES3DynamicTexture* p = (GLES3DynamicTexture*)pDT;

	GLES3DynamicTexture::DeleteInstance(p);
}

RHISamplerState* GLES3Device::NewSamplerState(size_t index,
	const rhi_sampler_desc_t& desc)
{
	GLES3SamplerState* p = GLES3SamplerState::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, desc))
	{
		GLES3SamplerState::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteSamplerState(RHISamplerState* pSS)
{
	Assert(pSS != NULL);

	GLES3SamplerState* p = (GLES3SamplerState*)pSS;

	GLES3SamplerState::DeleteInstance(p);
}

RHIRasterizeState* GLES3Device::NewRasterizeState(size_t index,
	const rhi_rasterize_desc_t& desc)
{
	GLES3RasterizeState* p = GLES3RasterizeState::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, desc))
	{
		GLES3RasterizeState::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteRasterizeState(RHIRasterizeState* pRS)
{
	Assert(pRS != NULL);

	GLES3RasterizeState* p = (GLES3RasterizeState*)pRS;

	GLES3RasterizeState::DeleteInstance(p);
}

RHIDepthStencilState* GLES3Device::NewDepthStencilState(size_t index,
	const rhi_depth_stencil_desc_t& desc)
{
	GLES3DepthStencilState* p = GLES3DepthStencilState::CreateInstance(
		m_pRenderService);

	if (!p->CreateResource(index, desc))
	{
		GLES3DepthStencilState::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteDepthStencilState(RHIDepthStencilState* pDSS)
{
	Assert(pDSS != NULL);

	GLES3DepthStencilState* p = (GLES3DepthStencilState*)pDSS;

	GLES3DepthStencilState::DeleteInstance(p);
}

RHIBlendState* GLES3Device::NewBlendState(size_t index,
	const rhi_blend_desc_t& desc)
{
	GLES3BlendState* p = GLES3BlendState::CreateInstance(m_pRenderService);

	if (!p->CreateResource(index, desc))
	{
		GLES3BlendState::DeleteInstance(p);
		return NULL;
	}

	return p;
}

void GLES3Device::DeleteBlendState(RHIBlendState* pBS)
{
	Assert(pBS != NULL);

	GLES3BlendState* p = (GLES3BlendState*)pBS;

	GLES3BlendState::DeleteInstance(p);
}

RHIShaderTexture* GLES3Device::NewShaderTexture(int width, int height,
	int format, int levels, const void* data, size_t size)
{
	GLES3ShaderTexture* p = GLES3ShaderTexture::CreateInstance();

	if (!p->CreateResource(width, height, format, levels, data, size))
	{
		p->Release();
		return NULL;
	}

	return p;
}
