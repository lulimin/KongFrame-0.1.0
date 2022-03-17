// gles3_header.h
// Created by lulimin on 2020/10/21.

#ifndef __GLES3_HEADER_H
#define __GLES3_HEADER_H

#include "../../inc/common.h"

#if defined(K_PLATFORM_IOS) || defined(K_PLATFORM_MAC)
	#include <OpenGLES/ES3/gl.h>
	#include <OpenGLES/ES3/glext.h>
#else
	#include <gles3/gl3.h>
#endif // K_PLATFORM_IOS || K_PLATFORM_MAC

#endif // __GLES3_HEADER_H
