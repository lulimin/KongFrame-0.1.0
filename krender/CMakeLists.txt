# kong-frame cmake file
project(krender_static)

file(GLOB KRENDER_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KRENDER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB GLES3_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/gles3/*.h)
file(GLOB GLES3_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gles3/*.cpp)
source_group("" FILES ${KRENDER_HDRS} ${KRENDER_SRCS})
source_group("gles3" FILES ${GLES3_HDRS} ${GLES3_SRCS})

add_library(${PROJECT_NAME} STATIC ${KRENDER_HDRS} ${KRENDER_SRCS} ${GLES3_HDRS} ${GLES3_SRCS})

if(WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC ${KONG_EXTERN_DIR}/ogles)
endif(WIN32)
