# kong-frame cmake file
project(kframe_static)

option(K_MULTI_FRAME "Enable multiple frameworks in multi-threaded mode." OFF)

file(GLOB LUA_HDRS ${KONG_EXTERN_DIR}/lua-5.3.6/*.h)
file(GLOB LUA_SRCS ${KONG_EXTERN_DIR}/lua-5.3.6/*.c)
file(GLOB KFRAME_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KFRAME_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("" FILES ${KFRAME_HDRS} ${KFRAME_SRCS})
source_group("lua" FILES ${LUA_HDRS} ${LUA_SRCS})

add_library(${PROJECT_NAME} STATIC ${LUA_HDRS} ${LUA_SRCS} ${KFRAME_HDRS} ${KFRAME_SRCS})

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
endif(WIN32)

if(K_MULTI_FRAME)
    target_compile_definitions(${PROJECT_NAME} PUBLIC K_MULTI_FRAME)
endif(K_MULTI_FRAME)
