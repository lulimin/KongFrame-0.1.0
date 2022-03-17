// rhi_resource.h
// Created by lulimin on 2020/10/15.

#ifndef __RHI_RESOURCE_H
#define __RHI_RESOURCE_H

#include "../inc/common.h"

// Resource types.
#define RHI_RESOURCE_FRAME_BUFFER 1
#define RHI_RESOURCE_COLOR_BUFFER 2
#define RHI_RESOURCE_DEPTH_BUFFER 3
#define RHI_RESOURCE_COLOR_TEXTURE 4
#define RHI_RESOURCE_DEPTH_TEXTURE 5
#define RHI_RESOURCE_VERTEX_SHADER 6
#define RHI_RESOURCE_PIXEL_SHADER 7
#define RHI_RESOURCE_SHADER_PROGRAM 8
#define RHI_RESOURCE_VERTEX_DESCRIPTOR 9
#define RHI_RESOURCE_STATIC_VERTEX_BUFFER 10
#define RHI_RESOURCE_STATIC_INDEX_BUFFER 11
#define RHI_RESOURCE_STATIC_UNIFORM_BUFFER 12
#define RHI_RESOURCE_STATIC_TEXTURE 13
#define RHI_RESOURCE_DYNAMIC_VERTEX_BUFFER 14
#define RHI_RESOURCE_DYNAMIC_INDEX_BUFFER 15
#define RHI_RESOURCE_DYNAMIC_UNIFORM_BUFFER 16
#define RHI_RESOURCE_DYNAMIC_TEXTURE 17
#define RHI_RESOURCE_SAMPLER_STATE 18
#define RHI_RESOURCE_RASTERIZE_STATE 19
#define RHI_RESOURCE_DEPTH_STENCIL_STATE 20
#define RHI_RESOURCE_BLEND_STATE 21

// RHIResource

class RHIResource
{
public:
	virtual ~RHIResource() {}

	// Release reference.
	virtual void Release() = 0;
	// Get type of resource.
	virtual int GetResourceType() = 0;
	// Get resource index.
	virtual size_t GetIndex() = 0;
};

// Image formats.
// Integer type format.
#define RHI_FORMAT_R8 1
#define RHI_FORMAT_R8G8 2
#define RHI_FORMAT_R8G8B8 3
#define RHI_FORMAT_R8G8B8A8 4
#define RHI_FORMAT_R8G8B8_SRGB 5
#define RHI_FORMAT_R8G8B8_SRGB_A8 6
#define RHI_FORMAT_R4G4B4A4 7
#define RHI_FORMAT_R5G6B5 8
#define RHI_FORMAT_R5G5B5A1 9
#define RHI_FORMAT_R10G10B10A2 10
#define RHI_FORMAT_R9G9B9E5 11
#define RHI_FORMAT_L8 12
#define RHI_FORMAT_L8_A8 13
#define RHI_FORMAT_A8 14
// Float type format.
#define RHI_FORMAT_R16_FLOAT 16
#define RHI_FORMAT_R16G16_FLOAT 17
#define RHI_FORMAT_R16G16B16_FLOAT 18
#define RHI_FORMAT_R16G16B16A16_FLOAT 19
#define RHI_FORMAT_R32_FLOAT 20
#define RHI_FORMAT_R32G32_FLOAT 21
#define RHI_FORMAT_R11G11B10_FLOAT 22
// Depth & stencil format.
#define RHI_FORMAT_D16 32
#define RHI_FORMAT_D32 33
#define RHI_FORMAT_D24S8 34
#define RHI_FORMAT_X24S8 35
// GL compressed format.
#define RHI_FORMAT_R11_EAC 48
#define RHI_FORMAT_R11G11_EAC 49
#define RHI_FORMAT_R8G8B8_ETC2 50
#define RHI_FORMAT_R8G8B8_SRGB_ETC2 51
#define RHI_FORMAT_R8G8B8_A1_ETC2 52
#define RHI_FORMAT_R8G8B8_SRGB_A1_ETC2 53
#define RHI_FORMAT_R8G8B8A8_ETC2_EAC 54
#define RHI_FORMAT_R8G8B8A8_SRGB_ETC2_EAC 55
// DirectX compressed format.
#define RHI_FORMAT_BC1 64
#define RHI_FORMAT_BC2 65
#define RHI_FORMAT_BC3 66
#define RHI_FORMAT_BC4 67
#define RHI_FORMAT_BC5 68
#define RHI_FORMAT_BC6 69
#define RHI_FORMAT_BC7 70

// RHIFrameBuffer

class RHIFrameBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_FRAME_BUFFER; }
};

// RHIColorBuffer

class RHIColorBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_COLOR_BUFFER; }
};

// RHIDepthBuffer

class RHIDepthBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DEPTH_BUFFER; }
};

// RHIColorTexture

class RHIColorTexture : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_COLOR_TEXTURE; }
};

// RHIDepthTexture

class RHIDepthTexture : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DEPTH_TEXTURE; }
};

// RHIVertexShader

class RHIVertexShader : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_VERTEX_SHADER; }
};

// RHIPixelShader

class RHIPixelShader : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_PIXEL_SHADER; }
};

// RHIShaderProgram

class RHIShaderProgram : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_SHADER_PROGRAM; }
	// Increase reference count.
	virtual int IncRefCount() = 0;
	// Set uniform block name.
	virtual bool SetUniformBlockName(int slot, const char* name) = 0;
	// Set uniform name of texture sampler.
	virtual bool SetSamplerUniformName(int slot, const char* name) = 0;
	// Save shader binary to file.
	virtual bool SaveBinary(void* fp) = 0;
	// Load shader binary from file.
	virtual bool LoadBinary(void* fp) = 0;
};

// Maximum vertex attributes.
#define RHI_VERTEX_ATTRIB_MAX 16

// Type of vertex attribute.
#define RHI_VERTEX_DATA_BYTE 1
#define RHI_VERTEX_DATA_UNSIGNED_BYTE 2
#define RHI_VERTEX_DATA_SHORT 3
#define RHI_VERTEX_DATA_UNSIGNED_SHORT 4
#define RHI_VERTEX_DATA_INT 5
#define RHI_VERTEX_DATA_UNSIGNED_INT 6
#define RHI_VERTEX_DATA_HALF_FLOAT 7
#define RHI_VERTEX_DATA_FLOAT 8

// Vertex attribute.
struct rhi_vertex_attrib_t
{
	const char* pSemanticName;
	unsigned int nSemanticIndex;
	int nType;
	int nSize;
	int nInputSlot;
	int nOffset;
	bool bNormalized;
};

// RHIVertexDescriptor

class RHIVertexDescriptor : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_VERTEX_DESCRIPTOR; }
};

// RHIVertexBuffer

class RHIVertexBuffer : public RHIResource
{
public:
	// Get stride of vertex data.
	virtual unsigned int GetStride() = 0;
	// Offset bytes.
	virtual void SetOffset(unsigned int offset) = 0;
	virtual unsigned int GetOffset() = 0;
};

// RHIStaticVertexBuffer

class RHIStaticVertexBuffer : public RHIVertexBuffer
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_STATIC_VERTEX_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
};

// RHIStaticIndexBuffer

class RHIStaticIndexBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_STATIC_INDEX_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
};

// RHIStaticUniformBuffer

class RHIStaticUniformBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_STATIC_UNIFORM_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
};

// RHIStaticTexture

class RHIStaticTexture : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_STATIC_TEXTURE; }
};

// RHIDynamicVertexBuffer

class RHIDynamicVertexBuffer : public RHIVertexBuffer
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DYNAMIC_VERTEX_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
	// Set stride of vertex data.
	virtual void SetStride(unsigned int stride) = 0;
};

// RHIDynamicIndexBuffer

class RHIDynamicIndexBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DYNAMIC_INDEX_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
};

// RHIDynamicUniformBuffer

class RHIDynamicUniformBuffer : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DYNAMIC_UNIFORM_BUFFER; }
	// Get buffer size.
	virtual unsigned int GetBufferSize() = 0;
};

// RHIDynamicTexture

class RHIDynamicTexture : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DYNAMIC_TEXTURE; }
};

// Texture filter types.
#define RHI_TEXTURE_FILTER_NEAREST 0
#define RHI_TEXTURE_FILTER_LINEAR 1
#define RHI_TEXTURE_FILTER_ANISOTROPIC 2

// Texture address modes.
#define RHI_TEXTURE_ADDRESS_WRAP 0
#define RHI_TEXTURE_ADDRESS_MIRROR 1
#define RHI_TEXTURE_ADDRESS_CLAMP 2

// Texture comparison functions.
#define RHI_TEXTURE_COMPARISON_NEVER 0
#define RHI_TEXTURE_COMPARISON_LESS 1
#define RHI_TEXTURE_COMPARISON_EQUAL 2
#define RHI_TEXTURE_COMPARISON_LESS_EQUAL 3
#define RHI_TEXTURE_COMPARISON_GREATER 4
#define RHI_TEXTURE_COMPARISON_NOT_EQUAL 5
#define RHI_TEXTURE_COMPARISON_GREATER_EQUAL 6
#define RHI_TEXTURE_COMPARISON_ALWAYS 7

// Sampler description.
struct rhi_sampler_desc_t
{
	int nMinFilter;
	int nMagFilter;
	int nMipFilter;
	int nAddressU;
	int nAddressV;
	int nAddressW;
	float fMinLOD;
	float fMaxLOD;
	int nCompareMode;
	int nCompareFunc;
};

// RHISamplerState

class RHISamplerState : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_SAMPLER_STATE; }
};

// Triangle fill modes.
#define RHI_FILL_WIREFRAME 0
#define RHI_FILL_SOLID 1

// Triangle cull modes.
#define RHI_CULL_NONE 0
#define RHI_CULL_FRONT 1
#define RHI_CULL_BACK 2

// Rasterize description.
struct rhi_rasterize_desc_t
{
	int nFillMode;
	int nCullMode;
	bool bFrontCounterClockwise;
	int nDepthBias;
	float fDepthBiasClamp;
	float fSlopeScaledDepthBias;
	bool bDepthClipEnable;
	bool bScissorEnable;
	bool bMultisampleEnable;
	bool bAntialiasedLineEnable;
};

// RHIRasterizeState

class RHIRasterizeState : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_RASTERIZE_STATE; }
};

// Depth buffer comparison.
#define RHI_COMPARISON_NEVER 0
#define RHI_COMPARISON_LESS 1
#define RHI_COMPARISON_EQUAL 2
#define RHI_COMPARISON_LESS_EQUAL 3
#define RHI_COMPARISON_GREATER 4
#define RHI_COMPARISON_NOT_EQUAL 5
#define RHI_COMPARISON_GREATER_EQUAL 6
#define RHI_COMPARISON_ALWAYS 7

// Stencil buffer operations.
#define RHI_STENCIL_OP_KEEP 0
#define RHI_STENCIL_OP_ZERO 1
#define RHI_STENCIL_OP_REPLACE 2
#define RHI_STENCIL_OP_INCR_SAT 3
#define RHI_STENCIL_OP_DECR_SAT 4
#define RHI_STENCIL_OP_INVERT 5
#define RHI_STENCIL_OP_INCR 6
#define RHI_STENCIL_OP_DECR 7

// Depth & stencil description.
struct rhi_depth_stencil_desc_t
{
	bool bDepthEnable;
	unsigned int nDepthWriteMask; // 0 or 1
	int nDepthFunc;
	bool bStencilEnable;
	unsigned char nStencilReadMask;
	unsigned char nStencilWriteMask;
	int nFrontStencilFailOp;
	int nFrontStencilDepthFailOp;
	int nFrontStencilPassOp;
	int nFrontStencilFunc;
	int nBackStencilFailOp;
	int nBackStencilDepthFailOp;
	int nBackStencilPassOp;
	int nBackStencilFunc;
};

// RHIDepthStencilState

class RHIDepthStencilState : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_DEPTH_STENCIL_STATE; }
};

// Blend operands.
#define RHI_BLEND_ZERO 0
#define RHI_BLEND_ONE 1
#define RHI_BLEND_SRC_COLOR 2
#define RHI_BLEND_INV_SRC_COLOR 3
#define RHI_BLEND_DEST_COLOR 4
#define RHI_BLEND_INV_DEST_COLOR 5
#define RHI_BLEND_SRC_ALPHA 6
#define RHI_BLEND_INV_SRC_ALPHA 7
#define RHI_BLEND_DEST_ALPHA 8
#define RHI_BLEND_INV_DEST_ALPHA 9
#define RHI_BLEND_CONST_COLOR 10
#define RHI_BLEND_INV_CONST_COLOR 11
#define RHI_BLEND_CONST_ALPHA 12
#define RHI_BLEND_INV_CONST_ALPHA 13
#define RHI_BLEND_SRC_ALPHA_SAT 14

// Blend operators.
#define RHI_BLEND_OP_ADD 0
#define RHI_BLEND_OP_SUBTRACT 1
#define RHI_BLEND_OP_REV_SUBTRACT 2
#define RHI_BLEND_OP_MIN 3
#define RHI_BLEND_OP_MAX 4

// Blend state of attachment.
struct rhi_blend_t
{
	bool bBlendEnable;
	int nSrcBlend;
	int nDestBlend;
	int nBlendOp;
	int nSrcBlendAlpha;
	int nDestBlendAlpha;
	int nBlendOpAlpha;
	unsigned int nWriteMask;
};

// Blend description.
struct rhi_blend_desc_t
{
	bool bAlphaToCoverage;
	bool bIndependentBlend;
	rhi_blend_t Blends[4];
};

// RHIBlendState

class RHIBlendState : public RHIResource
{
public:
	// Get type of resource.
	virtual int GetResourceType() { return RHI_RESOURCE_BLEND_STATE;  }
};

#endif // __RHI_RESOURCE_H
