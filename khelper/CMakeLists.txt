# kong-frame cmake file
project(khelper_static)

file(GLOB KHELPER_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KHELPER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("" FILES ${KHELPER_HDRS} ${KHELPER_SRCS})

add_library(${PROJECT_NAME} STATIC ${KHELPER_HDRS} ${KHELPER_SRCS})
