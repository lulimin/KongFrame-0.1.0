/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <jni.h>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include "android_native_app_glue.h"
#include "android_file_access.h"
#include "../../inc/common.h"
#include "../../inc/frame_def.h"
#include "../../inc/auto_string.h"
#include "../../inc/arg_data.h"
#include "../../inc/arg_list.h"
#include "../../inc/utility.h"
#include "../../inc/auto_buffer.h"
#include "../../inc/time_counter.h"
#include "../../inc/platform.h"
#include "../../inc/thread_lock.h"
#include "../../kgraphic/i_input_service.h"
#include "../../kgraphic/i_render_service.h"
#include "../memory_link.h"
#include "../memory_list.h"
#include "../read_json.h"
#include "../link_creator.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

DEFINE_ASSERTION_FAIL;

// Local data types.
typedef TAutoString<char, 128> TempString;
typedef TArgList<8, 128> TempArgList;

// Our saved state data.
struct saved_state_t
{
};

// Shared state for our app.
struct engine_t
{
    struct android_app* pApp;
    ASensorManager* pSensorManager;
    const ASensor* pAccelerometerSensor;
    ASensorEventQueue* pSensorEventQueue;
    AAssetManager* pAssetManager;
    ANativeWindow* pNativeWindow;
    struct saved_state_t SavedState;
    EGLDisplay hDisplay;
    EGLContext hContext;
    EGLSurface hSurface;
    EGLConfig hConfig;
    int32_t nWidth;
    int32_t nHeight;
	bool bCreated;
    bool bResumed;
	bool bFocused;
	bool bSurfaceReady;
    double dActionUpTime;
    // Android assets file access.
    AndroidFileAccess* pFileAccess;
    // User input router.
    IInputService* pInputService;
    // Render service.
    IRenderService* pRenderService;
    // Sleep milliseconds.
    int nSleep;
};

// Set display context function.
//static bool egl_set_context(void* param)
//{
//	struct engine* engine = (struct engine*)param;
//
//    if (engine->hDisplay == EGL_NO_DISPLAY)
//    {
//        return false;
//    }
//
//    return eglMakeCurrent(engine->hDisplay, engine->hSurface,
//        engine->hSurface, engine->hContext) != EGL_FALSE;
//}

// Present per frame function.
static bool egl_present(void* param)
{
	struct engine_t* engine = (struct engine_t*)param;
    
    if (engine->hDisplay == EGL_NO_DISPLAY)
    {
        return false;
    }

    return eglSwapBuffers(engine->hDisplay, engine->hSurface) != EGL_FALSE;
}

// Set current native window.
static bool engine_set_window(struct engine_t* engine, ANativeWindow* window)
{
	Assert(engine != NULL);

	if (window)
	{
		EGLDisplay display = engine->hDisplay;
		EGLContext context = engine->hContext;
		EGLConfig config = engine->hConfig;

		// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		// As soon as we picked a EGLConfig, we can safely reconfigure the
		// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
		EGLint format;

		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		if (ANativeWindow_setBuffersGeometry(window, 0, 0, format) != 0)
		{
			LOGW("Unable to ANativeWindow_setBufferGeometry.");
			return false;
		}

		EGLSurface surface = eglCreateWindowSurface(display, config, window,
			nullptr);

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
		{
			LOGW("Unable to eglMakeCurrent.");
			return false;
		}

		EGLint w;
		EGLint h;

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);
		engine->hSurface = surface;
		engine->nWidth = w;
		engine->nHeight = h;
	}
	else
	{
		if (engine->hDisplay != EGL_NO_DISPLAY)
		{
			eglMakeCurrent(engine->hDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE,
				EGL_NO_CONTEXT);

			if (engine->hSurface != EGL_NO_SURFACE)
			{
				eglDestroySurface(engine->hDisplay, engine->hSurface);
			}
		}

		engine->hSurface = EGL_NO_SURFACE;
	}

	return true;
}

// Initialize an EGL context for the current display.
static bool engine_init_display(struct engine_t* engine)
{
    Assert(engine != NULL);

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major;
    EGLint minor;

    eglInitialize(display, &major, &minor);

	// Here specify the attributes of the desired configuration.
	// Below, we select an EGLConfig with at least 8 bits per color
	// component compatible with on-screen windows
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

    // Here, the application chooses the configuration it desires.
    // find the best match if possible, otherwise use the very first one
	EGLint numConfigs;

    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    if (numConfigs == 0)
	{
		LOGW("Invalidate EGL configuration number.");
    	return false;
	}

    TAutoBuffer<EGLConfig, 16> auto_buf(numConfigs);
    EGLConfig* supportedConfigs = auto_buf.GetBuffer();

	eglChooseConfig(display, attribs, supportedConfigs, numConfigs, &numConfigs);

	EGLConfig config = nullptr;
	int i = 0;

    for (; i < numConfigs; i++)
    {
        EGLConfig& cfg = supportedConfigs[i];
        EGLint r, g, b, d, s;
        EGLint rtype;

        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
			eglGetConfigAttrib(display, cfg, EGL_STENCIL_SIZE, &s) &&
			eglGetConfigAttrib(display, cfg, EGL_RENDERABLE_TYPE, &rtype) &&
            (r == 8) && (g == 8) && (b == 8) && (d == 24) && (s == 8) &&
            ((rtype & EGL_OPENGL_ES3_BIT) != 0))
        {
            config = supportedConfigs[i];
            break;
        }
    }
    
    if (i == numConfigs)
    {
        config = supportedConfigs[0];
    }

    if (config == nullptr)
    {
        LOGW("Unable to initialize EGLConfig.");
        return false;
    }

    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    EGLContext context = eglCreateContext(display, config, nullptr,
        context_attribs);

    if (context == EGL_NO_CONTEXT)
    {
        context_attribs[1] = 2;
        context = eglCreateContext(display, config, nullptr, context_attribs);

        if (context == EGL_NO_CONTEXT)
        {
            LOGW("Unable to eglCreateContext.");
            return false;
        }
    }

    engine->hDisplay = display;
	engine->hContext = context;
	engine->hConfig = config;
	return engine_set_window(engine, engine->pApp->window);
}

// Tear down the EGL context currently associated with the display.
static bool engine_term_display(struct engine_t* engine)
{
    Assert(engine != NULL);

    if (engine->hDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(engine->hDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT);

        if (engine->hContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(engine->hDisplay, engine->hContext);
        }

        if (engine->hSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(engine->hDisplay, engine->hSurface);
        }

        eglTerminate(engine->hDisplay);
    }

    engine->hDisplay = EGL_NO_DISPLAY;
    engine->hContext = EGL_NO_CONTEXT;
    engine->hSurface = EGL_NO_SURFACE;
    engine->hConfig = 0;
    return true;
}

// Display fatal error message.
static void show_fatal_error(const char* info)
{
	Assert(info != NULL);

    LOGW("(KongFrame)fatal error:%s", info);
	//plat_popup_error(info, "fatal error");
}

// Retrieve full path and validate it.
static const char* parse_full_path(IFileAccess* pFileAccess,
   	const char* src_path, const char* def_path, char* buffer,
   	size_t buffer_size)
{
	Assert(pFileAccess != NULL);
	Assert(src_path != NULL);
	Assert(def_path != NULL);
	Assert(buffer != NULL);

	if (util_string_empty(src_path))
	{
		return def_path;
	}

	util_string_copy(buffer, buffer_size - 1, src_path);

	if (!pFileAccess->DirectoryExists(buffer))
	{
		char info[256];

		plat_safe_sprintf(info, sizeof(info),
			"(parse_full_path)Path %s not found.", buffer);
		show_fatal_error(info);
		abort();
	}

	char* p = buffer + strlen(buffer);

	p[0] = PLAT_PATH_SEPARATOR;
	p[1] = 0;
	return buffer;
}

// Custom log function.
static bool android_frame_log(void* context, int level, const char* info)
{
    LOGI("(KongFrame)[%d]%s", level, info);
    return true;
}

// Create system frame.
static void create_frame(struct engine_t* engine)
{
	Assert(engine != NULL);
    
    // Current work path.
	TempString work_path = "";

	if (work_path.Empty())
	{
		char path[256];

		plat_get_current_dir(path, sizeof(path));

		if ((path[0] != 0) && (path[strlen(path) - 1] != '/'))
		{
			work_path = TempString(path) + "/";
		}
		else
        {
		    work_path = path;
        }
	}

	// Get file name of configuration.
	TempString json_name = "asset/config/kclient_android.json";
	// Read configuration file.
    IFileAccess* pFileAccess = engine->pFileAccess;
	void* fp = pFileAccess->FileOpen(json_name.CString(), "rb");

	if (NULL == fp)
	{
		show_fatal_error("Open json file failed.");
		abort();
	}

	pFileAccess->FileSeek(fp, 0, SEEK_END);
	size_t json_size = pFileAccess->FileTell(fp);
	pFileAccess->FileSeek(fp, 0, SEEK_SET);

	TAutoBuffer<char, 1024> auto_buf(json_size);
	char* json_data = auto_buf.GetBuffer();

	if (pFileAccess->FileRead(fp, json_data, json_size) != json_size)
	{
		pFileAccess->FileClose(fp);
		show_fatal_error("Read json file error.");
		abort();
	}

	pFileAccess->FileClose(fp);

	TAutoBuffer<size_t, 1024> auto_buf1(json_size);
	size_t* pJsonMem = auto_buf1.GetBuffer();
	const sajson::document& doc = sajson::parse(
		sajson::bounded_allocation(pJsonMem, json_size),
		sajson::mutable_string_view(json_size, json_data));

	if (!doc.is_valid())
	{
		show_fatal_error("Parse json file error.");
		abort();
	}

	const sajson::value& root = doc.get_root();

	// Load paths.
	char script_path[256];
	char coder_path[256];
	char asset_path[256];
    char output_path[256];
	TempString s1 = read_json_string(root, "ScriptPath");
	TempString s2 = read_json_string(root, "CoderPath");
	TempString s3 = read_json_string(root, "AssetPath");
    TempString s4 = read_json_string(root, "OutputPath");
	frame_preset_t preset;

	preset.szWorkPath = work_path.CString();
	preset.szScriptPath = parse_full_path(pFileAccess, s1.CString(),
	    work_path.CString(), script_path, sizeof(script_path));
	preset.szCoderPath = parse_full_path(pFileAccess, s2.CString(),
	    work_path.CString(), coder_path, sizeof(coder_path));
	preset.szAssetPath = parse_full_path(pFileAccess, s3.CString(),
	    work_path.CString(), asset_path, sizeof(asset_path));

	if (s4.Empty())
	{
		plat_safe_sprintf(output_path, sizeof(output_path), "%s/",
			engine->pApp->activity->internalDataPath);
	}
	else
	{
		plat_safe_sprintf(output_path, sizeof(output_path), "%s/%s/",
		    engine->pApp->activity->internalDataPath, s4.CString());
	}

    preset.szOutputPath = output_path;

	// Component configuration.
	IServiceCreator* pSvcCreator = NULL;
	IObjectCreator* pObjCreator = NULL;

	link_creators(&pSvcCreator, &pObjCreator);

	// Name of main object.
	preset.szMainObject = read_json_string(root, "MainObject");
	// Interface to file system.
	preset.pFileAccess = engine->pFileAccess;
	// Service creators.
	preset.pServiceCreator = pSvcCreator;
	// Object creators.
	preset.pObjectCreator = pObjCreator;
	// Reload script when file changed.
	preset.bScriptReload = read_json_boolean(root, "ScriptReload");
	// Profile script performance.
	preset.nScriptProfile = 0;

	// Sleep time per frame.
	engine->nSleep = read_json_int32(root, "Sleep");

	// memory validate
	int memory_check = read_json_int32(root, "MemoryCheck");

	if (memory_check == 1)
	{
		memory_link_init();
		// Create frame.
		frame_create(memory_link_debug_alloc, memory_link_alloc,
			memory_link_realloc, memory_link_free, memory_link_dump);
	}
	else if (memory_check == 2)
	{
		memory_list_init();
		// Create frame.
		frame_create(memory_list_debug_alloc, memory_list_alloc,
			memory_list_realloc, memory_list_free, memory_list_dump);
	}
	else
	{
		// Create frame.
		frame_create(NULL, NULL, NULL, NULL, NULL);
	}

    // Redirect log to custom function.
	frame_set_log_func(android_frame_log, NULL);

	// Start frame.
	if (!frame_startup(preset))
	{
		show_fatal_error("frame startup failed");
		abort();
	}

	// Get user input service.
	engine->pInputService = (IInputService*)frame_get_service("FlatInputService");
	
	if (NULL == engine->pInputService)
	{
		show_fatal_error("no input service");
		abort();
	}

	// Get render service.
	engine->pRenderService = (IRenderService*)frame_get_service("RenderService");

	if (engine->pRenderService)
	{
		engine->pRenderService->SetWindowSize(engine->nWidth, engine->nHeight);
		engine->pRenderService->SetEglFunction(egl_present, engine);
		engine->pRenderService->SetSafeArea(0, 80, 0, 0);
	}
	else
	{
		show_fatal_error("No render service.");
		abort();
	}

	// Name of main script.
	const char* main_script = read_json_string(root, "MainScript");

	// Execute main script.
	if (!util_string_empty(main_script))
	{
		IObject* pMainObj = frame_get_main_object();

		if (NULL == pMainObj)
		{
			show_fatal_error("No main object.");
			abort();
		}
		
		bool succeed = frame_run_routine(main_script, "main",
			pMainObj->GetUID(), TempArgList(), NULL); //args, NULL);

		if (!succeed)
		{
			show_fatal_error("Run main script failed.");
			abort();
		}
	}

    engine->bCreated = true;
}

// Delete system frame.
static void delete_frame(struct engine_t* engine)
{
	Assert(engine != NULL);
    
    if (engine->pInputService)
	{
		frame_release_service(engine->pInputService);
		engine->pInputService = NULL;
	}

	if (engine->pRenderService)
	{
		frame_release_service(engine->pRenderService);
		engine->pRenderService = NULL;
	}

	// Shutdown frame.
	frame_shutdown();
	// Delete frame.
	frame_delete();
	// Record memory leaking.
	memory_list_dump("memory_list_leak.log");
	memory_list_shut();
	memory_link_dump("memory_link_leak.log");
	memory_link_shut();
	// File access interface already be deleted.
	engine->pFileAccess = NULL;
    engine->bCreated = false;
}

// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
    Assert(app != NULL);

    struct engine_t* engine = (struct engine_t*)app->userData;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
		int32_t action = AMotionEvent_getAction(event);
		int32_t pointer_index = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		int32_t pointer_id = AMotionEvent_getPointerId(event, pointer_index);
		int x = (int)AMotionEvent_getX(event, pointer_index);
		int y = (int)AMotionEvent_getY(event, pointer_index);

		switch (action & AMOTION_EVENT_ACTION_MASK)
		{
		case AMOTION_EVENT_ACTION_DOWN:
		{
			if (pointer_id == 0)
			{
			    IInputService::input_t msg;
                double cur_time = plat_get_time_count();

                if ((cur_time - engine->dActionUpTime) < 0.3F)
                {
			        msg.nType = IInputService::TYPE_LEFT_DOUBLE;
                }
                else
                {
			        msg.nType = IInputService::TYPE_LEFT_DOWN;
                }
			    
			    msg.dPosX = (double)x;
			    msg.dPosY = (double)y;
			    msg.nFlags = 0;
                engine->pInputService->ProcessInput(msg);
			}

			break;
		}
		case AMOTION_EVENT_ACTION_UP:
		{
			if (pointer_id == 0)
			{
				engine->dActionUpTime = plat_get_time_count();

			    IInputService::input_t msg;

			    msg.nType = IInputService::TYPE_LEFT_UP;
			    msg.dPosX = (double)x;
			    msg.dPosY = (double)y;
			    msg.nFlags = 0;
			    engine->pInputService->ProcessInput(msg);
			}

			break;
		}
		case AMOTION_EVENT_ACTION_MOVE:
		{
			if (pointer_id == 0)
			{
			    IInputService::input_t msg;

			    msg.nType = IInputService::TYPE_MOUSE_MOVE;
			    msg.dPosX = (double)x;
			    msg.dPosY = (double)y;
			    msg.nFlags = IInputService::FLAG_LEFT;
			    engine->pInputService->ProcessInput(msg);
			}
			
			break;
		}
		case AMOTION_EVENT_ACTION_POINTER_DOWN:
        {
			break;
        }
		case AMOTION_EVENT_ACTION_POINTER_UP:
        {
			break;
        }
		default:
			break;
		}

		return 1;
    }

    return 0;
}

// Process the next main command.
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
    Assert(app != NULL);

    struct engine_t* engine = (struct engine_t*)app->userData;

    switch (cmd)
    {
    case APP_CMD_SAVE_STATE:
    {
        // The system has asked us to save our current state.  Do so.
        app->savedState = malloc(sizeof(struct saved_state_t));
        *(struct saved_state_t*)app->savedState = engine->SavedState;
        app->savedStateSize = sizeof(struct saved_state_t);
        break;
    }
	case APP_CMD_RESUME:
    {
		engine->bResumed = true;
		break;
    }
	case APP_CMD_PAUSE:
    {
		engine->bResumed = false;
		break;
    }
    case APP_CMD_INIT_WINDOW:
    {
    	// The window is being shown, get it ready.
        if (engine->pApp->window != nullptr)
        {
            if (NULL == engine->pNativeWindow)
            {
                engine_init_display(engine);
            }
            else
            {
                engine_set_window(engine, engine->pApp->window);
            }

            engine->pNativeWindow = engine->pApp->window;
            engine->bSurfaceReady = true;
        }

        if (!engine->bCreated)
        {
            create_frame(engine);
        }
        
        break;
    }
    case APP_CMD_TERM_WINDOW:
    {
        // The window is being hidden or closed, clean it up.
        if (engine->bSurfaceReady)
        {
			//engine_term_display(engine);
			engine_set_window(engine, NULL);
		    engine->bSurfaceReady = false;
        }

        break;
    }
	case APP_CMD_DESTROY:
    {
		delete_frame(engine);
        engine_term_display(engine);
		break;
    }
    case APP_CMD_GAINED_FOCUS:
    {
        // When our app gains focus, we start monitoring the accelerometer.
        if (engine->pAccelerometerSensor != nullptr)
        {
            ASensorEventQueue_enableSensor(engine->pSensorEventQueue,
                engine->pAccelerometerSensor);
            // We'd like to get 60 events per second (in us).
            ASensorEventQueue_setEventRate(engine->pSensorEventQueue,
                engine->pAccelerometerSensor, (1000L / 60) * 1000);
        }
        
		engine->bFocused = true;
        break;
    }
    case APP_CMD_LOST_FOCUS:
    {
        // When our app loses focus, we stop monitoring the accelerometer.
        // This is to avoid consuming battery while not being used.
        if (engine->pAccelerometerSensor != nullptr)
        {
            ASensorEventQueue_disableSensor(engine->pSensorEventQueue,
                engine->pAccelerometerSensor);
        }
     
		engine->bFocused = false;
        break;
    }
    default:
        break;
    }
}

// AcquireASensorManagerInstance(void)
//    Workaround ASensorManager_getInstance() deprecation false alarm
//    for Android-N and before, when compiling with NDK-r15
#include <dlfcn.h>

ASensorManager* AcquireASensorManagerInstance(android_app* app)
{
    if (!app)
    {
        return nullptr;
    }

    typedef ASensorManager*(*PF_GETINSTANCEFORPACKAGE)(const char* name);
    
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc = 
        (PF_GETINSTANCEFORPACKAGE)dlsym(androidHandle,
		"ASensorManager_getInstanceForPackage");

    if (getInstanceForPackageFunc)
    {
        JNIEnv* env = nullptr;

        app->activity->vm->AttachCurrentThread(&env, nullptr);

        jclass android_content_Context = env->GetObjectClass(
        	app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(
        	android_content_Context, "getPackageName", "()Ljava/lang/String;");
        jstring packageName= (jstring)env->CallObjectMethod(
        	app->activity->clazz, midGetPackageName);

        const char* nativePackageName = env->GetStringUTFChars(packageName,
			nullptr);
        ASensorManager* mgr = getInstanceForPackageFunc(nativePackageName);

        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();

        if (mgr)
        {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager*(*PF_GETINSTANCE)();

    PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE)dlsym(androidHandle,
        "ASensorManager_getInstance");

    // by all means at this point, ASensorManager_getInstance should be available
    Assert(getInstanceFunc);
    dlclose(androidHandle);
    return getInstanceFunc();
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main(struct android_app* app)
{
    Assert(app != NULL);

    //LOGI("app started");

    struct engine_t engine {};

    memset(&engine, 0, sizeof(engine));
    app->userData = &engine;
    app->onAppCmd = engine_handle_cmd;
    app->onInputEvent = engine_handle_input;
    engine.pApp = app;
    // Prepare to monitor accelerometer
    engine.pSensorManager = AcquireASensorManagerInstance(app);
    engine.pAccelerometerSensor = ASensorManager_getDefaultSensor(
        engine.pSensorManager, ASENSOR_TYPE_ACCELEROMETER);
    engine.pSensorEventQueue = ASensorManager_createEventQueue(
        engine.pSensorManager, app->looper, LOOPER_ID_USER, nullptr, nullptr);
    engine.pAssetManager = app->activity->assetManager;
    engine.pFileAccess = AndroidFileAccess::CreateInstance(engine.pAssetManager,
        app->activity->internalDataPath);

    if (app->savedState != nullptr)
    {
        // We are starting with a previous saved state; restore from it.
        if (app->savedStateSize == sizeof(engine.SavedState))
        {
			engine.SavedState = *(struct saved_state_t*)app->savedState;
		}

        free(app->savedState);
        app->savedState = nullptr;
        app->savedStateSize = 0;
    }

	// Create system frame.
	//create_frame(&engine);

	// Main timer.
	TimeCounter timer;

	timer.SetHighPrecision(true);
	timer.Create();

    // loop waiting for stuff to do.
    while (true)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(
            (engine.bCreated && engine.bResumed && engine.bFocused) ? 0 : -1,
            nullptr, &events, (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != nullptr)
            {
                source->process(app, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER)
            {
                if (engine.pAccelerometerSensor != nullptr)
                {
                    ASensorEvent event;

                    while (ASensorEventQueue_getEvents(engine.pSensorEventQueue,
                        &event, 1) > 0)
                    {
                        LOGI("Accelerometer: x=%f y=%f z=%f.",
                             event.acceleration.x, event.acceleration.y,
                             event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (app->destroyRequested != 0)
            {
                if (engine.bCreated)
                {
					delete_frame(&engine);
				}

				engine_term_display(&engine);
                return;
            }
        }

        if (engine.bCreated && engine.bResumed && engine.bFocused &&
        	engine.bSurfaceReady)
        {
            if (frame_get_exit())
            {
				delete_frame(&engine);
                engine_term_display(&engine);
                break;
            }

            frame_period_set_time(timer.GetElapseSeconds());
            frame_period_check_routine();
            frame_period_work();

            if (engine.nSleep >= 0)
            {
                plat_sleep(engine.nSleep);
            }
        }
    }

	//delete_frame(&engine);
}
//END_INCLUDE(all)
