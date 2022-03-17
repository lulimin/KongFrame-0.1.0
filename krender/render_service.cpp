// render_service.cpp
// Created by lulimin on 2020/10/13.

#include "render_service.h"
#include "asset_thread.h"
#include "texture.h"
#include "../inc/platform.h"
#include "../inc/i_file_access.h"
#include "../inc/frame_file.h"
#include "../inc/auto_buffer.h"
//#include "gles3/gles3_device.h"

/// \file render_service.cpp
/// \brief Render service.

/// service: RenderService
/// \brief Render service.
DEF_SERVICE(RenderService);

// RenderService

RenderService::RenderService()
{
	m_pDevice = NULL;
	m_nWindowWidth = 1920;
	m_nWindowHeight = 1080;
	m_nBackBufferWidth = 1920;
	m_nBackBufferHeight = 1080;
	m_nSafeAreaLeft = 0;
	m_nSafeAreaTop = 0;
	m_nSafeAreaRight = 0;
	m_nSafeAreaBottom = 0;
	m_EglPresentFunc = NULL;
	m_pEglParameter = NULL;
	m_nAssetThreadNum = 0;
	memset(m_AssetThreads, 0, sizeof(m_AssetThreads));
}

RenderService::~RenderService()
{
	for (size_t i = 0; i < m_nAssetThreadNum; ++i)
	{
		if (m_AssetThreads[i])
		{
			m_AssetThreads[i]->Stop();
			K_DELETE(m_AssetThreads[i]);
			m_AssetThreads[i] = NULL;
		}
	}

	for (size_t k = 0; k < m_Shaders.Size(); ++k)
	{
		this->DeleteShaderItem(m_Shaders[k]);
	}

	this->ClearResources();
	this->ClearTextures();
	SAFE_RELEASE(m_pDevice);
}

bool RenderService::Startup()
{
	return true;
}

bool RenderService::Shutdown()
{
	if (m_pDevice)
	{
		m_pDevice->Cleanup();
	}
	
	return true;
}

bool RenderService::StartRender(int rhi_api_type, int asset_thread_num,
	int back_buffer_width, int back_buffer_height)
{
	Assert((asset_thread_num > 0) && 
		(asset_thread_num <= MAX_ASSET_THREAD_NUM));
	Assert(back_buffer_width > 0);
	Assert(back_buffer_height > 0);
	Assert(NULL == m_pDevice);

	m_nBackBufferWidth = back_buffer_width;
	m_nBackBufferHeight = back_buffer_height;

	create_device_func_t create_device_func = NULL;

#ifdef K_PLATFORM_WINDOWS
	extern RHIDevice* create_gles3_device(RenderService* pRS);

	if (rhi_api_type == RHI_API_DX11)
	{
		Assert(0);
	}
	else if (rhi_api_type == RHI_API_DX12)
	{
		Assert(0);
	}
	else if (rhi_api_type == RHI_API_VULKAN)
	{
		Assert(0);
	}
	else
	{
		create_device_func = create_gles3_device;
	}
#endif // K_PLATFORM_WINDOWS

#ifdef K_PLATFORM_IOS
	extern RHIDevice* create_gles3_device(RenderService* pRS);

	if (rhi_api_type == RHI_API_METAL)
	{
		Assert(0);
	}
	else
	{
		create_device_func = create_gles3_device;
	}
#endif // K_PLATFORM_IOS

#ifdef K_PLATFORM_MAC
	extern RHIDevice* create_gles3_device(RenderService* pRS);

	if (rhi_api_type == RHI_API_METAL)
	{
		Assert(0);
	}
	else
	{
		create_device_func = create_gles3_device;
	}
#endif // K_PLATFORM_MAC

#ifdef K_PLATFORM_ANDROID
	extern RHIDevice* create_gles3_device(RenderService* pRS);

	if (rhi_api_type == RHI_API_VULKAN)
	{
		Assert(0);
	}
	else
	{
		create_device_func = create_gles3_device;
	}
#endif // K_PLATFORM_ANDROID

	if (NULL == create_device_func)
	{
		K_LOG("(RenderService::StartRender)No render device!");
		return false;
	}

	m_pDevice = create_device_func(this);

	if (NULL == m_pDevice)
	{
		K_LOG("(RenderService::StartRender)Create render device failed!");
		return false;
	}

	if (!m_pDevice->Startup())
	{
		K_LOG("(RenderService::StartRender)Render device start failed!");
		return false;
	}

	// Check shader binary directory.
	IFileAccess* pFileAccess = frame_get_file_access();
	char sbin_dir[256];

	plat_safe_sprintf(sbin_dir, sizeof(sbin_dir), "%sshader_bin",
		frame_get_output_path());

	if (!pFileAccess->DirectoryExists(sbin_dir))
	{
		if (!pFileAccess->DirectoryCreate(sbin_dir))
		{
			K_LOG("(RenderService::StartRender)"
				"Cannot create shader binary directory.");
			return false;
		}
	}

	// Create asset threads.
	Assert(m_nAssetThreadNum == 0);

	for (size_t i = 0; i < asset_thread_num; ++i)
	{
		AssetThread* pThread = K_NEW(AssetThread);

		pThread->Start();
		m_AssetThreads[i] = pThread;
	}
	
	m_nAssetThreadNum = asset_thread_num;

	//if (m_EglSetContextFunc)
	//{
	//	m_EglSetContextFunc(m_pEglParameter);
	//}
	
	return true;
}

bool RenderService::FrameBegin(float red, float green, float blue, float alpha)
{
	m_pDevice->ClearFrame(red, green, blue, alpha, 1.0F, 0);
	return true;
}

bool RenderService::FrameEnd()
{
	if (m_EglPresentFunc)
	{
		m_EglPresentFunc(m_pEglParameter);
	}

	return true;
}

void RenderService::SetEglFunction(egl_present_func_t present_func, void* param)
{
	m_EglPresentFunc = present_func;
	m_pEglParameter = param;
}

bool RenderService::AddAssetTask(int asset_indicator, int task_type,
	IAssetLoader* pLoader)
{
	Assert(pLoader != NULL);

	size_t index = (size_t)asset_indicator % m_nAssetThreadNum;

	Assert(m_AssetThreads[index] != NULL);

	return m_AssetThreads[index]->AddTask(task_type, pLoader);
}

void RenderService::CheckAssetThread()
{
	for (size_t i = 0; i < m_nAssetThreadNum; ++i)
	{
		m_AssetThreads[i]->CheckOutTask();
	}
}

bool RenderService::SetWindowSize(int width, int height)
{
	m_nWindowWidth = width;
	m_nWindowHeight = height;
	return true;
}

int RenderService::GetWindowWidth()
{
	return m_nWindowWidth;
}

int RenderService::GetWindowHeight()
{
	return m_nWindowHeight;
}

bool RenderService::SetSafeArea(int left, int top, int right, int bottom)
{
	m_nSafeAreaLeft = left;
	m_nSafeAreaTop = top;
	m_nSafeAreaRight = right;
	m_nSafeAreaBottom = bottom;
	return true;
}

int RenderService::GetSafeAreaLeft()
{
	return m_nSafeAreaLeft;
}

int RenderService::GetSafeAreaTop()
{
	return m_nSafeAreaTop;
}

int RenderService::GetSafeAreaRight()
{
	return m_nSafeAreaRight;
}

int RenderService::GetSafeAreaBottom()
{
	return m_nSafeAreaBottom;
}

int RenderService::GetRenderWidth()
{
	return m_nBackBufferWidth;
}

int RenderService::GetRenderHeight()
{
	return m_nBackBufferHeight;
}

RHIDevice* RenderService::GetRenderDevice()
{
	return m_pDevice;
}

RHIFrameBuffer* RenderService::CreateFrameBuffer(RHIColorBuffer* pCB,
	RHIDepthBuffer* pDB)
{
	Assert(pCB != NULL);
	Assert(pDB != NULL);

	size_t index = m_Resources.GetIndex();
	RHIFrameBuffer* pFB = m_pDevice->NewFrameBuffer(index, pCB, pDB);

	if (NULL == pFB)
	{
		return NULL;
	}

	m_Resources.Add(pFB);
	return pFB;
}

RHIColorBuffer* RenderService::CreateColorBuffer(int width, int height,
	int format, int samples)
{
	size_t index = m_Resources.GetIndex();
	RHIColorBuffer* pCB = m_pDevice->NewColorBuffer(index, width, height,
		format, samples);

	if (NULL == pCB)
	{
		return NULL;
	}

	m_Resources.Add(pCB);
	return pCB;
}

RHIDepthBuffer* RenderService::CreateDepthBuffer(int width, int height,
	int format, int samples)
{
	size_t index = m_Resources.GetIndex();
	RHIDepthBuffer* pDB = m_pDevice->NewDepthBuffer(index, width, height,
		format, samples);

	if (NULL == pDB)
	{
		return NULL;
	}

	m_Resources.Add(pDB);
	return pDB;
}

RHIColorTexture* RenderService::CreateColorTexture(int width, int height,
	int format)
{
	size_t index = m_Resources.GetIndex();
	RHIColorTexture* pCT = m_pDevice->NewColorTexture(index, width, height,
		format);

	if (NULL == pCT)
	{
		return NULL;
	}

	m_Resources.Add(pCT);
	return pCT;
}

RHIDepthTexture* RenderService::CreateDepthTexture(int width, int height,
	int format)
{
	size_t index = m_Resources.GetIndex();
	RHIDepthTexture* pDT = m_pDevice->NewDepthTexture(index, width, height,
		format);

	if (NULL == pDT)
	{
		return NULL;
	}

	m_Resources.Add(pDT);
	return pDT;
}

RHIVertexDescriptor* RenderService::CreateVertexDescriptor(
	const rhi_vertex_attrib_t* pAttribs, int attrib_num,
	unsigned int stride, RHIShaderProgram* pSP)
{
	Assert(pAttribs != NULL);
	Assert((attrib_num > 0) && (attrib_num <= RHI_VERTEX_ATTRIB_MAX));
	Assert(pSP != NULL);

	size_t index = m_Resources.GetIndex();
	RHIVertexDescriptor* pVD = m_pDevice->NewVertexDescriptor(index, pAttribs, 
		attrib_num, stride, pSP);

	if (NULL == pVD)
	{
		return NULL;
	}

	m_Resources.Add(pVD);
	return pVD;
}

RHIStaticVertexBuffer* RenderService::CreateStaticVertexBuffer(
	const void* data, unsigned int size, unsigned int stride)
{
	size_t index = m_Resources.GetIndex();
	RHIStaticVertexBuffer* pVB = m_pDevice->NewStaticVertexBuffer(index, data,
		size, stride);

	if (NULL == pVB)
	{
		return NULL;
	}

	m_Resources.Add(pVB);
	return pVB;
}

RHIStaticIndexBuffer* RenderService::CreateStaticIndexBuffer(const void* data,
	unsigned int size)
{
	size_t index = m_Resources.GetIndex();
	RHIStaticIndexBuffer* pIB = m_pDevice->NewStaticIndexBuffer(index, data,
		size);

	if (NULL == pIB)
	{
		return NULL;
	}

	m_Resources.Add(pIB);
	return pIB;
}

RHIStaticUniformBuffer* RenderService::CreateStaticUniformBuffer(
	const void* data, unsigned int size)
{
	size_t index = m_Resources.GetIndex();
	RHIStaticUniformBuffer* pUB = m_pDevice->NewStaticUniformBuffer(index,
		data, size);

	if (NULL == pUB)
	{
		return NULL;
	}

	m_Resources.Add(pUB);
	return pUB;
}

RHIStaticTexture* RenderService::CreateStaticTexture(int width, int height,
	int format, int levels, const void* data, size_t size)
{
	size_t index = m_Resources.GetIndex();
	RHIStaticTexture* pST = m_pDevice->NewStaticTexture(index, width, height,
		format, levels, data, size);

	if (NULL == pST)
	{
		return NULL;
	}

	m_Resources.Add(pST);
	return pST;
}

RHIDynamicVertexBuffer* RenderService::CreateDynamicVertexBuffer(
	unsigned int size, unsigned int stride)
{
	size_t index = m_Resources.GetIndex();
	RHIDynamicVertexBuffer* pVB = m_pDevice->NewDynamicVertexBuffer(index, 
		size, stride);

	if (NULL == pVB)
	{
		return NULL;
	}

	m_Resources.Add(pVB);
	return pVB;
}

RHIDynamicIndexBuffer* RenderService::CreateDynamicIndexBuffer(
	unsigned int size)
{
	size_t index = m_Resources.GetIndex();
	RHIDynamicIndexBuffer* pIB = m_pDevice->NewDynamicIndexBuffer(index,
		size);

	if (NULL == pIB)
	{
		return NULL;
	}

	m_Resources.Add(pIB);
	return pIB;
}

RHIDynamicUniformBuffer* RenderService::CreateDynamicUniformBuffer(
	unsigned int size)
{
	size_t index = m_Resources.GetIndex();
	RHIDynamicUniformBuffer* pUB = m_pDevice->NewDynamicUniformBuffer(index,
		size);

	if (NULL == pUB)
	{
		return NULL;
	}

	m_Resources.Add(pUB);
	return pUB;
}

RHIDynamicTexture* RenderService::CreateDynamicTexture(int width, int height,
	int format, int levels)
{
	size_t index = m_Resources.GetIndex();
	RHIDynamicTexture* pDT = m_pDevice->NewDynamicTexture(index, width, height, 
		format, levels);

	if (NULL == pDT)
	{
		return NULL;
	}

	m_Resources.Add(pDT);
	return pDT;
}

RHISamplerState* RenderService::CreateSamplerState(
	const rhi_sampler_desc_t& desc)
{
	size_t index = m_Resources.GetIndex();
	RHISamplerState* pSS = m_pDevice->NewSamplerState(index, desc);

	if (NULL == pSS)
	{
		return NULL;
	}

	m_Resources.Add(pSS);
	return pSS;
}

RHIRasterizeState* RenderService::CreateRasterizeState(
	const rhi_rasterize_desc_t& desc)
{
	size_t index = m_Resources.GetIndex();
	RHIRasterizeState* pRS = m_pDevice->NewRasterizeState(index, desc);

	if (NULL == pRS)
	{
		return NULL;
	}

	m_Resources.Add(pRS);
	return pRS;
}

RHIDepthStencilState* RenderService::CreateDepthStencilState(
	const rhi_depth_stencil_desc_t& desc)
{
	size_t index = m_Resources.GetIndex();
	RHIDepthStencilState* pDSS = m_pDevice->NewDepthStencilState(index, desc);

	if (NULL == pDSS)
	{
		return NULL;
	}

	m_Resources.Add(pDSS);
	return pDSS;
}

RHIBlendState* RenderService::CreateBlendState(const rhi_blend_desc_t& desc)
{
	size_t index = m_Resources.GetIndex();
	RHIBlendState* pBS = m_pDevice->NewBlendState(index, desc);

	if (NULL == pBS)
	{
		return NULL;
	}

	m_Resources.Add(pBS);
	return pBS;
}

void RenderService::ReleaseResource(RHIResource* pResource)
{
	Assert(pResource != NULL);
	
	size_t index = pResource->GetIndex();

	DeleteResource(pResource);
	m_Resources.Remove(pResource, index);
}

void RenderService::DeleteResource(RHIResource* pResource)
{
	Assert(pResource != NULL);

	switch (pResource->GetResourceType())
	{
	case RHI_RESOURCE_FRAME_BUFFER:
		m_pDevice->DeleteFrameBuffer((RHIFrameBuffer*)pResource);
		break;
	case RHI_RESOURCE_COLOR_BUFFER:
		m_pDevice->DeleteColorBuffer((RHIColorBuffer*)pResource);
		break;
	case RHI_RESOURCE_DEPTH_BUFFER:
		m_pDevice->DeleteDepthBuffer((RHIDepthBuffer*)pResource);
		break;
	case RHI_RESOURCE_COLOR_TEXTURE:
		m_pDevice->DeleteColorTexture((RHIColorTexture*)pResource);
		break;
	case RHI_RESOURCE_DEPTH_TEXTURE:
		m_pDevice->DeleteDepthTexture((RHIDepthTexture*)pResource);
		break;
	case RHI_RESOURCE_VERTEX_SHADER:
		m_pDevice->DeleteVertexShader((RHIVertexShader*)pResource);
		break;
	case RHI_RESOURCE_PIXEL_SHADER:
		m_pDevice->DeletePixelShader((RHIPixelShader*)pResource);
		break;
	case RHI_RESOURCE_SHADER_PROGRAM:
		m_pDevice->DeleteShaderProgram((RHIShaderProgram*)pResource);
		break;
	case RHI_RESOURCE_VERTEX_DESCRIPTOR:
		m_pDevice->DeleteVertexDescriptor((RHIVertexDescriptor*)pResource);
		break;
	case RHI_RESOURCE_STATIC_VERTEX_BUFFER:
		m_pDevice->DeleteStaticVertexBuffer((RHIStaticVertexBuffer*)pResource);
		break;
	case RHI_RESOURCE_STATIC_INDEX_BUFFER:
		m_pDevice->DeleteStaticIndexBuffer((RHIStaticIndexBuffer*)pResource);
		break;
	case RHI_RESOURCE_STATIC_UNIFORM_BUFFER:
		m_pDevice->DeleteStaticUniformBuffer(
			(RHIStaticUniformBuffer*)pResource);
		break;
	case RHI_RESOURCE_STATIC_TEXTURE:
		m_pDevice->DeleteStaticTexture((RHIStaticTexture*)pResource);
		break;
	case RHI_RESOURCE_DYNAMIC_VERTEX_BUFFER:
		m_pDevice->DeleteDynamicVertexBuffer(
			(RHIDynamicVertexBuffer*)pResource);
		break;
	case RHI_RESOURCE_DYNAMIC_INDEX_BUFFER:
		m_pDevice->DeleteDynamicIndexBuffer((RHIDynamicIndexBuffer*)pResource);
		break;
	case RHI_RESOURCE_DYNAMIC_UNIFORM_BUFFER:
		m_pDevice->DeleteDynamicUniformBuffer(
			(RHIDynamicUniformBuffer*)pResource);
		break;
	case RHI_RESOURCE_DYNAMIC_TEXTURE:
		m_pDevice->DeleteDynamicTexture((RHIDynamicTexture*)pResource);
		break;
	case RHI_RESOURCE_SAMPLER_STATE:
		m_pDevice->DeleteSamplerState((RHISamplerState*)pResource);
		break;
	case RHI_RESOURCE_RASTERIZE_STATE:
		m_pDevice->DeleteRasterizeState((RHIRasterizeState*)pResource);
		break;
	case RHI_RESOURCE_DEPTH_STENCIL_STATE:
		m_pDevice->DeleteDepthStencilState((RHIDepthStencilState*)pResource);
		break;
	case RHI_RESOURCE_BLEND_STATE:
		m_pDevice->DeleteBlendState((RHIBlendState*)pResource);
		break;
	default:
		Assert(0);
		break;
	}
}

void RenderService::ClearResources()
{
	RHIResource** elements = m_Resources.GetElements();
	size_t size = m_Resources.GetSize();

	for (size_t i = 0; i < size; ++i)
	{
		if (elements[i])
		{
			this->DeleteResource(elements[i]);
		}
	}

	m_Resources.Clear();
}

void RenderService::ClearTextures()
{
	Texture** elements = m_Textures.GetElements();
	size_t size = m_Textures.GetSize();

	for (size_t i = 0; i < size; ++i)
	{
		if (elements[i])
		{
			Texture::DeleteInstance(elements[i]);
		}
	}

	m_Textures.Clear();
	m_TextureIndices.Clear();
}

static bool get_file_time(const char* fname, uint64_t* pTime)
{
	Assert(fname != NULL);
	Assert(pTime != NULL);

	IFileAccess::file_time_t ft;

	if (!frame_get_file_access()->FileTime(fname, &ft))
	{
		return false;
	}

	*pTime = 
		(((uint64_t)ft.nYear & 0xFFFF) << 40) + 
		(((uint64_t)ft.nMonth & 0xFF) << 32) +
		(((uint64_t)ft.nDay & 0xFF) << 24) + 
		(((uint64_t)ft.nHour & 0xFF) << 16) +
		(((uint64_t)ft.nMinute & 0xFF) << 8) + 
		(((uint64_t)ft.nSecond & 0xFF) << 0);
	return true;
}

RHIShaderProgram* RenderService::LoadShaderProgram(const char* vs_file,
	const char* vs_func, const char* vs_def, const char* ps_file,
	const char* ps_func, const char* ps_def)
{
	Assert(vs_file != NULL);
	Assert(vs_func != NULL);
	Assert(vs_def != NULL);
	Assert(ps_file != NULL);
	Assert(ps_func != NULL);
	Assert(ps_def != NULL);

	char vs_name[512];
	char ps_name[512];

	plat_safe_sprintf(vs_name, sizeof(vs_name), "%sshader/%s.%s",
		frame_get_coder_path(), vs_file, m_pDevice->GetVertexShaderFileExt());
	plat_safe_sprintf(ps_name, sizeof(ps_name), "%sshader/%s.%s",
		frame_get_coder_path(), ps_file, m_pDevice->GetPixelShaderFileExt());

	uint64_t vs_time;
	uint64_t ps_time;

	if ((!get_file_time(vs_name, &vs_time)) ||
		(!get_file_time(ps_name, &ps_time)))
	{
		return NULL;
	}

	shader_item_t* pItem = NULL;
	int found_index = -1;

	for (size_t i = 0; i < m_Shaders.Size(); ++i)
	{
		shader_item_t* p = m_Shaders[i];

		if ((stricmp(p->sVSFileName.CString(), vs_file) == 0) &&
			(stricmp(p->sPSFileName.CString(), ps_file) == 0))
		{
		 	pItem = p;
			found_index = (int)i;
			break;
		}
	}

	if (pItem)
	{
		if ((pItem->nVSFileTime != vs_time) ||
			(pItem->nPSFileTime != ps_time))
		{
			// Shader binary time out.
			this->DeleteShaderItem(pItem);
			m_Shaders.Remove(found_index);
			pItem = NULL;
		}
	}

	if (NULL == pItem)
	{
		// Create new shader entry.
		pItem = K_NEW(shader_item_t);
		pItem->sVSFileName = vs_file;
		pItem->sPSFileName = ps_file;
		pItem->nVSFileTime = vs_time;
		pItem->nPSFileTime = ps_time;
		m_Shaders.PushBack(pItem);
		
		if (m_pDevice->GetSaveShader())
		{
			// Try load shader binary from file.
			this->LoadShaderItem(pItem);
		}
	}
	
	// Search exists shader program.
	for (size_t k = 0; k < pItem->Slices.Size(); ++k)
	{
		shader_slice_t* p = pItem->Slices[k];

		if ((p->sVSFunc == vs_func) && (p->sVSDef == vs_def) &&
			(p->sPSFunc == ps_func) && (p->sPSDef == ps_def))
		{
			// Increase reference count of shader program.
			p->pProgram->IncRefCount();
			return p->pProgram;
		}
	}

	RHIVertexShader* pVS = m_pDevice->NewVertexShaderFromFile(0, vs_name,
		vs_func, vs_def);
	RHIPixelShader* pPS = m_pDevice->NewPixelShaderFromFile(0, ps_name,
		ps_func, ps_def);
	RHIShaderProgram* pSP = NULL;

	if ((pVS != NULL) && (pPS != NULL))
	{
		size_t index = m_Resources.GetIndex();

		pSP = m_pDevice->NewShaderProgram(index, pVS, pPS);

		if (pSP != NULL)
		{
			// Increase reference count for reuse.
			pSP->IncRefCount();
			m_Resources.Add(pSP);
			
			shader_slice_t* p = K_NEW(shader_slice_t);

			p->sVSFunc = vs_func;
			p->sVSDef = vs_def;
			p->sPSFunc = ps_func;
			p->sPSDef = ps_def;
			p->pProgram = pSP;
			pItem->Slices.PushBack(p);

			if (m_pDevice->GetSaveShader())
			{
				// Update shader binary file.
				this->SaveShaderItem(pItem);
			}
		}
	}

	if (pVS)
	{
		m_pDevice->DeleteVertexShader(pVS);
	}

	if (pPS)
	{
		m_pDevice->DeletePixelShader(pPS);
	}

	return pSP;
}

void RenderService::DeleteShaderItem(shader_item_t* pItem)
{
	Assert(pItem != NULL);

	for (size_t i = 0; i < pItem->Slices.Size(); ++i)
	{
		shader_slice_t* p = pItem->Slices[i];

		p->pProgram->Release();
		K_DELETE(p);
	}

	K_DELETE(pItem);
}

void RenderService::GetShaderBinaryFileName(shader_item_t* pItem, char* name,
	size_t name_size)
{
	//plat_safe_sprintf(name, name_size, "%sshader/bin/vs_%s-ps_%s.sbin",
	//	frame_get_coder_path(), pItem->sVSFileName.CString(),
	//	pItem->sPSFileName.CString());
	plat_safe_sprintf(name, name_size, "%sshader_bin/vs_%s-ps_%s.sbin",
		frame_get_output_path(), pItem->sVSFileName.CString(),
		pItem->sPSFileName.CString());
}

#define SHADER_BINARY_FILE_IDENTITY 'SBIN'

bool RenderService::SaveShaderItem(shader_item_t* pItem)
{
	Assert(pItem != NULL);

	char fname[512];

	this->GetShaderBinaryFileName(pItem, fname, sizeof(fname));

	void* fp = frame_fopen(fname, "wb");

	if (NULL == fp)
	{
		K_LOG("(RenderService::SaveShaderItem)Open file error.");
		K_LOG(fname);
		return false;
	}

	int file_ident = SHADER_BINARY_FILE_IDENTITY;

	if (frame_fwrite(&file_ident, sizeof(file_ident), 1, fp) != 1)
	{
		K_LOG("(RenderService::SaveShaderItem)Write file identity error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	if (frame_fwrite(&pItem->nVSFileTime, sizeof(pItem->nVSFileTime), 1, fp) != 1)
	{
		K_LOG("(RenderService::SaveShaderItem)Write VS file time error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	if (frame_fwrite(&pItem->nPSFileTime, sizeof(pItem->nPSFileTime), 1, fp) != 1)
	{
		K_LOG("(RenderService::SaveShaderItem)Write PS file time error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	unsigned int slice_num = (unsigned int)pItem->Slices.Size();

	if (frame_fwrite(&slice_num, sizeof(slice_num), 1, fp) != 1)
	{
		K_LOG("(RenderService::SaveShaderItem)Write slice number error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	for (unsigned int i = 0; i < slice_num; ++i)
	{
		shader_slice_t* p = pItem->Slices[i];
		char shader_ident[512];

		plat_safe_sprintf(shader_ident, sizeof(shader_ident), "%s,%s,%s,%s",
			p->sVSFunc.CString(), p->sVSDef.CString(), p->sPSFunc.CString(),
			p->sPSDef.CString());

		unsigned int shader_ident_size = (unsigned int)strlen(shader_ident) + 1;

		if (frame_fwrite(&shader_ident_size, sizeof(shader_ident_size), 1, 
			fp) != 1)
		{
			K_LOG("(RenderService::SaveShaderItem)Write identity size error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		if (frame_fwrite(shader_ident, 1, shader_ident_size, fp) != 
			shader_ident_size)
		{
			K_LOG("(RenderService::SaveShaderItem)Write shader identity error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}
		
		if (!p->pProgram->SaveBinary(fp))
		{
			K_LOG("(RenderService::SaveShaderItem)Write program binary error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}
	}

	frame_fclose(fp);
	return true;
}

static bool parse_shader_ident(char* shader_ident, const char** vs_func,
	const char** vs_def, const char** ps_func, const char** ps_def)
{
	char* comma1 = strchr(shader_ident, ',');

	if (NULL == comma1)
	{
		return false;
	}

	char* comma2 = strchr(comma1 + 1, ',');

	if (NULL == comma2)
	{
		return false;
	}

	char* comma3 = strchr(comma2 + 1, ',');

	if (NULL == comma3)
	{
		return false;
	}

	*comma1 = 0;
	*comma2 = 0;
	*comma3 = 0;
	*vs_func = shader_ident;
	*vs_def = comma1 + 1;
	*ps_func = comma2 + 1;
	*ps_def = comma3 + 1;
	return true;
}

bool RenderService::LoadShaderItem(shader_item_t* pItem)
{
	Assert(pItem != NULL);

	char fname[512];

	this->GetShaderBinaryFileName(pItem, fname, sizeof(fname));

	void* fp = frame_fopen(fname, "rb");

	if (NULL == fp)
	{
		return false;
	}

	int file_ident;

	if (frame_fread(&file_ident, sizeof(file_ident), 1, fp) != 1)
	{
		K_LOG("(RenderService::LoadShaderItem)Read binary identity error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	if (file_ident != SHADER_BINARY_FILE_IDENTITY)
	{
		K_LOG("(RenderService::LoadShaderItem)Binary identity error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	uint64_t vs_file_time;
	uint64_t ps_file_time;

	if ((frame_fread(&vs_file_time, sizeof(vs_file_time), 1, fp) != 1) ||
		(frame_fread(&ps_file_time, sizeof(ps_file_time), 1, fp) != 1))
	{
		K_LOG("(RenderService::LoadShaderItem)Read shader file time error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	if ((pItem->nVSFileTime != vs_file_time) ||
		(pItem->nPSFileTime != ps_file_time))
	{
		// Shader binary file time out.
		frame_fclose(fp);
		return false;
	}

	unsigned int slice_num;

	if (frame_fread(&slice_num, sizeof(slice_num), 1, fp) != 1)
	{
		K_LOG("(RenderService::LoadShaderItem)Read program number error.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	if (slice_num > 0x10000)
	{
		K_LOG("(RenderService::LoadShaderItem)Program number too big.");
		K_LOG(fname);
		frame_fclose(fp);
		return false;
	}

	for (unsigned int i = 0; i < slice_num; ++i)
	{
		unsigned int shader_ident_size;

		if (frame_fread(&shader_ident_size, sizeof(shader_ident_size), 1, fp)
			!= 1)
		{
			K_LOG("(RenderService::LoadShaderItem)"
				"Read shader identity size error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		if ((shader_ident_size == 0) || (shader_ident_size > 4096))
		{
			K_LOG("(RenderService::LoadShaderItem)Shader identity size error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		TAutoBuffer<char, 512, FrameAlloc> auto_buf(shader_ident_size);
		char* shader_ident = auto_buf.GetBuffer();

		if (frame_fread(shader_ident, 1, shader_ident_size, fp) !=
			shader_ident_size)
		{
			K_LOG("(RenderService::LoadShaderItem)Read shader identity error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		if (shader_ident[shader_ident_size - 1] != 0)
		{
			K_LOG("(RenderService::LoadShaderItem)Shader identity no end symbol.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		const char* vs_func;
		const char* vs_def;
		const char* ps_func;
		const char* ps_def;

		if (!parse_shader_ident(shader_ident, &vs_func, &vs_def, &ps_func,
			&ps_def))
		{
			K_LOG("(RenderService::LoadShaderItem)Parse binary identity error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		size_t index = m_Resources.GetIndex();
		RHIShaderProgram* pSP = m_pDevice->NewShaderProgramEmpty(index);

		if (NULL == pSP)
		{
			K_LOG("(RenderService::LoadShaderItem)Create shader program error.");
			K_LOG(fname);
			frame_fclose(fp);
			return false;
		}

		if (!pSP->LoadBinary(fp))
		{
			K_LOG("(RenderService::LoadShaderItem)Load shader binary error.");
			K_LOG(fname);
			m_pDevice->DeleteShaderProgram(pSP);
			frame_fclose(fp);
			return false;
		}

		m_Resources.Add(pSP);

		shader_slice_t* p = K_NEW(shader_slice_t);

		p->sVSFunc = vs_func;
		p->sVSDef = vs_def;
		p->sPSFunc = ps_func;
		p->sPSDef = ps_def;
		p->pProgram = pSP;
		pItem->Slices.PushBack(p);
	}

	frame_fclose(fp);
	return true;
}

ITexture* RenderService::LoadTexture(const char* name, bool async, 
	int asset_indicator)
{
	Assert(name != NULL);

	size_t index;

	if (m_TextureIndices.GetData(name, &index))
	{
		Texture* pTex = m_Textures.Get(index);

		pTex->IncRefCount();
		return pTex;
	}

	index = m_Textures.GetIndex();

	Texture* pTexture = Texture::CreateInstance(this, index);

	pTexture->SetName(name);

	if (async)
	{
		pTexture->CreateStart();
		this->AddAssetTask(asset_indicator, 0, pTexture->GetAssetLoader());
	}
	else
	{
		pTexture->CreateSync();
	}

	m_Textures.Add(pTexture);
	m_TextureIndices.Add(name, index);
	return pTexture;
}

void RenderService::ReleaseTexture(Texture* pTexture)
{
	Assert(pTexture != NULL);

	size_t index = pTexture->GetIndex();

	m_Textures.Remove(pTexture, index);
	m_TextureIndices.RemoveData(pTexture->GetName(), index);
	Texture::DeleteInstance(pTexture);
}
