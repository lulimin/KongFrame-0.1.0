// gles3_vertex_shader.h
// Created by lulimin on 2020/10/15.

#ifndef __GLES3_VERTEX_SHADER_H
#define __GLES3_VERTEX_SHADER_H

#include "../../inc/common.h"
#include "../../kgraphic/rhi_resource.h"
#include "gles3_header.h"

// GLES3VertexShader

class RenderService;

class GLES3VertexShader : public RHIVertexShader
{
public:
	// Create new instance.
	static GLES3VertexShader* CreateInstance(RenderService* pRS);
	// Delete instance.
	static void DeleteInstance(GLES3VertexShader* pInstance);

public:
	GLES3VertexShader(RenderService* pRS);
	virtual ~GLES3VertexShader();

	virtual void Release();
	virtual size_t GetIndex() { return m_nIndex; }

	// Create resource data.
	bool CreateResource(size_t index, const void* data, unsigned int size);
	bool CreateFromFile(size_t index, const char* file, const char* func,
		const char* def);
	// Delete resource data.
	bool DeleteResource();

	// Get shader identity.
	GLuint GetShader() const { return m_nShader; }

private:
	RenderService* m_pRenderService;
	size_t m_nIndex;
	GLuint m_nShader;
};

#endif // __GLES3_VERTEX_SHADER_H
