// gles3_utility.cpp
// Created by lulimin on 2020/10/21.

#include "gles3_utility.h"

// Get last OpenGLES3 error information.
const char* gles3_get_error_string(GLenum err)
{
	//GLenum err = glGetError();

	switch (err)
	{
	case GL_NO_ERROR:
		return "(glGetError)No error.";
	case GL_INVALID_ENUM:
		return "(glGetError)Invalid enum.";
	case GL_INVALID_VALUE:
		return "(glGetError)Invalid value.";
	case GL_INVALID_OPERATION:
		return "(glGetError)Invalid operation.";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "(glGetError)Invalid framebuffer operation.";
	case GL_OUT_OF_MEMORY:
		return "(glGetError)Out of memory.";
	default:
		Assert(0);
		break;
	}

	return "";
}

// Get current frame buffer status information.
const char* gles3_framebuffer_status_string(GLenum status)
{
	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		return "(glCheckFramebufferStatus)Complete.";
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "(glCheckFramebufferStatus)Incomplete attachment.";
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "(glCheckFramebufferStatus)Incomplete missing attachment.";
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		return "(glCheckFramebufferStatus)Incomplete dimensions.";
	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "(glCheckFramebufferStatus)Unsupported.";
	default:
		Assert(0);
		break;
	}

	return "";
}
