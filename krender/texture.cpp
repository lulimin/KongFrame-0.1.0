// texture.cpp
// Created by lulimin on 2020/11/27.

#include "texture.h"
#include "texture_loader.h"
#include "render_service.h"
#include "../kgraphic/rhi_device.h"
#include "../inc/frame_mem.h"

Texture* Texture::CreateInstance(RenderService* pRenderService, size_t index)
{
	Texture* p = (Texture*)K_ALLOC(sizeof(Texture));

	new (p) Texture(pRenderService, index);
	return p;
}

void Texture::DeleteInstance(Texture* pTexture)
{
	K_DELETE(pTexture);
}

Texture::Texture(RenderService* pRenderService, size_t index)
{
	Assert(pRenderService != NULL);

	m_pRenderService = pRenderService;
	m_nIndex = index;
	m_RefCount.SetValue(1);
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nLevels = 0;
	m_pShaderTexture = NULL;
	m_pLoader = NULL;
}

Texture::~Texture()
{
	if (m_pShaderTexture)
	{
		m_pShaderTexture->Release();
	}

	if (m_pLoader)
	{
		m_pLoader->Release();
	}
}

void Texture::Release()
{
	if (this->DecRefCount() == 0)
	{
		m_pRenderService->ReleaseTexture(this);
	}
}

int Texture::GetTextureType()
{
	return ITexture::TYPE_2D;
}

const char* Texture::GetName()
{
	return m_sName.CString();
}

int Texture::GetWidth()
{
	return m_nWidth;
}

int Texture::GetHeight()
{
	return m_nHeight;
}

int Texture::GetFormat()
{
	return m_nFormat;
}

int Texture::GetLevels()
{
	return m_nLevels;
}

bool Texture::GetLoadSucceed()
{
	return m_pShaderTexture != NULL;
}

bool Texture::GetLoadEnded()
{
	return NULL == m_pLoader;
}

RHIShaderTexture* Texture::GetShaderTexture()
{
	return m_pShaderTexture;
}

void Texture::SetName(const char* name)
{
	Assert(name != NULL);

	m_sName = name;
}

IAssetLoader* Texture::GetAssetLoader() const
{
	return m_pLoader;
}

bool Texture::CreateSync()
{
	if (!this->CreateStart())
	{
		return false;
	}

	if (!m_pLoader->PreLoad(false))
	{
		return false;
	}

	if (!m_pLoader->PostLoad(false))
	{
		return false;
	}

	if (!this->CreateFinish(0, true))
	{
		return false;
	}
	
	return true;
}

bool Texture::CreateStart()
{
	Assert(NULL == m_pLoader);

	char fname[512];

	plat_safe_sprintf(fname, sizeof(fname), "%s%s", frame_get_asset_path(),
		m_sName.CString());
	m_pLoader = TextureLoader::CreateInstance(this);
	m_pLoader->SetFullName(fname);
	return true;
}

bool Texture::CreateFinish(int task_type, bool succeed)
{
	SAFE_RELEASE(m_pLoader);
	return true;
}

bool Texture::BuildAsset(int width, int height, int format, int levels,
	const unsigned char* tex_data, size_t tex_size)
{
	Assert(width > 0);
	Assert(height > 0);
	Assert(levels > 0);
	Assert(tex_data != NULL);
	Assert(tex_size > 0);

	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;
	m_nLevels = levels;

	RHIDevice* pDevice = m_pRenderService->GetRenderDevice();

	m_pShaderTexture = pDevice->NewShaderTexture(width, height, format, levels,
		tex_data, tex_size);
	return true;
}
