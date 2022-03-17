// i_render_service.h
// Created by lulimin on 2020/10/13.

#ifndef __I_RENDER_SERVICE_H
#define __I_RENDER_SERVICE_H

#include "../inc/common.h"
#include "../inc/frame_def.h"
#include "rhi_resource.h"
#include "rhi_device.h"
#include "i_texture.h"
#include "i_asset_loader.h"

// IRenderService

class IRenderService : public IService
{
public:
	// Asset indicators.
	enum ASSET_INDICATOR_ENUM
	{
		ASSET_INDICATOR_UI_IMAGE = 1,
		ASSET_INDICATOR_MODEL_TEXTURE = 2,
		ASSET_INDICATOR_GEOMETRY = 3,
		ASSET_INDICATOR_ACTION = 4,
		ASSET_INDICATOR_LANDSCAPE = 5,
		ASSET_INDICATOR_SOUND = 6,
		ASSET_INDICATOR_MUSIC = 7,
	};
	
	typedef bool(*egl_present_func_t)(void*);

public:
	// Initialize renderer.
	virtual bool StartRender(int rhi_api_type, int asset_thread_num, 
		int back_buffer_width, int back_buffer_height) = 0;

	// Begin render a frame.
	virtual bool FrameBegin(float red, float green, float blue,
		float alpha) = 0;
	// End render current frame.
	virtual bool FrameEnd() = 0;

	// Set EGL function pointer.
	virtual void SetEglFunction(egl_present_func_t present_func,
		void* param) = 0;

	// Add asset loading task.
	virtual bool AddAssetTask(int asset_indicator, int task_type,
		IAssetLoader* pLoader) = 0;
	// Check asset thread per frame.
	virtual void CheckAssetThread() = 0;

	// Set window size.
	virtual bool SetWindowSize(int width, int height) = 0;
	// Get window size.
	virtual int GetWindowWidth() = 0;
	virtual int GetWindowHeight() = 0;
	// Set safe area of screen.
	virtual bool SetSafeArea(int left, int top, int right, int bottom) = 0;
	// Get safe area of screen.
	virtual int GetSafeAreaLeft() = 0;
	virtual int GetSafeAreaTop() = 0;
	virtual int GetSafeAreaRight() = 0;
	virtual int GetSafeAreaBottom() = 0;

	// Get back buffer size.
	virtual int GetRenderWidth() = 0;
	virtual int GetRenderHeight() = 0;
	// Get render hardware interface device.
	virtual RHIDevice* GetRenderDevice() = 0;

	// Create frame buffer.
	virtual RHIFrameBuffer* CreateFrameBuffer(RHIColorBuffer* pCB,
		RHIDepthBuffer* pDB) = 0;
	// Create color buffer.
	virtual RHIColorBuffer* CreateColorBuffer(int width, int height,
		int format, int samples) = 0;
	// Create depth buffer.
	virtual RHIDepthBuffer* CreateDepthBuffer(int width, int height,
		int format, int samples) = 0;
	// Create color texture.
	virtual RHIColorTexture* CreateColorTexture(int width, int height,
		int format) = 0;
	// Create depth texture.
	virtual RHIDepthTexture* CreateDepthTexture(int width, int height,
		int format) = 0;
	// Create vertex descriptor.
	virtual RHIVertexDescriptor* CreateVertexDescriptor(
		const rhi_vertex_attrib_t* pAttribs, int attrib_num,
		unsigned int stride, RHIShaderProgram* pSP) = 0;
	// Create staitic vertex buffer.
	virtual RHIStaticVertexBuffer* CreateStaticVertexBuffer(const void* data,
		unsigned int size, unsigned int stride) = 0;
	// Create staitic index buffer.
	virtual RHIStaticIndexBuffer* CreateStaticIndexBuffer(const void* data,
		unsigned int size) = 0;
	// Create static uniform buffer.
	virtual RHIStaticUniformBuffer* CreateStaticUniformBuffer(const void* data,
		unsigned int size) = 0;
	// Create static texture.
	virtual RHIStaticTexture* CreateStaticTexture(int width, int height,
		int format, int levels, const void* data, size_t size) = 0;
	// Create dynamic vertex buffer.
	virtual RHIDynamicVertexBuffer* CreateDynamicVertexBuffer(
		unsigned int size, unsigned int stride) = 0;
	// Create dynamic index buffer.
	virtual RHIDynamicIndexBuffer* CreateDynamicIndexBuffer(
		unsigned int size) = 0;
	// Create dynamic uniform buffer.
	virtual RHIDynamicUniformBuffer* CreateDynamicUniformBuffer(
		unsigned int size) = 0;
	// Create dynamic texture.
	virtual RHIDynamicTexture* CreateDynamicTexture(int width, int height,
		int format, int levels) = 0;
	// Create texture sampler state.
	virtual RHISamplerState* CreateSamplerState(
		const rhi_sampler_desc_t& desc) = 0;
	// Create rasterize state.
	virtual RHIRasterizeState* CreateRasterizeState(
		const rhi_rasterize_desc_t& desc) = 0;
	// Create depth & stencil state.
	virtual RHIDepthStencilState* CreateDepthStencilState(
		const rhi_depth_stencil_desc_t& desc) = 0;
	// Create blend state.
	virtual RHIBlendState* CreateBlendState(const rhi_blend_desc_t& desc) = 0;

	// Load shader program.
	virtual RHIShaderProgram* LoadShaderProgram(const char* vs_file,
		const char* vs_func, const char* vs_def, const char* ps_file,
		const char* ps_func, const char* ps_def) = 0;

	// Load texture.
	virtual ITexture* LoadTexture(const char* name, bool async,
		int asset_indicator) = 0;
};

#endif // __I_RENDER_SERVICE_H
