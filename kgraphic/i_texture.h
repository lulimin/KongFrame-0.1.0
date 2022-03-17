// i_texture.h
// Created by lulimin on 2020/11/27.

#ifndef __I_TEXTURE_H
#define __I_TEXTURE_H

#include "../inc/common.h"
#include "rhi_shader_texture.h"

// ITexture

class ITexture
{
public:
	// Texture types.
	enum TEXTURE_TYPE_ENUM
	{
		TYPE_2D = 1,
		TYPE_CUBE = 2,
		TYPE_VOLUME = 3,
		TYPE_2D_ARRAY = 4,
	};

public:
	virtual ~ITexture() {}
	
	// Release reference.
	virtual void Release() = 0;
	// Get type of texture.
	virtual int GetTextureType() = 0;
	// Get name of texture.
	virtual const char* GetName() = 0;
	// Get width of texture.
	virtual int GetWidth() = 0;
	// Get height of texture.
	virtual int GetHeight() = 0;
	// Get format of texture.
	virtual int GetFormat() = 0;
	// Get mipmap levels of texture.
	virtual int GetLevels() = 0;
	// Whether in ready status.
	virtual bool GetLoadSucceed() = 0;
	// Whether in loading status.
	virtual bool GetLoadEnded() = 0;
	// Get texture handle for render.
	virtual RHIShaderTexture* GetShaderTexture() = 0;
};

#endif // __I_TEXTURE_H
