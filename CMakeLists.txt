cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(Compiler)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

# 向工程添加多个指定头文件的搜索路径，路径之间用空格分隔
include_directories(include)
aux_source_directory(
        ${PROJECT_SOURCE_DIR}/include
        INCLUDE_LIST
)

add_subdirectory(src)
#add_subdirectory(test)
