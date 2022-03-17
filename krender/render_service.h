// render_service.h
// Created by lulimin on 2020/10/13.

#ifndef __RENDER_SERVICE_H
#define __RENDER_SERVICE_H

#include "../kgraphic/i_render_service.h"
#include "../inc/pod_array.h"
#include "../inc/string_hash_table.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_types.h"
#include "asset_container.h"

DEFINE_FRAME_ALLOC(RenderService);

// RenderService

class AssetThread;
class Texture;

class RenderService : public IRenderService
{
private:
	// Function pointer for create renderer device.
	typedef RHIDevice*(*create_device_func_t)(RenderService* pRS);

	// Maximum number of asset threads.
	enum { MAX_ASSET_THREAD_NUM = 8 };

	struct shader_slice_t
	{
		FrameString sVSFunc;
		FrameString sVSDef;
		FrameString sPSFunc;
		FrameString sPSDef;
		RHIShaderProgram* pProgram;
	};
	
	struct shader_item_t
	{
		FrameString sVSFileName;
		FrameString sPSFileName;
		uint64_t nVSFileTime;
		uint64_t nPSFileTime;
		TPodArray<shader_slice_t*, 1, FrameAlloc> Slices;
	};

public:
	RenderService();
	virtual ~RenderService();

	// Service startup.
	virtual bool Startup();
	// Service shutdown.
	virtual bool Shutdown();

	// Initialize renderer.
	virtual bool StartRender(int rhi_api_type, int asset_thread_num,
		int back_buffer_width, int back_buffer_height);

	// Begin render a frame.
	virtual bool FrameBegin(float red, float green, float blue, float alpha);
	// End render current frame.
	virtual bool FrameEnd();

	// Set EGL function pointer.
	virtual void SetEglFunction(egl_present_func_t present_func, void* param);

	// Add asset loading task.
	virtual bool AddAssetTask(int asset_indicator, int task_type,
		IAssetLoader* pLoader);
	// Check asset thread per frame.
	virtual void CheckAssetThread();

	// Set window size.
	virtual bool SetWindowSize(int width, int height);
	// Get window size.
	virtual int GetWindowWidth();
	virtual int GetWindowHeight();
	// Set safe area of screen.
	virtual bool SetSafeArea(int left, int top, int right, int bottom);
	// Get safe area of screen.
	virtual int GetSafeAreaLeft();
	virtual int GetSafeAreaTop();
	virtual int GetSafeAreaRight();
	virtual int GetSafeAreaBottom();

	// Get back buffer size.
	virtual int GetRenderWidth();
	virtual int GetRenderHeight();
	// Get render hardware interface device.
	virtual RHIDevice* GetRenderDevice();

	// Create frame buffer.
	virtual RHIFrameBuffer* CreateFrameBuffer(RHIColorBuffer* pCB,
		RHIDepthBuffer* pDB);
	// Create color buffer.
	virtual RHIColorBuffer* CreateColorBuffer(int width, int height,
		int format, int samples);
	// Create depth buffer.
	virtual RHIDepthBuffer* CreateDepthBuffer(int width, int height,
		int format, int samples);
	// Create color texture.
	virtual RHIColorTexture* CreateColorTexture(int width, int height,
		int format);
	// Create depth texture.
	virtual RHIDepthTexture* CreateDepthTexture(int width, int height,
		int format);
	// Create vertex descriptor.
	virtual RHIVertexDescriptor* CreateVertexDescriptor(
		const rhi_vertex_attrib_t* pAttribs, int attrib_num,
		unsigned int stride, RHIShaderProgram* pSP);
	// Create static vertex buffer.
	virtual RHIStaticVertexBuffer* CreateStaticVertexBuffer(const void* data,
		unsigned int size, unsigned int stride);
	// Create static index buffer.
	virtual RHIStaticIndexBuffer* CreateStaticIndexBuffer(const void* data,
		unsigned int size);
	// Create static uniform buffer.
	virtual RHIStaticUniformBuffer* CreateStaticUniformBuffer(const void* data,
		unsigned int size);
	// Create static texture.
	virtual RHIStaticTexture* CreateStaticTexture(int width, int height,
		int format, int levels, const void* data, size_t size);
	// Create dynamic vertex buffer.
	virtual RHIDynamicVertexBuffer* CreateDynamicVertexBuffer(
		unsigned int size, unsigned int stride);
	// Create dynamic index buffer.
	virtual RHIDynamicIndexBuffer* CreateDynamicIndexBuffer(unsigned int size);
	// Create dynamic uniform buffer.
	virtual RHIDynamicUniformBuffer* CreateDynamicUniformBuffer(
		unsigned int size);
	// Create dynamic texture.
	virtual RHIDynamicTexture* CreateDynamicTexture(int width, int height,
		int format, int levels);
	// Create texture sampler state.
	virtual RHISamplerState* CreateSamplerState(
		const rhi_sampler_desc_t& desc);
	// Create rasterize state.
	virtual RHIRasterizeState* CreateRasterizeState(
		const rhi_rasterize_desc_t& desc);
	// Create depth & stencil state.
	virtual RHIDepthStencilState* CreateDepthStencilState(
		const rhi_depth_stencil_desc_t& desc);
	// Create blend state.
	virtual RHIBlendState* CreateBlendState(const rhi_blend_desc_t& desc);

	// Load shader program.
	virtual RHIShaderProgram* LoadShaderProgram(const char* vs_file,
		const char* vs_func, const char* vs_def, const char* ps_file,
		const char* ps_func, const char* ps_def);

	// Load texture.
	virtual ITexture* LoadTexture(const char* name, bool async, 
		int asset_indicator);

	// Delete texture.
	void ReleaseTexture(Texture* pTexture);

	// Delete resource.
	void ReleaseResource(RHIResource* pResource);

	// Get back buffer size.
	int GetBackBufferWidth() const { return m_nBackBufferWidth; }
	int GetBackBufferHeight() const { return m_nBackBufferHeight; }

private:
	// Delete render resource.
	void DeleteResource(RHIResource* pResource);
	// Clear render resources.
	void ClearResources();
	// Clear all textures.
	void ClearTextures();
	// Delete shaders of specified vertex and fragment shader file.
	void DeleteShaderItem(shader_item_t* pItem);
	// Generate name of shader binary file.
	void GetShaderBinaryFileName(shader_item_t* pItem, char* name, 
		size_t name_size);
	// Save shader binary file.
	bool SaveShaderItem(shader_item_t* pItem);
	// Load shader from binary file.
	bool LoadShaderItem(shader_item_t* pItem);

private:
	RHIDevice* m_pDevice;
	int m_nWindowWidth;
	int m_nWindowHeight;
	int m_nBackBufferWidth;
	int m_nBackBufferHeight;
	int m_nSafeAreaLeft;
	int m_nSafeAreaTop;
	int m_nSafeAreaRight;
	int m_nSafeAreaBottom;
	egl_present_func_t m_EglPresentFunc;
	void* m_pEglParameter;
	size_t m_nAssetThreadNum;
	AssetThread* m_AssetThreads[MAX_ASSET_THREAD_NUM];
	TAssetContainer<RHIResource, RenderServiceAlloc> m_Resources;
	TPodArray<shader_item_t*, 1, RenderServiceAlloc> m_Shaders;
	TAssetContainer<Texture, RenderServiceAlloc> m_Textures;
	TStringHashTable<char, size_t, TCharTraitsCI<char>, 
		RenderServiceAlloc> m_TextureIndices;
};

#endif // __RENDER_SERVICE_H
