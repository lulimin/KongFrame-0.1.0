// win_egl.h
// Created by lulimin on 2020/9/25.

#ifndef __WIN_EGL_H
#define __WIN_EGL_H

#include "../../inc/common.h"
#include <egl/egl.h>

struct egl_device_t;

// Create EGL device.
egl_device_t* egl_device_create();
// Delete EGL device.
void egl_device_delete(egl_device_t* pDevice);
// Initialize EGL device.
bool egl_device_initialize(egl_device_t* pDevice, EGLNativeWindowType hwnd);
// Shutdown EGL device.
bool egl_device_shutdown(egl_device_t* pDevice);
// EGL set context.
bool egl_device_set_context(egl_device_t* pDevice);
// EGL present.
bool egl_device_present(egl_device_t* pDevice);

#endif // __WIN_EGL_H
