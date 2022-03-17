#import "ViewController.h"

#include "../../inc/common.h"
#include "../../inc/frame_def.h"
#include "../../inc/auto_string.h"
#include "../../inc/arg_data.h"
#include "../../inc/arg_list.h"
#include "../../inc/utility.h"
#include "../../inc/platform.h"
#include "../../inc/auto_buffer.h"
#include "../../inc/time_counter.h"
#include "../../kgraphic/i_input_service.h"
#include "../../kgraphic/i_render_service.h"
#include "../memory_link.h"
#include "../memory_list.h"
#include "../read_json.h"
#include "../link_creator.h"

DEFINE_ASSERTION_FAIL;

// Local data types.
typedef TAutoString<char, 128> TempString;
typedef TArgList<8, 128> TempArgList;

static ViewController* s_pController = nil;

@interface ViewController () {
}

@property (strong, nonatomic) EAGLContext* context;

@end

// Display fatal error message.
static void show_fatal_error(const char* info)
{
	Assert(info != NULL);

    NSLog(@"(KongFrame)fatal error:%s", info);
}

// Retrieve full path and validate it.
static const char* parse_full_path(const char* src_path,
	const char* work_path, char* buffer, size_t buffer_size)
{
	Assert(src_path != NULL);
	Assert(work_path != NULL);
	Assert(buffer != NULL);

	if (util_string_empty(src_path))
	{
		return work_path;
	}

	util_string_copy(buffer, buffer_size - 1, work_path);

	size_t work_path_len = strlen(buffer);

	if (src_path[0] == '/')
	{
		util_string_copy(buffer + work_path_len, 
			buffer_size - work_path_len - 1, src_path + 1);
	}
	else
	{
		util_string_copy(buffer + work_path_len,
			buffer_size - work_path_len - 1, src_path);
	}

	if (!plat_find_dir(buffer))
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
static bool ios_frame_log(void* context, int level, const char* info)
{
    NSLog(@"(KongFrame)[%d]%s", level, info);
    return true;
}

@implementation ViewController
{
    GLKView* m_pGLKView;
    // Main game timer.
    TimeCounter m_Timer;
    // User input router.
    IInputService* m_pInputService;
    // Render service.
    IRenderService* m_pRenderService;
	// Name of main script.
	TempString m_sMainScript;
    // Sleep milliseconds.
    int m_nSleep;
    // Action up time.
    double m_dActionUpTime;
	// In ready status.
	bool m_bReady;
}

+ (ViewController*)getLastInstance
{
    return s_pController;
}

- (id)init
{
    m_pGLKView = NULL;
    m_pInputService = NULL;
    m_pRenderService = NULL;
    m_nSleep = 0;
    m_dActionUpTime = 0.0;
	m_bReady = false;
    s_pController = [super init];
    return s_pController;
}

- (void)dealloc
{
    s_pController = nil;
    
    if ([EAGLContext currentContext] == self.context)
    {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)loadView
{
    CGRect bounds = [UIScreen mainScreen].bounds;

    self.view = [[GLKView alloc] initWithFrame:bounds];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft) || 
        (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
	// Dispose of any resources that can be recreated.
}

- (void)viewDidLoad
{
    [super viewDidLoad];
  
    NSLog(@"viewDidLoad");

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

    if (!self.context)
    {
    	//self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        NSLog(@"Failed to create OpenGLES context.");
    }

    GLKView* view = (GLKView*)self.view;
    
    view.context = self.context;
    view.backgroundColor = UIColor.blackColor;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    m_pGLKView = view;

	// Set FPS.
	//self.preferredFramesPerSecond = 60;

    [EAGLContext setCurrentContext:self.context];

    CGFloat screenScale = [[UIScreen mainScreen] scale];
    CGRect bounds = [[UIScreen mainScreen] bounds];

    printf("screenScale:%f\n", screenScale);
    printf("bounds x:%f y:%f w:%f h:%f\n", bounds.origin.x, bounds.origin.y,
    	bounds.size.width, bounds.size.height);
  
	// Get bundle directory.
    NSString* appFolderPath = [[NSBundle mainBundle] resourcePath];
    const char* folder = [appFolderPath UTF8String];
    // Current work path.
	TempString work_path = TempString(folder) + "/";

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
	TempString json_name = work_path + "asset/config/kclient_ios.json";
	// Read configuration file.
	FILE* fp = plat_file_open(json_name.CString(), "rb");

	if (NULL == fp)
	{
		show_fatal_error("Open json file failed.");
		abort();
	}

	fseek(fp, 0, SEEK_END);
	size_t json_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	TAutoBuffer<char, 1024> auto_buf(json_size);
	char* json_data = auto_buf.GetBuffer();

	if (fread(json_data, 1, json_size, fp) != json_size)
	{
		fclose(fp);
		show_fatal_error("Read json file error.");
		abort();
	}

	fclose(fp);

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
	char script_path[512];
	char coder_path[512];
	char asset_path[512];
    char output_path[512];
	TempString s1 = read_json_string(root, "ScriptPath");
	TempString s2 = read_json_string(root, "CoderPath");
	TempString s3 = read_json_string(root, "AssetPath");
    TempString s4 = read_json_string(root, "OutputPath");
	frame_preset_t preset;

	preset.szWorkPath = work_path.CString();
	preset.szScriptPath = parse_full_path(s1.CString(),
	    work_path.CString(), script_path, sizeof(script_path));
	preset.szCoderPath = parse_full_path(s2.CString(),
	    work_path.CString(), coder_path, sizeof(coder_path));
	preset.szAssetPath = parse_full_path(s3.CString(),
	    work_path.CString(), asset_path, sizeof(asset_path));

	// Get document directory.
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
		NSUserDomainMask, YES);
	NSString* docPath = [paths objectAtIndex:0];
	const char* doc_dir = [docPath UTF8String];

	if (s4.Empty())
	{
		plat_safe_sprintf(output_path, sizeof(output_path), "%s/", doc_dir);
	}
	else
	{
		plat_safe_sprintf(output_path, sizeof(output_path), "%s/%s/",
		    doc_dir, s4.CString());
	}

    preset.szOutputPath = output_path;

	// Component configuration.
	IServiceCreator* pSvcCreator = NULL;
	IObjectCreator* pObjCreator = NULL;

	link_creators(&pSvcCreator, &pObjCreator);

	// Name of main object.
	preset.szMainObject = read_json_string(root, "MainObject");
	// Interface to file system.
	preset.pFileAccess = NULL;
	// Service creators.
	preset.pServiceCreator = pSvcCreator;
	// Object creators.
	preset.pObjectCreator = pObjCreator;
	// Reload script when file changed.
	preset.bScriptReload = read_json_boolean(root, "ScriptReload");
	// Profile script performance.
	preset.nScriptProfile = 0;

	// Sleep time per frame.
	m_nSleep = read_json_int32(root, "Sleep");

	// Memory validate.
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

    // Set custom log function.
    frame_set_log_func(ios_frame_log, NULL);

	// Start frame.
	if (!frame_startup(preset))
	{
		show_fatal_error("Frame startup failed.");
		abort();
	}

	// Get user input service.
	m_pInputService = (IInputService*)frame_get_service("FlatInputService");
	
	if (NULL == m_pInputService)
	{
		show_fatal_error("No input service.");
		abort();
	}

	// Get render service.
	m_pRenderService = (IRenderService*)frame_get_service("RenderService");

	if (m_pRenderService)
	{
		//int width = (int)view.drawableWidth;
        //int height = (int)view.drawableHeight;
		int width = (int)(bounds.size.width * screenScale);
        int height = (int)(bounds.size.height * screenScale);
        
        m_pRenderService->SetWindowSize(width, height);
		//m_pRenderService->SetEglFunction(egl_present, engine);
	}
	else
	{
		show_fatal_error("No render service.");
		abort();
	}

	// Name of main script.
	m_sMainScript = read_json_string(root, "MainScript");
	/*
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
			pMainObj->GetUID(), TempArgList(), NULL);

		if (!succeed)
		{
			show_fatal_error("Run main script failed.");
			abort();
		}
	}
	*/

	// Initialize main timer.
	m_Timer.SetHighPrecision(true);
	m_Timer.Create();
}

- (void)viewDidAppear:(BOOL)animated
{
    NSLog(@"viewDidAppear");

	if (m_pRenderService)
	{
		// Set safe area.
		UIEdgeInsets safeAreaInsets = self.view.safeAreaInsets;
		CGFloat scale = m_pGLKView.contentScaleFactor;
		int sa_left = (int)(safeAreaInsets.left * scale);
		int sa_top = (int)(safeAreaInsets.top * scale);
		int sa_right = (int)(safeAreaInsets.right * scale);
		int sa_bottom = (int)(safeAreaInsets.bottom * scale);

		m_pRenderService->SetSafeArea(sa_left, sa_top, sa_right, sa_bottom);
	}

	// Execute main script.
	if (!m_sMainScript.Empty())
	{
		IObject* pMainObj = frame_get_main_object();

		if (NULL == pMainObj)
		{
			show_fatal_error("No main object.");
			abort();
		}
		
		bool succeed = frame_run_routine(m_sMainScript.CString(), "main",
			pMainObj->GetUID(), TempArgList(), NULL);

		if (!succeed)
		{
			show_fatal_error("Run main script failed.");
			abort();
		}

		m_sMainScript = "";
	}

	m_bReady = true;
}

- (void)viewDidUnload
{
    [super viewDidUnload];

    NSLog(@"viewDidUnload");

    if (m_pInputService)
	{
		frame_release_service(m_pInputService);
		m_pInputService = NULL;
	}

	if (m_pRenderService)
	{
		frame_release_service(m_pRenderService);
		m_pRenderService = NULL;
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

    if ([self isViewLoaded] && ([[self view] window] == nil))
    {
        self.view = nil;

        if ([EAGLContext currentContext] == self.context)
        {
            [EAGLContext setCurrentContext:nil];
        }
      
        self.context = nil;
    }
}

- (void)update
{
    //update();
}

- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
    //draw();
	if (!m_bReady)
	{
		return;
	}

    if (frame_get_exit())
    {
        return;
    }

    frame_period_set_time(m_Timer.GetElapseSeconds());
    frame_period_check_routine();
    frame_period_work();

    if (m_nSleep >= 0)
    {
        plat_sleep(m_nSleep);
    }
}

// Handle touches.

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint p = [touch locationInView:touch.view];
		int x = p.x * m_pGLKView.contentScaleFactor;
		int y = p.y * m_pGLKView.contentScaleFactor;
        IInputService::input_t msg;
        double cur_time = plat_get_time_count();

        if ((cur_time - m_dActionUpTime) < 0.3F)
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
        m_pInputService->ProcessInput(msg);
    }
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint p = [touch locationInView:touch.view];
		int x = p.x * m_pGLKView.contentScaleFactor;
		int y = p.y * m_pGLKView.contentScaleFactor;
        IInputService::input_t msg;

        msg.nType = IInputService::TYPE_MOUSE_MOVE;
        msg.dPosX = (double)x;
        msg.dPosY = (double)y;
        msg.nFlags = IInputService::FLAG_LEFT;
        m_pInputService->ProcessInput(msg);
    }
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint p = [touch locationInView:touch.view];
		int x = p.x * m_pGLKView.contentScaleFactor;
		int y = p.y * m_pGLKView.contentScaleFactor;
        
		m_dActionUpTime = plat_get_time_count();

        IInputService::input_t msg;

        msg.nType = IInputService::TYPE_LEFT_UP;
        msg.dPosX = (double)x;
        msg.dPosY = (double)y;
        msg.nFlags = 0;
        m_pInputService->ProcessInput(msg);
    }
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    // UITouch* touch = [touches anyObject];
    for (UITouch* touch in touches)
    {
        CGPoint p = [touch locationInView:touch.view];
        
        //touch("CANCEL", p.x, p.y);
    }
}

@end
