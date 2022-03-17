# kong-frame cmake file
project(kflatex_static)

file(GLOB KFLATEX_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KFLATEX_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("" FILES ${KFLATEX_HDRS} ${KFLATEX_SRCS})

add_library(${PROJECT_NAME} STATIC ${KFLATEX_HDRS} ${KFLATEX_SRCS})
