# version support
cmake_minimum_required(VERSION 2.8)

project (tiny_plus)
# version info
set(TINY_PLUS_MAJOR_VERSION "1")
set(TINY_PLUS_MINOR_VERSION "0")
set(TINY_PLUS_PATCH_VERSION "0")
set(TINY_PLUS_VERSION_STRING
    "${TINY_PLUS_MAJOR_VERSION}.${TINY_PLUS_MINOR_VERSION}.${TINY_PLUS_PATCH_VERSION}"
)

# 设置CMake变量
set(CMAKE_CXX "clang++")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
# set(CMAKE_VERBOSE_MAKEFILE on) # 编译过程详细信息
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置debug模式和release模式
#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -g")
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    message("Build Type (${CMAKE_BUILD_TYPE}): ${CMAKE_CXX_FLAGS_DEBUG}")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    message("Build Type (${CMAKE_BUILD_TYPE}): ${CMAKE_CXX_FLAGS_RELEASE}")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )


# 查找当前目录下所有源文件并保存到SRC_LIST中
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
# 去除带有main函数的源文件
list(REMOVE_ITEM SRC_LIST
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

# 头文件的位置
include_directories(${PROJECT_SOURCE_DIR}/include)
# 链接库的位置
#link_directories(${PROJECT_SOURCE_DIR}/libs)

# 编译main
add_executable(compiler
    ${PROJECT_SOURCE_DIR}/src/main.cpp ${SRC_LIST}
)
