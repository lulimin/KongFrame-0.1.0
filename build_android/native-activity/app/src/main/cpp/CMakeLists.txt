#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

#project(kclient)
project(kong)

# build native_app_glue as a static lib
#set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
#add_library(native_app_glue STATIC
#    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)

# Get source directory of project.
string(FIND ${CMAKE_SOURCE_DIR} "/build_android" BUILD_DIR_POS)
string(SUBSTRING ${CMAKE_SOURCE_DIR} "0" ${BUILD_DIR_POS} KONG_SOURCE_DIR)

set(KONG_CONTRIB_DIR ${KONG_SOURCE_DIR}/contrib)
#set(KONG_KCLIENT_DIR ${KONG_SOURCE_DIR}/kclient)

# Don't install freetype headers and libaries.
set(SKIP_INSTALL_ALL TRUE)
add_subdirectory(${KONG_CONTRIB_DIR}/freetype-2.10.4 ${CMAKE_BINARY_DIR}/freetype)

add_subdirectory(${KONG_SOURCE_DIR}/kflat ${CMAKE_BINARY_DIR}/kflat)
add_subdirectory(${KONG_SOURCE_DIR}/kflatex ${CMAKE_BINARY_DIR}/kflatex)
add_subdirectory(${KONG_SOURCE_DIR}/kframe ${CMAKE_BINARY_DIR}/kframe)
add_subdirectory(${KONG_SOURCE_DIR}/khelper ${CMAKE_BINARY_DIR}/khelper)
add_subdirectory(${KONG_SOURCE_DIR}/krender ${CMAKE_BINARY_DIR}/krender)
add_subdirectory(${KONG_SOURCE_DIR}/kclient ${CMAKE_BINARY_DIR}/kclient)

#list(APPEND KCLIENT_SRCS ${KONG_KCLIENT_DIR}/android/android_native_app_glue.c)
#list(APPEND KCLIENT_SRCS ${KONG_KCLIENT_DIR}/android/android_file_access.cpp)
#list(APPEND KCLIENT_SRCS ${KONG_KCLIENT_DIR}/android/android_main.cpp)
#list(APPEND KCLIENT_SRCS ${KONG_KCLIENT_DIR}/memory_list.cpp)
#list(APPEND KCLIENT_SRCS ${KONG_KCLIENT_DIR}/memory_link.cpp)

# now build app's shared lib
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
#set(CMAKE_SHARED_LINKER_FLAGS
#    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

#add_library(native-activity SHARED main.cpp)
#add_library(${PROJECT_NAME} SHARED ${KCLIENT_SRCS})

#target_include_directories(native-activity PRIVATE
#    ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
#target_link_libraries(${PROJECT_NAME}
#    kflat_static
#    kflatex_static
#    kframe_static
#    khelper_static
#    krender_static
#    freetype
#    android
#    EGL
#    GLESv3
#    log)
#target_link_libraries(native-activity
#    android
#    native_app_glue
#    EGL
#    GLESv1_CM
#    log)
