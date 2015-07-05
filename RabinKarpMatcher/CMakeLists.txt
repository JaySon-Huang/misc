# version support
cmake_minimum_required(VERSION 2.8)

project (RABIN_KARP_MATCHER)

# 设置CMake变量
set(CMAKE_CXX "g++")
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# 查找当前目录下所有源文件并保存到SRC_LIST中
aux_source_directory(${PROJECT_SOURCE_DIR}/ SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR}/)

add_executable(main ${SRC_LIST})

