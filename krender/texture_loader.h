// texture_loader.h
// Created by lulimin on 2020/11/27.

#ifndef __TEXTURE_LOADER_H
#define __TEXTURE_LOADER_H

#include "../kgraphic/i_asset_loader.h"
#include "../inc/frame_types.h"

// TextureLoader

class Texture;

class TextureLoader : public IAssetLoader
{
public:
	static TextureLoader* CreateInstance(Texture* pTexture);

public:
	TextureLoader(Texture* pTexture);
	virtual ~TextureLoader();

	// Delete self while reference count equal zero, like below:
	// void xxxLoader::Release() { if (this->DecRefCount() == 0) delete(this); }
	virtual void Release();
	// Load asset data.
	virtual bool PreLoad(bool async);
	// Create asset.
	virtual bool PostLoad(bool async);
	// Asyncronize load finished.
	virtual bool FinishLoad(int task_type, bool succeed);

	// Set texture full path.
	void SetFullName(const char* name);

private:
	TextureLoader();
	TextureLoader(const TextureLoader&);
	TextureLoader& operator=(const TextureLoader&);

private:
	Texture* m_pTexture;
	FrameString m_sFullName;
	int m_nWidth;
	int m_nHeight;
	int m_nFormat;
	int m_nLevels;
	unsigned char* m_pTexData;
	size_t m_nTexSize;
};

#endif // __TEXTURE_LOADER_H
