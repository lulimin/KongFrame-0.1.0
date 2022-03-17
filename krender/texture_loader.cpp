// texture_loader.cpp
// Created by lulimin on 2020/11/27.

#include "texture_loader.h"
#include "texture.h"
#include "../kgraphic/rhi_resource.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_file.h"
#include "../inc/auto_buffer.h"

static void* image_malloc(size_t size)
{
	return K_ALLOC(size);
}

static void* image_realloc(void* ptr, size_t new_size)
{
	return K_REALLOC(ptr, new_size);
}

static void image_free(void* ptr)
{
	K_FREE(ptr);
}

#define STBI_MALLOC image_malloc
#define STBI_REALLOC image_realloc
#define STBI_FREE image_free
#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb/stb_image.h"

TextureLoader* TextureLoader::CreateInstance(Texture* pTexture)
{
	TextureLoader* p = (TextureLoader*)K_ALLOC(sizeof(TextureLoader));

	new (p) TextureLoader(pTexture);
	return p;
}

TextureLoader::TextureLoader(Texture* pTexture)
{
	Assert(pTexture != NULL);

	m_pTexture = pTexture;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = 0;
	m_nLevels = 0;
	m_pTexData = NULL;
	m_nTexSize = 0;
}

TextureLoader::~TextureLoader()
{
	if (m_pTexData)
	{
		stbi_image_free(m_pTexData);
	}
}

void TextureLoader::Release()
{
	if (this->DecRefCount() == 0)
	{
		K_DELETE(this);
	}
}

void TextureLoader::SetFullName(const char* name)
{
	Assert(name != NULL);

	m_sFullName = name;
}

bool TextureLoader::PreLoad(bool async)
{
	void* fp = frame_fopen(m_sFullName.CString(), "rb");

	if (NULL == fp)
	{
		K_LOG("(TextureLoader::PreLoad)Open file error.");
		K_LOG(m_sFullName.CString());
		return false;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	size_t file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	TAutoBuffer<unsigned char, 2048, FrameAlloc> auto_buf(file_size);
	unsigned char* file_data = auto_buf.GetBuffer();

	if (frame_fread(file_data, 1, file_size, fp) != file_size)
	{
		frame_fclose(fp);
		K_LOG("(TextureLoader::PreLoad)Read file data error.");
		K_LOG(m_sFullName.CString());
		return false;
	}

	frame_fclose(fp);

	int x;
	int y;
	int comp;
	//int res = stbi_info_from_memory(file_data, (int)file_size, &x, &y, &comp);
	stbi_uc* image_data = stbi_load_from_memory(file_data, (int)file_size, &x,
		&y, &comp, 0);

	if (NULL == image_data)
	{
		K_LOG("(TextureLoader::PreLoad)Load image data error.");
		K_LOG(m_sFullName.CString());
		return false;
	}

	if (((x & 0x3) != 0) || ((y & 0x3) != 0))
	{
		stbi_image_free(image_data);
		K_LOG("(TextureLoader::PreLoad)Image size not aligned with 4 bytes.");
		K_LOG(m_sFullName.CString());
		return false;
	}

	if (comp == 3)
	{
		m_nFormat = RHI_FORMAT_R8G8B8;
	}
	else if (comp == 4)
	{
		m_nFormat = RHI_FORMAT_R8G8B8A8;
	}
	else
	{
		stbi_image_free(image_data);
		K_LOG("(TextureLoader::PreLoad)Image format error.");
		K_LOG(m_sFullName.CString());
		return false;
	}

	m_nWidth = x;
	m_nHeight = y;
	m_nLevels = 1;
	m_pTexData = image_data;
	m_nTexSize = x * y * comp;
	return true;
}

bool TextureLoader::PostLoad(bool async)
{
	if (!m_pTexture->BuildAsset(m_nWidth, m_nHeight, m_nFormat, m_nLevels,
		m_pTexData, m_nTexSize))
	{
		return false;
	}
	
	return true;
}

bool TextureLoader::FinishLoad(int task_type, bool succeed)
{
	return m_pTexture->CreateFinish(task_type, succeed);
}
