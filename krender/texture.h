// texture.h
// Created by lulimin on 2020/11/27.

#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "../inc/common.h"
#include "../inc/frame_types.h"
#include "../inc/thread_lock.h"
#include "../kgraphic/i_texture.h"
#include "../kgraphic/i_asset_loader.h"

// Texture

class TextureLoader;
class RenderService;

class Texture : public ITexture
{
public:
	// Create new instance.
	static Texture* CreateInstance(RenderService* pRenderService, size_t index);
	static void DeleteInstance(Texture* pTexture);

public:
	Texture(RenderService* pRenderService, size_t index);
	virtual ~Texture();

	// Release reference.
	virtual void Release();
	// Get type of texture.
	virtual int GetTextureType();
	// Get name of texture.
	virtual const char* GetName();
	// Get width of texture.
	virtual int GetWidth();
	// Get height of texture.
	virtual int GetHeight();
	// Get format of texture.
	virtual int GetFormat();
	// Get mipmap levels of texture.
	virtual int GetLevels();
	// Whether in ready status.
	virtual bool GetLoadSucceed();
	// Whether in loading status.
	virtual bool GetLoadEnded();
	// Get texture handle for render.
	virtual RHIShaderTexture* GetShaderTexture();

	// Set name of texture.
	void SetName(const char* name);

	size_t GetIndex() const { return m_nIndex; }

	// Get reference count.
	int GetRefCount()
	{
		return m_RefCount.GetValue();
	}

	// Increase reference count.
	int IncRefCount()
	{
		return m_RefCount.Increment();
	}

	// Decrease reference count.
	int DecRefCount()
	{
		return m_RefCount.Decrement();
	}
	
	IAssetLoader* GetAssetLoader() const;

	bool CreateSync();
	bool CreateStart();
	bool CreateFinish(int task_type, bool succeed);
	bool BuildAsset(int width, int height, int format, int levels,
		const unsigned char* tex_data, size_t tex_size);

private:
	Texture();
	Texture(const Texture&);
	Texture& operator=(const Texture&);

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	LockInt m_RefCount;
	FrameString m_sName;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nLevels;
	RHIShaderTexture* m_pShaderTexture;
	TextureLoader* m_pLoader;
};

#endif // __TEXTURE_H
