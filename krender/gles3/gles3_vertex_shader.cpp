// gles3_vertex_shader.cpp
// Created by lulimin on 2020/10/15.

#include "gles3_vertex_shader.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"
#include "../../inc/frame_file.h"
#include "../../inc/auto_buffer.h"

// GLES3VertexShader

GLES3VertexShader* GLES3VertexShader::CreateInstance(RenderService* pRS)
{
	GLES3VertexShader* p = (GLES3VertexShader*)K_ALLOC(sizeof(GLES3VertexShader));

	new (p) GLES3VertexShader(pRS);
	return p;
}

void GLES3VertexShader::DeleteInstance(GLES3VertexShader* pInstance)
{
	K_DELETE(pInstance);
}

GLES3VertexShader::GLES3VertexShader(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nShader = 0;
}

GLES3VertexShader::~GLES3VertexShader()
{
	this->DeleteResource();
}

void GLES3VertexShader::Release()
{
	m_pRenderService->ReleaseResource(this);
}

bool GLES3VertexShader::CreateResource(size_t index, const void* data, 
	unsigned int size)
{
	Assert(data != NULL);
	Assert(size > 0);

	m_nIndex = index;
	m_nShader = glCreateShader(GL_VERTEX_SHADER);

//	GLenum err = glGetError();
//
//	if (err != GL_NO_ERROR)
//	{
//		return false;
//	}

	return true;
}

bool compile_shader(GLuint shader, const char* file, const char* def)
{
	Assert(file != NULL);
	Assert(def != NULL);
	
	void* fp = frame_fopen(file, "rb");

	if (NULL == fp)
	{
		return false;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	long file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	TAutoBuffer<char, 2048, FrameAlloc> auto_buf(file_size + 1);
	char* file_data = auto_buf.GetBuffer();

	if (frame_fread(file_data, 1, file_size, fp) != file_size)
	{
		return false;
	}

	frame_fclose(fp);
	file_data[file_size] = 0;

//	const char* src[3] = { 
//		"#version 300 es\n"
//		"#extension GL_EXT_separate_shader_objects : enable\n"
//		"#extension GL_ARB_separate_shader_objects : enable\n", 
//		def, 
//		file_data
//	};
	const char* src[3] = {
		"#version 300 es\n",
		def,
		file_data
	};

	glShaderSource(shader, 3, src, NULL);
	glCompileShader(shader);

	int compile_status = GL_FALSE;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (compile_status != GL_TRUE)
	{
		K_LOG("(compile_shader)Compile error.");

		int info_log_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);

		if (info_log_len > 0)
		{
			TAutoBuffer<char, 256, FrameAlloc> auto_buf1(info_log_len + 1);
			char* info_log = auto_buf1.GetBuffer();
			GLsizei len;

			glGetShaderInfoLog(shader, info_log_len, &len, info_log);
			K_LOG(info_log);
		}

		K_LOG(file);
		return false;
	}

	return true;
}

bool GLES3VertexShader::CreateFromFile(size_t index, const char* file,
	const char* func, const char* def)
{
	Assert(file != NULL);
	Assert(func != NULL);
	Assert(def != NULL);

	m_nIndex = index;
	m_nShader = glCreateShader(GL_VERTEX_SHADER);
	
	if (!compile_shader(m_nShader, file, def))
	{
		return false;
	}
	
	return true;
}

bool GLES3VertexShader::DeleteResource()
{
	if (m_nShader != 0)
	{
		glDeleteShader(m_nShader);
		m_nShader = 0;
	}

	return true;
}
