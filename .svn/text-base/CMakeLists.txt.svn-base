##
 # ----------------------------------------------------------------------------------
 #     系统名称: 列控设备动态监测系统 
 #       文件名: CMakeLists.txt
 #
 #       版本号: 1.0
 #     创建日期: 2014年08月20日 10时26分38秒
 #
 #         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 #  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 #
 #
 #     修改历史:   none
 #
 # ----------------------------------------------------------------------------------
 ##

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
project(DMS)

if(ARM3250)
    ## 设置为交叉编译模式
    set(CMAKE_SYSTEM_NAME Linux)

    set(CMAKE_C_COMPILER "arm-vfp-linux-gnu-gcc")
    set(CMAKE_CXX_COMPILER "arm-vfp-linux-gnu-g++")
    set(CMAKE_STRIP "arm-vfp-linux-gnu-strip")
    include_directories("/opt/nxp/gcc-4.3.2-glibc-2.7/include")
    link_directories("/opt/nxp/gcc-4.3.2-glibc-2.7/lib")
endif(ARM3250)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set(HELIUM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/helium/")
set(UTILITY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/apps/utils/")

set(DMS_VERSION_MAJOR "3")
set(DMS_VERSION_MINOR "0")
set(DMS_VERSION_PATCH "0")

set(DMS_VERSION "${DMS_VERSION_MAJOR}.${DMS_VERSION_MINOR}.${DMS_VERSION_PATCH}")

get_filename_component(DMS_BIN_DIR bin ABSOLUTE)
get_filename_component(DMS_LIB_DIR ${DMS_BIN_DIR}/lib ABSOLUTE)
get_filename_component(DMS_INC_DIR include ABSOLUTE)
get_filename_component(DMS_SRC_DIR src ABSOLUTE)
get_filename_component(DMS_BUILD_DIR build ABSOLUTE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")

option(make_static_libs "Build static library instead of shared" OFF)
if(make_static_libs)
    set(BUILD_SHARED_LIBS OFF)
else()
    set(BUILD_SHARED_LIBS ON)
endif(make_static_libs)

## 设置库文件和可执行文件的存放目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DMS_LIB_DIR} CACHE PATH
    "Output directory for the dynamic libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DMS_LIB_DIR} CACHE PATH
    "Output directory for the static libraries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DMS_BIN_DIR} CACHE PATH
    "Output directory for the binaries")

## 设置make install的指令
install(DIRECTORY ${DMS_LIB_DIR}}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

## 附加包含目录
include_directories(${HELIUM_DIR})
include_directories(${UTILITY_DIR})

## 添加子目录
add_subdirectory(src)
