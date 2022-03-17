// gles3_shader_program.h
// Created by lulimin on 2020/10/15.

#ifndef __GLES3_SHADER_PROGRAM_H
#define __GLES3_SHADER_PROGRAM_H

#include "../../inc/common.h"
#include "../../inc/thread_lock.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// maximum uniform block number
#define UNIFORM_BLOCK_MAX 16
// maximum sampler number in shader
#define SHADER_SAMPLER_MAX 16

// GLES3ShaderProgram

class RenderService;

class GLES3ShaderProgram : public RHIShaderProgram
{
public:
	// Create new instance.
	static GLES3ShaderProgram* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3ShaderProgram* pInstance);

private:
	struct uniform_block_t
	{
		char szBlockName[32];
		int nBlockIndex;
	};
	
	struct sampler_uniform_t
	{
		char szUniformName[32];
		int nUniformIndex;
	};

public:
	GLES3ShaderProgram(RenderService* pRS);
	virtual ~GLES3ShaderProgram();

	virtual void Release();
	// Increase reference count.
	virtual int IncRefCount();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, GLuint vs, GLuint fs);
	bool CreateEmpty(size_t index);
	// Delete resource data.
	bool DeleteResource();

	// Get program identity.
	GLuint GetProgram() const { return m_nProgram; }

	// Set uniform block name.
	virtual bool SetUniformBlockName(int slot, const char* name);
	// Get uniform block index.
	int GetUniformBlockIndex(int slot);

	// Set uniform name of texture sampler.
	virtual bool SetSamplerUniformName(int slot, const char* name);
	// Get uniform index of texture sampler.
	int GetSamplerUniformIndex(int slot);

	// Save shader binary to file.
	virtual bool SaveBinary(void* fp);
	// Load shader binary from file.
	virtual bool LoadBinary(void* fp);

private:
	LockInt m_RefCount;
	RenderService* m_pRenderService;
	size_t m_nIndex;
	GLuint m_nProgram;
	//GLuint m_nVertexShader;
	//GLuint m_nFragmentShader;
	uniform_block_t m_UniformBlocks[UNIFORM_BLOCK_MAX];
	sampler_uniform_t m_SamplerUniforms[SHADER_SAMPLER_MAX];
};

#endif // __GLES3_SHADER_PROGRAM_H
