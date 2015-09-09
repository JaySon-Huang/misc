# version support
cmake_minimum_required(VERSION 2.8)

project (RABIN_KARP_MATCHER)
# version info
set(LIB_MAJOR_VERSION "0")
set(LIB_MINOR_VERSION "1")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING 
    "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

# 设置CMake变量
set(CMAKE_CXX "g++")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
# set(CMAKE_VERBOSE_MAKEFILE on) # 编译过程详细信息
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置debug模式和release模式
#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -g")
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    message("Build Type (${CMAKE_BUILD_TYPE}): ${CMAKE_CXX_FLAGS_DEBUG}")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    message("Build Type (${CMAKE_BUILD_TYPE}): ${CMAKE_CXX_FLAGS_RELEASE}")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )


# 查找当前目录下所有源文件并保存到SRC_LIST中
aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)
# 去除带有main函数的源文件
list(REMOVE_ITEM SRC_LIST 
    ${PROJECT_SOURCE_DIR}/test_rabinkarp.cpp 
    ${PROJECT_SOURCE_DIR}/test_simple.cpp
)

# 头文件的位置
include_directories(${PROJECT_SOURCE_DIR}/include)
# 链接库的位置
link_directories(${PROJECT_SOURCE_DIR}/libs)

# 编译main并链接gtest库
add_executable(rabinkarp
    test_rabinkarp.cpp ${SRC_LIST})
target_link_libraries(rabinkarp gtest)

add_executable(simple
    test_simple.cpp ${SRC_LIST})
target_link_libraries(simple gtest)
