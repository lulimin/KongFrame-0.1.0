# kong-frame cmake file
project(kflat_static)

file(GLOB KFLAT_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB KFLAT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("" FILES ${KFLAT_HDRS} ${KFLAT_SRCS})

add_library(${PROJECT_NAME} STATIC ${KFLAT_HDRS} ${KFLAT_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${KONG_EXTERN_DIR}/freetype-2.10.4/include)
