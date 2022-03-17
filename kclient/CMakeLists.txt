# kong-frame cmake file
project(kclient)

file(GLOB KCLIENT_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KCLIENT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("" FILES ${KCLIENT_HDRS} ${KCLIENT_SRCS})

if(WIN32)
    file(GLOB WIN_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/windows/*.h)
    file(GLOB WIN_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/windows/*.cpp)
    source_group("windows" FILES ${WIN_HDRS} ${WIN_SRCS})

    add_executable(${PROJECT_NAME} WIN32 ${KCLIENT_HDRS} ${KCLIENT_SRCS} ${WIN_HDRS} ${WIN_SRCS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_EXTERN_DIR}/ogles)
    target_link_libraries(${PROJECT_NAME} ${KONG_EXTERN_DIR}/ogles/x64/libglesv2.lib)
    target_link_libraries(${PROJECT_NAME} ${KONG_EXTERN_DIR}/ogles/x64/libegl.lib)
    target_link_libraries(${PROJECT_NAME} ${KONG_EXTERN_DIR}/dbghelp/x64/dbghelp.lib)
elseif(ANDROID)
    # Now build app's shared lib.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")
    # Export ANativeActivity_onCreate(),
    # Refer to: https://github.com/android-ndk/ndk/issues/381.
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

    list(APPEND ANDROID_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/android/android_native_app_glue.h)
    list(APPEND ANDROID_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/android/android_native_app_glue.c)
    list(APPEND ANDROID_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/android/android_file_access.h)
    list(APPEND ANDROID_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/android/android_file_access.cpp)
    list(APPEND ANDROID_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/android/android_main.cpp)
    source_group("android" FILES ${ANDROID_SRCS})

    add_library(${PROJECT_NAME} SHARED ${KCLIENT_HDRS} ${KCLIENT_SRCS} ${ANDROID_SRCS})
    target_link_libraries(${PROJECT_NAME} android EGL GLESv3 log)
elseif(IOS)
    list(APPEND IOS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ios/AppDelegate.h)
    list(APPEND IOS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ios/AppDelegate.m)
    list(APPEND IOS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ios/ViewController.h)
    list(APPEND IOS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ios/ViewController.mm)
    list(APPEND IOS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ios/main.m)
    source_group("ios" FILES ${IOS_SRCS})
    
    add_executable(${PROJECT_NAME} ${KCLIENT_HDRS} ${KCLIENT_SRCS} ${IOS_SRCS})
    find_library(UIKIT_LIB UIKit)
    find_library(GLKIT_LIB GLKit)
    find_library(OPENGLES_LIB OpenGLES)
    target_link_libraries(${PROJECT_NAME} ${UIKIT_LIB} ${GLKIT_LIB} ${OPENGLES_LIB})
    
    # Set bundle.
    list(APPEND RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ios/Main.storyboard)
    target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_FILES})
    source_group("Resources" FILES ${RESOURCE_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE 1
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.kong.kclient
        #XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS @executable_path/Frameworks
        #XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
        #XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        #OSX_ARCHITECTURES "arm64;x86_64"
        #COMPILE_OPTIONS "-F ${FRAMEWORK_FOLDER}"
        RESOURCE "${RESOURCE_FILES}"
    )
    
    # Asset files.
    file(GLOB_RECURSE ASSET_SRCS "${KONG_SOURCE_DIR}/asset/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ASSET_SRCS})
    source_group("asset" FILES ${ASSET_SRCS})
    
    # Copy asset files to bundle.
    foreach(ASSET_FILE ${ASSET_SRCS})
        file(RELATIVE_PATH ASSET_PATH "${KONG_SOURCE_DIR}/asset" ${ASSET_FILE})
        string(FIND ${ASSET_PATH} "/" SLASH REVERSE)
        if(${SLASH} EQUAL -1)
            set(TO_PATH "")
        else()
            string(SUBSTRING ${ASSET_PATH} 0 ${SLASH} TO_PATH)
        endif()
        set_property(SOURCE ${ASSET_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "asset/${TO_PATH}")
        set_property(SOURCE ${ASSET_FILE} PROPERTY HEADER_FILE_ONLY 1)
    endforeach(ASSET_FILE)
endif(WIN32)

target_link_libraries(${PROJECT_NAME} freetype kframe_static kflat_static kflatex_static khelper_static krender_static)
