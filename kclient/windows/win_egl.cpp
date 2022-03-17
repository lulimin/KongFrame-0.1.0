// win_egl.cpp
// Created by lulimin on 2020/9/25.

#include "win_egl.h"

// EGL device.
struct egl_device_t
{
	EGLDisplay eglDisplay;
	EGLConfig eglConfig;
	EGLSurface eglSurface;
	EGLContext eglContext;
	EGLNativeWindowType eglWindow;
	EGLint eglConfigValues[32];
	int nConfigCount;
	HDC hDC;
};

// Create EGL device.
egl_device_t* egl_device_create()
{
	egl_device_t* pDevice = NEW egl_device_t;

	pDevice->eglDisplay = NULL;
	pDevice->eglConfig = NULL;
	pDevice->eglSurface = NULL;
	pDevice->eglContext = NULL;
	pDevice->eglWindow = NULL;
	pDevice->eglConfigValues[0] = EGL_NONE;
	pDevice->nConfigCount = 0;
	pDevice->hDC = NULL;
	return pDevice;
}

// Delete EGL device.
void egl_device_delete(egl_device_t* pDevice)
{
	Assert(pDevice != NULL);
	
	delete pDevice;
}

// Initialize EGL device.
bool egl_device_initialize(egl_device_t* pDevice, EGLNativeWindowType hwnd)
{
	Assert(pDevice != NULL);

	pDevice->eglWindow = hwnd;
	pDevice->hDC = GetDC(hwnd);

	if (NULL == pDevice->hDC)
	{
		return false;
	}

	pDevice->eglDisplay = eglGetDisplay(pDevice->hDC);

	if (pDevice->eglDisplay == EGL_NO_DISPLAY)
	{
		pDevice->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		if (pDevice->eglDisplay == EGL_NO_DISPLAY)
		{
			return false;
		}
	}

	EGLint major_version;
	EGLint minor_version;

	if (!eglInitialize(pDevice->eglDisplay, &major_version, &minor_version))
	{
		return false;
	}

	if (!eglBindAPI(EGL_OPENGL_ES_API))
	{
		return false;
	}

//	EGLint err = eglGetError();
//
//	if (err != EGL_SUCCESS)
//	{
//		return false;
//	}

	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_LEVEL;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = 0;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_SURFACE_TYPE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_WINDOW_BIT;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_RENDERABLE_TYPE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_OPENGL_ES2_BIT;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_NATIVE_RENDERABLE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_FALSE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_BUFFER_SIZE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_DONT_CARE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_DEPTH_SIZE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = 16;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_STENCIL_SIZE;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = 4;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_SAMPLE_BUFFERS;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = 0;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = EGL_SAMPLES;
	pDevice->eglConfigValues[pDevice->nConfigCount++] = 0;
	pDevice->eglConfigValues[pDevice->nConfigCount] = EGL_NONE;
	
	int configs;

	if (!eglChooseConfig(pDevice->eglDisplay, pDevice->eglConfigValues,
		&pDevice->eglConfig, 1, &configs))
	{
		return false;
	}

	if (configs != 1)
	{
		return false;
	}

	pDevice->eglSurface = eglCreateWindowSurface(pDevice->eglDisplay,
		pDevice->eglConfig, pDevice->eglWindow, NULL);

	if (pDevice->eglSurface == EGL_NO_SURFACE)
	{
		//eglGetError();
		pDevice->eglSurface = eglCreateWindowSurface(pDevice->eglSurface,
			pDevice->eglConfig, NULL, NULL);

		if (pDevice->eglSurface == EGL_NO_SURFACE)
		{
			return false;
		}
	}

//	err = eglGetError();
//
//	if (err != EGL_SUCCESS)
//	{
//		return false;
//	}

	// OpenGLES 3.0.
	EGLint context_values[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

	pDevice->eglContext = eglCreateContext(pDevice->eglDisplay,
		pDevice->eglConfig, NULL, context_values);

	if (pDevice->eglContext == EGL_NO_CONTEXT)
	{
		return false;
	}

//	err = eglGetError();
//
//	if (err != EGL_SUCCESS)
//	{
//		return false;
//		// OpenGLES 2.0.
//		context_values[1] = 2;
//		pDevice->eglContext = eglCreateContext(pDevice->eglDisplay,
//			pDevice->eglConfig, NULL, context_values);
//		err = eglGetError();
//
//		if (err != EGL_SUCCESS)
//		{
//			return false;
//		}
//	}

	return true;
}

// Shutdown EGL device.
bool egl_device_shutdown(egl_device_t* pDevice)
{
	Assert(pDevice != NULL);

	eglMakeCurrent(pDevice->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE,
		EGL_NO_CONTEXT);
	eglDestroyContext(pDevice->eglDisplay, pDevice->eglContext);
	eglDestroySurface(pDevice->eglDisplay, pDevice->eglSurface);
	eglReleaseThread();
	eglTerminate(pDevice->eglDisplay);

	if (pDevice->hDC)
	{
		ReleaseDC(pDevice->eglWindow, pDevice->hDC);
	}

	return true;
}

// EGL set context.
bool egl_device_set_context(egl_device_t* pDevice)
{
	Assert(pDevice != NULL);

	return eglMakeCurrent(pDevice->eglDisplay, pDevice->eglSurface, 
		pDevice->eglSurface, pDevice->eglContext) != EGL_FALSE;

//	EGLint err = eglGetError();
//
//	if (err != EGL_SUCCESS)
//	{
//		return false;
//	}
//
//	return true;
}

// EGL present.
bool egl_device_present(egl_device_t* pDevice)
{
	Assert(pDevice != NULL);

	return eglSwapBuffers(pDevice->eglDisplay, 
		pDevice->eglSurface) != EGL_FALSE;

//	EGLint err = eglGetError();
//
//	if (err != EGL_SUCCESS)
//	{
//		return false;
//	}
//	
//	return true;
}
