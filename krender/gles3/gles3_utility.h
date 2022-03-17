// gles3_utility.h
// Created by lulimin on 2020/10/21.

#ifndef __GLES3_UTILITY_H
#define __GLES3_UTILITY_H

#include "../../inc/common.h"
#include "gles3_header.h"

// Get last OpenGLES3 error information.
const char* gles3_get_error_string(GLenum err);
// Get current frame buffer status information.
const char* gles3_framebuffer_status_string(GLenum status);

#ifdef K_DEBUG
#define gles3_check_error_hint(info) \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) { \
		K_LOG(info); \
		K_LOG(gles3_get_error_string(err)); \
	}
#else
#define gles3_check_error_hint(info)
#endif // K_DEBUG

#ifdef K_DEBUG
#define gles3_check_error_return(info) \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) { \
		K_LOG(info); \
		K_LOG(gles3_get_error_string(err)); \
		return; \
	}
#else
#define gles3_check_error_return(info)
#endif // K_DEBUG

#ifdef K_DEBUG
#define gles3_check_error_return_false(info) \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) { \
		K_LOG(info); \
		K_LOG(gles3_get_error_string(err)); \
		return false; \
	}
#else
#define gles3_check_error_return_false(info)
#endif // K_DEBUG

#ifdef K_DEBUG
#define gles3_check_framebuffer_hint(info) \
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
	if (status != GL_FRAMEBUFFER_COMPLETE) { \
		K_LOG(info); \
		K_LOG(gles3_framebuffer_status_string(status)); \
	}
#else
#define gles3_check_framebuffer_hint(info)
#endif // K_DEBUG

#ifdef K_DEBUG
#define gles3_check_framebuffer_return(info) \
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
	if (status != GL_FRAMEBUFFER_COMPLETE) { \
		K_LOG(info); \
		K_LOG(gles3_framebuffer_status_string(status)); \
		return; \
	}
#else
#define gles3_check_framebuffer_return(info)
#endif // K_DEBUG

#ifdef K_DEBUG
#define gles3_check_framebuffer_return_false(info) \
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
	if (status != GL_FRAMEBUFFER_COMPLETE) { \
		K_LOG(info); \
		K_LOG(gles3_framebuffer_status_string(status)); \
		return false; \
	}
#else
#define gles3_check_framebuffer_return_false(info)
#endif // K_DEBUG

#endif // __GLES3_UTILITY_H
