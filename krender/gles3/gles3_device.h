// gles3_device.h
// Created by lulimin on 2020/10/16.

#ifndef __GLES3_DEVICE_H
#define __GLES3_DEVICE_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_device.h"
#include "gles3_header.h"

// GLES3Device

class RenderService;
class GLES3ShaderProgram;
class GLES3RasterizeState;
class GLES3DepthStencilState;
class GLES3BlendState;

class GLES3Device : public RHIDevice
{
public:
	GLES3Device(RenderService* pRS);
	virtual ~GLES3Device();

	// Delete self.
	virtual void Release();

	// Get render hardware interface API type.
	virtual int GetApiType() { return RHI_API_GLES3; }
	// Get shader language type.
	virtual int GetShaderType() { return RHI_SHADER_GLSL; }
	// Get shader file extension.
	virtual const char* GetVertexShaderFileExt() { return "vert"; }
	virtual const char* GetPixelShaderFileExt() { return "frag"; }
	// Can save shader binary.
	virtual bool GetSaveShader() { return m_bSaveShader; }

	// Initializ device.
	virtual bool Startup();
	// Shutdown device.
	virtual bool Cleanup();

	// Set frame back buffer.
	virtual bool SetFrameBuffer(RHIFrameBuffer* pFB);
	// Clear frame buffer.
	virtual bool ClearFrame(float color_r, float color_g, float color_b,
		float color_a, float depth, int stencil);
	// Set viewport of screen.
	virtual bool SetViewport(int left, int top, int width, int height);
	// Set scissor test rectangle.
	virtual bool SetScissorRect(int left, int top, int width, int height);
	// Set pipeline states.
	virtual bool SetShaderProgram(RHIShaderProgram* pSP);
	virtual bool SetVertexArray(int vertex_buffer_num, RHIVertexBuffer** vbs,
		RHIVertexDescriptor* pVD);
	virtual bool SetStaticIndexBuffer(RHIStaticIndexBuffer* pIB, int format,
		unsigned int offset);
	virtual bool SetDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB, int format,
		unsigned int offset);
	virtual bool SetStaticUniformBuffer(unsigned int slot,
		RHIStaticUniformBuffer* pUB);
	virtual bool SetDynamicUniformBuffer(unsigned int slot,
		RHIDynamicUniformBuffer* pUB);
	virtual bool SetStaticTexture(unsigned int slot, RHIStaticTexture* pST);
	virtual bool SetDynamicTexture(unsigned int slot, RHIDynamicTexture* pDT);
	virtual bool SetShaderTexture(unsigned int slot, RHIShaderTexture* pST);
	virtual bool SetColorTexture(unsigned int slot, RHIColorTexture* pCT);
	virtual bool SetDepthTexture(unsigned int slot, RHIDepthTexture* pDT);
	virtual bool SetSamplerState(unsigned int slot, RHISamplerState* pSS);
	virtual bool SetRasterizeState(RHIRasterizeState* pRS);
	virtual bool SetDepthStencilState(RHIDepthStencilState* pDSS, 
		unsigned int stencil_ref);
	virtual bool SetBlendState(RHIBlendState* pBS, const float* blend_factor,
		unsigned int sample_mask);
	// Draw primitives.
	virtual bool Draw(int primitive_type, unsigned int first,
		unsigned int count);
	virtual bool DrawIndexed(int primitive_type, unsigned int count);

	// Udpate dynamic buffer.
	virtual bool UpdateDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB,
		unsigned int offset, const void* data, unsigned int size);
	virtual bool UpdateDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB,
		unsigned int offset, const void* data, unsigned int size);
	virtual bool UpdateDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB,
		unsigned int offset, const void* data, unsigned int size);
	// Update dynamic texture.
	virtual bool UpdateDynamicTexture(RHIDynamicTexture* pDT, int level,
		const void* data, unsigned int size);
	virtual bool UpdateDynamicTextureRegion(RHIDynamicTexture* pDT, int level,
		int x_offset, int y_offset, int width, int height, const void* data,
		unsigned int size);

	// Create and delete render resources, call in render only.
	virtual RHIFrameBuffer* NewFrameBuffer(size_t index, RHIColorBuffer* pCB,
		RHIDepthBuffer* pDB);
	virtual void DeleteFrameBuffer(RHIFrameBuffer* pFB);
	virtual RHIColorBuffer* NewColorBuffer(size_t index, int width, int height,
		int format, int samples);
	virtual void DeleteColorBuffer(RHIColorBuffer* pCB);
	virtual RHIDepthBuffer* NewDepthBuffer(size_t index, int width, int height,
		int format, int samples);
	virtual void DeleteDepthBuffer(RHIDepthBuffer* pDB);
	virtual RHIColorTexture* NewColorTexture(size_t index, int width,
		int height, int format);
	virtual void DeleteColorTexture(RHIColorTexture* pCT);
	virtual RHIDepthTexture* NewDepthTexture(size_t index, int width,
		int height, int format);
	virtual void DeleteDepthTexture(RHIDepthTexture* pDT);
	virtual RHIVertexShader* NewVertexShader(size_t index, const void* data,
		unsigned int size);
	virtual RHIVertexShader* NewVertexShaderFromFile(size_t index,
		const char* file, const char* func, const char* def);
	virtual void DeleteVertexShader(RHIVertexShader* pVS);
	virtual RHIPixelShader* NewPixelShader(size_t index, const void* data,
		unsigned int size);
	virtual RHIPixelShader* NewPixelShaderFromFile(size_t index,
		const char* file, const char* func, const char* def);
	virtual void DeletePixelShader(RHIPixelShader* pVS);
	virtual RHIShaderProgram* NewShaderProgram(size_t index,
		RHIVertexShader* pVS, RHIPixelShader* pPS);
	virtual RHIShaderProgram* NewShaderProgramEmpty(size_t index);
	virtual void DeleteShaderProgram(RHIShaderProgram* pProgram);
	virtual RHIVertexDescriptor* NewVertexDescriptor(size_t index,
		const rhi_vertex_attrib_t* pAttribs, int attrib_num,
		unsigned int stride, RHIShaderProgram* pSP);
	virtual void DeleteVertexDescriptor(RHIVertexDescriptor* pVD);
	virtual RHIStaticVertexBuffer* NewStaticVertexBuffer(size_t index,
		const void* data, unsigned int size, unsigned int stride);
	virtual void DeleteStaticVertexBuffer(RHIStaticVertexBuffer* pVB);
	virtual RHIStaticIndexBuffer* NewStaticIndexBuffer(size_t index,
		const void* data, unsigned int size);
	virtual void DeleteStaticIndexBuffer(RHIStaticIndexBuffer* pIB);
	virtual RHIStaticUniformBuffer* NewStaticUniformBuffer(size_t index,
		const void* data, unsigned int size);
	virtual void DeleteStaticUniformBuffer(RHIStaticUniformBuffer* pUB);
	virtual RHIStaticTexture* NewStaticTexture(size_t index, int width,
		int height, int format, int levels, const void* data, size_t size);
	virtual void DeleteStaticTexture(RHIStaticTexture* pST);
	virtual RHIDynamicVertexBuffer* NewDynamicVertexBuffer(size_t index,
		unsigned int size, unsigned int stride);
	virtual void DeleteDynamicVertexBuffer(RHIDynamicVertexBuffer* pVB);
	virtual RHIDynamicIndexBuffer* NewDynamicIndexBuffer(size_t index,
		unsigned int size);
	virtual void DeleteDynamicIndexBuffer(RHIDynamicIndexBuffer* pIB);
	virtual RHIDynamicUniformBuffer* NewDynamicUniformBuffer(size_t index,
		unsigned int size);
	virtual void DeleteDynamicUniformBuffer(RHIDynamicUniformBuffer* pUB);
	virtual RHIDynamicTexture* NewDynamicTexture(size_t index, int width, 
		int height, int format, int levels);
	virtual void DeleteDynamicTexture(RHIDynamicTexture* pDT);
	virtual RHISamplerState* NewSamplerState(size_t index,
		const rhi_sampler_desc_t& desc);
	virtual void DeleteSamplerState(RHISamplerState* pSS);
	virtual RHIRasterizeState* NewRasterizeState(size_t index,
		const rhi_rasterize_desc_t& desc);
	virtual void DeleteRasterizeState(RHIRasterizeState* pRS);
	virtual RHIDepthStencilState* NewDepthStencilState(size_t index,
		const rhi_depth_stencil_desc_t& desc);
	virtual void DeleteDepthStencilState(RHIDepthStencilState* pDSS);
	virtual RHIBlendState* NewBlendState(size_t index,
		const rhi_blend_desc_t& desc);
	virtual void DeleteBlendState(RHIBlendState* pBS);
	virtual RHIShaderTexture* NewShaderTexture(int width, int height,
		int format, int levels, const void* data, size_t size);

private:
	RenderService* m_pRenderService;
	bool m_bSaveShader;
	GLES3ShaderProgram* m_pCurrentProgram;
	GLES3RasterizeState* m_pCurrentRasterizeState;
	GLES3DepthStencilState* m_pCurrentDepthStencilState;
	unsigned int m_nCurrentStencilRef;
	GLES3BlendState* m_pCurrentBlendState;
	float m_fCurrentBlendFactor[4];
	//unsigned int m_nCurrentSampleMask;
	int m_nCurrentIndexFormat;
	unsigned int m_nCurrentIndexOffset;
};

#endif // __GLES3_DEIVCE_H
