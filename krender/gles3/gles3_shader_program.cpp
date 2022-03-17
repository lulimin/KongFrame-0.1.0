// gles3_shader_program.cpp
// Created by lulimin on 2020/10/15.

#include "gles3_shader_program.h"
#include "../render_service.h"
#include "../../inc/frame_mem.h"
#include "../../inc/frame_file.h"
#include "../../inc/auto_buffer.h"

// GLES3ShaderProgram

GLES3ShaderProgram* GLES3ShaderProgram::CreateInstance(RenderService* pRS)
{
	GLES3ShaderProgram* p = (GLES3ShaderProgram*)K_ALLOC(
		sizeof(GLES3ShaderProgram));

	new (p) GLES3ShaderProgram(pRS);
	return p;
}

void GLES3ShaderProgram::DeleteInstance(GLES3ShaderProgram* pInstance)
{
	K_DELETE(pInstance);
}

GLES3ShaderProgram::GLES3ShaderProgram(RenderService* pRS)
{
	Assert(pRS != NULL);

	m_RefCount.SetValue(1);
	m_pRenderService = pRS;
	m_nIndex = 0;
	m_nProgram = 0;
	//m_nVertexShader = 0;
	//m_nFragmentShader = 0;
	memset(m_UniformBlocks, 0, sizeof(m_UniformBlocks));
	memset(m_SamplerUniforms, 0, sizeof(m_SamplerUniforms));
}

GLES3ShaderProgram::~GLES3ShaderProgram()
{
	this->DeleteResource();
}

void GLES3ShaderProgram::Release()
{
	if (m_RefCount.Decrement() == 0)
	{
		m_pRenderService->ReleaseResource(this);
	}
}

int GLES3ShaderProgram::IncRefCount()
{
	return m_RefCount.Increment();
}

bool GLES3ShaderProgram::CreateResource(size_t index, GLuint vs, GLuint fs)
{
	m_nIndex = index;
	m_nProgram = glCreateProgram();

	glAttachShader(m_nProgram, vs);
	glAttachShader(m_nProgram, fs);
	glProgramParameteri(m_nProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
		GL_TRUE);
	glLinkProgram(m_nProgram);

	GLint link_status = GL_FALSE;

	glGetProgramiv(m_nProgram, GL_LINK_STATUS, &link_status);

	if (link_status != GL_TRUE)
	{
		K_LOG("(GLES3ShaderProgram::CreateResource)Link error.");

		int info_log_len = 0;

		glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &info_log_len);

		if (info_log_len > 0)
		{
			TAutoBuffer<char, 256, FrameAlloc> auto_buf1(info_log_len + 1);
			char* info_log = auto_buf1.GetBuffer();
			GLsizei len;

			glGetProgramInfoLog(m_nProgram, info_log_len, &len, info_log);
			K_LOG(info_log);
		}

		return false;
	}

	//m_nVertexShader = vs;
	//m_nFragmentShader = fs;
	return true;
}

bool GLES3ShaderProgram::CreateEmpty(size_t index)
{
	m_nIndex = index;
	m_nProgram = glCreateProgram();
	return true;
}

bool GLES3ShaderProgram::DeleteResource()
{
	if (m_nProgram != 0)
	{
		glDeleteProgram(m_nProgram);
		m_nProgram = 0;
	}

	return true;
}

bool GLES3ShaderProgram::SetUniformBlockName(int slot, const char* name)
{
	Assert((slot >= 0) && (slot < UNIFORM_BLOCK_MAX));
	Assert(name != NULL);
	
	if (0 == m_nProgram)
	{
		return false;
	}

	GLuint index = glGetUniformBlockIndex(m_nProgram, name);

	if ((int)index < 0)
	{
		return false;
	}

	util_string_copy(m_UniformBlocks[slot].szBlockName,
		sizeof(m_UniformBlocks[slot].szBlockName), name);
	m_UniformBlocks[slot].nBlockIndex = index;
	return true;
}

int GLES3ShaderProgram::GetUniformBlockIndex(int slot)
{
	Assert((slot >= 0) && (slot < UNIFORM_BLOCK_MAX));

	return m_UniformBlocks[slot].nBlockIndex;
}

bool GLES3ShaderProgram::SetSamplerUniformName(int slot, const char* name)
{
	Assert((slot >= 0) && (slot < SHADER_SAMPLER_MAX));
	Assert(name != NULL);

	if (0 == m_nProgram)
	{
		return false;
	}

	GLint index = glGetUniformLocation(m_nProgram, name);

	if (index < 0)
	{
		return false;
	}
	
	util_string_copy(m_SamplerUniforms[slot].szUniformName,
		sizeof(m_SamplerUniforms[slot].szUniformName), name);
	m_SamplerUniforms[slot].nUniformIndex = index;
	return true;
}

int GLES3ShaderProgram::GetSamplerUniformIndex(int slot)
{
	Assert((slot >= 0) && (slot < SHADER_SAMPLER_MAX));

	return m_SamplerUniforms[slot].nUniformIndex;
}

bool GLES3ShaderProgram::SaveBinary(void* fp)
{
	Assert(fp != NULL);
	Assert(m_nProgram != 0);

	GLsizei bin_len = 0;

	glGetProgramiv(m_nProgram, GL_PROGRAM_BINARY_LENGTH, &bin_len);

	if (bin_len <= 0)
	{
		K_LOG("(GLES3ShaderProgram::SaveBinary)Binary length error.");
		return false;
	}
	
	TAutoBuffer<char, 4096, FrameAlloc> auto_buf(bin_len);
	char* bin_buf = auto_buf.GetBuffer();
	GLenum bin_format = 0;

	glGetProgramBinary(m_nProgram, bin_len, &bin_len, &bin_format,
		bin_buf);

	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		K_LOG("(GLES3ShaderProgram::SaveBinary)Get binary error.");
		return false;
	}

	int save_bin_format = bin_format;
	int save_bin_len = (int)bin_len;

	frame_fwrite(&save_bin_format, sizeof(save_bin_format), 1, fp);
	frame_fwrite(&save_bin_len, sizeof(save_bin_len), 1, fp);
	frame_fwrite(bin_buf, 1, bin_len, fp);
	return true;
}

bool GLES3ShaderProgram::LoadBinary(void* fp)
{
	Assert(fp != NULL);

	int bin_format;

	if (frame_fread(&bin_format, sizeof(bin_format), 1, fp) != 1)
	{
		K_LOG("(GLES3ShaderProgram::LoadBinary)Read format error.");
		return false;
	}

	int bin_len;

	if (frame_fread(&bin_len, sizeof(bin_len), 1, fp) != 1)
	{
		K_LOG("(GLES3ShaderProgram::LoadBinary)Read length error.");
		return false;
	}

	if ((bin_len < 0) || (bin_len > 0x100000))
	{
		K_LOG("(GLES3ShaderProgram::LoadBinary)Length error.");
		return false;
	}

	TAutoBuffer<char, 2048, FrameAlloc> auto_buf(bin_len);
	char* bin_buf = auto_buf.GetBuffer();

	if (frame_fread(bin_buf, 1, bin_len, fp) != bin_len)
	{
		K_LOG("(GLES3ShaderProgram::LoadBinary)Read binary error.");
		return false;
	}

	Assert(m_nProgram != 0);

	glProgramBinary(m_nProgram, bin_format, bin_buf, bin_len);

	GLint link_status = GL_FALSE;

	glGetProgramiv(m_nProgram, GL_LINK_STATUS, &link_status);

	if (link_status != GL_TRUE)
	{
		K_LOG("(GLES3ShaderProgram::LoadBinary)Link error.");

		int info_log_len = 0;

		glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &info_log_len);

		if (info_log_len > 0)
		{
			TAutoBuffer<char, 256, FrameAlloc> auto_buf1(info_log_len + 1);
			char* info_log = auto_buf1.GetBuffer();
			GLsizei len;

			glGetProgramInfoLog(m_nProgram, info_log_len, &len, info_log);
			K_LOG(info_log);
		}

		return false;
	}

	return true;
}
