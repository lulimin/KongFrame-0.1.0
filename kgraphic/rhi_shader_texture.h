// rhi_shader_texture.h
// Created by lulimin on 2020/11/27.

#ifndef __RHI_SHADER_TEXTURE_H
#define __RHI_SHADER_TEXTURE_H

#include "../inc/common.h"

// RHIShaderTexture

class RHIShaderTexture
{
public:
	virtual ~RHIShaderTexture() {}

	// Release reference.
	virtual void Release() = 0;
};

#endif // __RHI_SHADER_TEXTURE_H
