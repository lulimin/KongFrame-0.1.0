// gles3_pixel_shader.cpp
// Created by lulimin on 2020/10/15.

#include "gles3_pixel_shader.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"

// GLES3PixelShader

GLES3PixelShader* GLES3PixelShader::CreateInstance(RenderService* pRS)
{
	GLES3PixelShader* p = (GLES3PixelShader*)K_ALLOC(sizeof(GLES3PixelShader));

	new (p) GLES3PixelShader(pRS);
	return p;
}

void GLES3PixelShader::DeleteInstance(GLES3PixelShader* pInstance)
{
	K_DELETE(pInstance);
}

GLES3PixelShader::GLES3PixelShader(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nShader = 0;
}

GLES3PixelShader::~GLES3PixelShader()
{
	this->DeleteResource();
}

void GLES3PixelShader::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3PixelShader::CreateResource(size_t index, const void* data,
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	m_nIndex = index;

	m_nShader = glCreateShader(GL_FRAGMENT_SHADER);

//	GLenum err = glGetError();
//
//	if (err != GL_NO_ERROR)
//	{
//		return false;
//	}

	return true;
}

bool compile_shader(GLuint shader, const char* file, const char* def);

bool GLES3PixelShader::CreateFromFile(size_t index, const char* file,
	const char* func, const char* def)
{
	Assert(file != NULL);
	Assert(func != NULL);
	Assert(def != NULL);

	m_nIndex = index;
	m_nShader = glCreateShader(GL_FRAGMENT_SHADER);

	if (!compile_shader(m_nShader, file, def))
	{
		return false;
	}

	return true;
}

bool GLES3PixelShader::DeleteResource()
{
	if (m_nShader != 0)
	{
		glDeleteShader(m_nShader);
		m_nShader = 0;
	}

	return true;
}
