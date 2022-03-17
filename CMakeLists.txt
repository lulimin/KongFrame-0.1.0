# kong-frame cmake file
cmake_minimum_required(VERSION 3.6)

project(kong)

#add_compile_definitions($<$<CONFIG:Debug>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(KONG_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(KONG_EXTERN_DIR ${KONG_SOURCE_DIR}/ext)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "..." FORCE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "" CACHE PATH "..." FORCE)
endif()

#if(ANDROID)
#    # Get source directory of project.
#    string(FIND ${CMAKE_SOURCE_DIR} "/build_android" BUILD_DIR_POS)
#    string(SUBSTRING ${CMAKE_SOURCE_DIR} "0" ${BUILD_DIR_POS} KONG_SOURCE_DIR)
#    set(KONG_EXTERN_DIR ${KONG_SOURCE_DIR}/ext)
#else(ANDROID)
#    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
#    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#    set(KONG_SOURCE_DIR ${CMAKE_SOURCE_DIR})
#    set(KONG_EXTERN_DIR ${KONG_SOURCE_DIR}/ext)
#    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "..." FORCE)
#
#    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#        set(CMAKE_INSTALL_PREFIX "" CACHE PATH "..." FORCE)
#    endif()
#endif(ANDROID)

# Don't install freetype headers and libaries.
set(SKIP_INSTALL_ALL TRUE)
add_subdirectory(${KONG_EXTERN_DIR}/freetype-2.10.4 ${CMAKE_BINARY_DIR}/freetype)

add_compile_definitions($<$<CONFIG:Debug>:K_DEBUG>)
# Libraries and executable.
add_subdirectory(${KONG_SOURCE_DIR}/kflat ${CMAKE_BINARY_DIR}/kflat)
add_subdirectory(${KONG_SOURCE_DIR}/kflatex ${CMAKE_BINARY_DIR}/kflatex)
add_subdirectory(${KONG_SOURCE_DIR}/kframe ${CMAKE_BINARY_DIR}/kframe)
add_subdirectory(${KONG_SOURCE_DIR}/khelper ${CMAKE_BINARY_DIR}/khelper)
add_subdirectory(${KONG_SOURCE_DIR}/krender ${CMAKE_BINARY_DIR}/krender)
add_subdirectory(${KONG_SOURCE_DIR}/kclient ${CMAKE_BINARY_DIR}/kclient)

if(WIN32)
    # Set visual studio startup project.
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kclient)
    
    # Copy necessary files for executing.
    install(FILES 
        ext/dbghelp/x64/dbghelp.dll 
        ext/ogles/x64/libegl.dll 
        ext/ogles/x64/libglesv2.dll 
        asset/config/kclient.json
        DESTINATION ${CMAKE_BINARY_DIR}/bin/debug CONFIGURATIONS Debug)
    install(FILES 
        ext/dbghelp/x64/dbghelp.dll 
        ext/ogles/x64/libegl.dll 
        ext/ogles/x64/libglesv2.dll 
        asset/config/kclient.json
        DESTINATION ${CMAKE_BINARY_DIR}/bin/release CONFIGURATIONS Release)
endif(WIN32)
