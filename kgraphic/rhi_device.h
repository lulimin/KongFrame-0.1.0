// rhi_device.h
// Created by lulimin on 2020/10/16.

#ifndef __RHI_DEVICE_H
#define __RHI_DEVICE_H

#include "../inc/common.h"
#include "rhi_resource.h"
#include "rhi_shader_texture.h"

// Render hardware interface api type.
#define RHI_API_GLES3 1
#define RHI_API_DX11 2
#define RHI_API_DX12 3
#define RHI_API_METAL 4
#define RHI_API_VULKAN 5

// Shader language types.
#define RHI_SHADER_GLSL 1
#define RHI_SHADER_HLSL 2
#define RHI_SHADER_METAL 3

// Primitive types.
#define RHI_PRIMITIVE_POINTS 1
#define RHI_PRIMITIVE_LINE_LIST 2
#define RHI_PRIMITIVE_LINE_STRIP 3
#define RHI_PRIMITIVE_TRIANGLE_LIST 4
#define RHI_PRIMITIVE_TRIANGLE_STRIP 5

// Index data types.
#define RHI_INDEX_UNSIGNED_BYTE 1
#define RHI_INDEX_UNSIGNED_SHORT 2
#define RHI_INDEX_UNSIGNED_INT 3

// RHIDevice

class RHIDevice
{
public:
	virtual ~RHIDevice() {}

	// Delete self.
	virtual void Release() = 0;

	// Get render hardware interface api type.
	virtual int GetApiType() = 0;
	// Get shader language type.
	virtual int GetShaderType() = 0;
	// Get shader file extension.
	virtual const char* GetVertexShaderFileExt() = 0;
	virtual const char* GetPixelShaderFileExt() = 0;
	// Can save shader binary.
	virtual bool GetSaveShader() = 0;

	// Initializ device.
	virtual bool Startup() = 0;
	// Shutdown device.
	virtual bool Cleanup() = 0;

	// Set frame back buffer.
	virtual bool SetFrameBuffer(RHIFrameBuffer* pFB) = 0;
	// Clear back buffer.
	virtual bool ClearFrame(float color_r, float color_g, float color_b,
		float color_a, float depth, int stencil) = 0;
	// Set viewport of screen.
	virtual bool SetViewport(int left, int top, int width, int height) = 0;
	// Set scissor test rectangle.
	virtual bool SetScissorRect(int left, int top, int width, int height) = 0;
	// Set pipeline states.
	virtual bool SetShaderProgram(RHIShaderProgram* pSP) = 0;
	virtual bool SetVertexArray(int vertex_buffer_num, RHIVertexBuffer** vbs,
		RHIVertexDescriptor* pVD) = 0;
	virtual bool SetStaticIndexBuffer(RHIStaticIndexBuffer* pIB, int format,
		unsigned int offset) = 0;
	virtual bool SetDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB, int format,
		unsigned int offset) = 0;
	virtual bool SetStaticUniformBuffer(unsigned int slot,
		RHIStaticUniformBuffer* pUB) = 0;
	virtual bool SetDynamicUniformBuffer(unsigned int slot,
		RHIDynamicUniformBuffer* pUB) = 0;
	virtual bool SetStaticTexture(unsigned int slot, 
		RHIStaticTexture* pST) = 0;
	virtual bool SetDynamicTexture(unsigned int slot, 
		RHIDynamicTexture* pDT) = 0;
	virtual bool SetShaderTexture(unsigned int slot,
		RHIShaderTexture* pST) = 0;
	virtual bool SetColorTexture(unsigned int slot, RHIColorTexture* pCT) = 0;
	virtual bool SetDepthTexture(unsigned int slot, RHIDepthTexture* pDT) = 0;
	virtual bool SetSamplerState(unsigned int slot, RHISamplerState* pSS) = 0;
	virtual bool SetRasterizeState(RHIRasterizeState* pRS) = 0;
	virtual bool SetDepthStencilState(RHIDepthStencilState* pDSS,
		unsigned int stencil_ref) = 0;
	virtual bool SetBlendState(RHIBlendState* pBS, const float* blend_factor,
		unsigned int sample_mask) = 0;
	// Draw primitives.
	virtual bool Draw(int primitive_type, unsigned int first, 
		unsigned int count) = 0;
	virtual bool DrawIndexed(int primitive_type, unsigned int count) = 0;

	// Udpate dynamic buffer.
	virtual bool UpdateDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB,
		unsigned int offset, const void* data, unsigned int size) = 0;
	virtual bool UpdateDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB,
		unsigned int offset, const void* data, unsigned int size) = 0;
	virtual bool UpdateDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB,
		unsigned int offset, const void* data, unsigned int size) = 0;
	// Update dynamic texture.
	virtual bool UpdateDynamicTexture(RHIDynamicTexture* pDT, int level,
		const void* data, unsigned int size) = 0;
	virtual bool UpdateDynamicTextureRegion(RHIDynamicTexture* pDT, int level,
		int x_offset, int y_offset, int width, int height, const void* data,
		unsigned int size) = 0;

	// Create and destroy render resources, call in render only.
	virtual RHIFrameBuffer* NewFrameBuffer(size_t index, RHIColorBuffer* pCB,
		RHIDepthBuffer* pDB) = 0;
	virtual void DeleteFrameBuffer(RHIFrameBuffer* pFB) = 0;
	virtual RHIColorBuffer* NewColorBuffer(size_t index, int width, int height,
		int format, int samples) = 0;
	virtual void DeleteColorBuffer(RHIColorBuffer* pCB) = 0;
	virtual RHIDepthBuffer* NewDepthBuffer(size_t index, int width, int height,
		int format, int samples) = 0;
	virtual void DeleteDepthBuffer(RHIDepthBuffer* pDB) = 0;
	virtual RHIColorTexture* NewColorTexture(size_t index, int width,
		int height, int format) = 0;
	virtual void DeleteColorTexture(RHIColorTexture* pCT) = 0;
	virtual RHIDepthTexture* NewDepthTexture(size_t index, int width,
		int height, int format) = 0;
	virtual void DeleteDepthTexture(RHIDepthTexture* pDT) = 0;
	virtual RHIVertexShader* NewVertexShader(size_t index, const void* data,
		unsigned int size) = 0;
	virtual RHIVertexShader* NewVertexShaderFromFile(size_t index,
		const char* file, const char* func, const char* def) = 0;
	virtual void DeleteVertexShader(RHIVertexShader* pVS) = 0;
	virtual RHIPixelShader* NewPixelShader(size_t index, const void* data,
		unsigned int size) = 0;
	virtual RHIPixelShader* NewPixelShaderFromFile(size_t index,
		const char* file, const char* func, const char* def) = 0;
	virtual void DeletePixelShader(RHIPixelShader* pPS) = 0;
	virtual RHIShaderProgram* NewShaderProgram(size_t index,
		RHIVertexShader* pVS, RHIPixelShader* pPS) = 0;
	virtual RHIShaderProgram* NewShaderProgramEmpty(size_t index) = 0;
	virtual void DeleteShaderProgram(RHIShaderProgram* pProgram) = 0;
	virtual RHIVertexDescriptor* NewVertexDescriptor(size_t index,
		const rhi_vertex_attrib_t* pAttribs, int attrib_num, 
		unsigned int stride, RHIShaderProgram* pSP) = 0;
	virtual void DeleteVertexDescriptor(RHIVertexDescriptor* pVD) = 0;
	virtual RHIStaticVertexBuffer* NewStaticVertexBuffer(size_t index,
		const void* data, unsigned int size, unsigned int stride) = 0;
	virtual void DeleteStaticVertexBuffer(RHIStaticVertexBuffer* pVB) = 0;
	virtual RHIStaticIndexBuffer* NewStaticIndexBuffer(size_t index,
		const void* data, unsigned int size) = 0;
	virtual void DeleteStaticIndexBuffer(RHIStaticIndexBuffer* pIB) = 0;
	virtual RHIStaticUniformBuffer* NewStaticUniformBuffer(size_t index,
		const void* data, unsigned int size) = 0;
	virtual void DeleteStaticUniformBuffer(RHIStaticUniformBuffer* pUB) = 0;
	virtual RHIStaticTexture* NewStaticTexture(size_t index, int width,
		int height, int format, int levels, const void* data, size_t size) = 0;
	virtual void DeleteStaticTexture(RHIStaticTexture* pST) = 0;
	virtual RHIDynamicVertexBuffer* NewDynamicVertexBuffer(size_t index,
		unsigned int size, unsigned int stride) = 0;
	virtual void DeleteDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB) = 0;
	virtual RHIDynamicIndexBuffer* NewDynamicIndexBuffer(size_t index,
		unsigned int size) = 0;
	virtual void DeleteDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB) = 0;
	virtual RHIDynamicUniformBuffer* NewDynamicUniformBuffer(size_t index,
		unsigned int size) = 0;
	virtual void DeleteDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB) = 0;
	virtual RHIDynamicTexture* NewDynamicTexture(size_t index, int width,
		int height, int format, int levels) = 0;
	virtual void DeleteDynamicTexture(RHIDynamicTexture* pDT) = 0;
	virtual RHISamplerState* NewSamplerState(size_t index,
		const rhi_sampler_desc_t& desc) = 0;
	virtual void DeleteSamplerState(RHISamplerState* pSS) = 0;
	virtual RHIRasterizeState* NewRasterizeState(size_t index,
		const rhi_rasterize_desc_t& desc) = 0;
	virtual void DeleteRasterizeState(RHIRasterizeState* pRS) = 0;
	virtual RHIDepthStencilState* NewDepthStencilState(size_t index,
		const rhi_depth_stencil_desc_t& desc) = 0;
	virtual void DeleteDepthStencilState(RHIDepthStencilState* pDSS) = 0;
	virtual RHIBlendState* NewBlendState(size_t index,
		const rhi_blend_desc_t& desc) = 0;
	virtual void DeleteBlendState(RHIBlendState* pBS) = 0;
	virtual RHIShaderTexture* NewShaderTexture(int width, int height,
		int format, int levels, const void* data, size_t size) = 0;
};

#endif // __RHI_DEVICE_H
