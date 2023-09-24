cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
#set(CMAKE_CXX_FLAGS "-v")

project(Compiler)

include_directories(${PROJECT_SOURCE_DIR}/include)
#aux_source_directory(${PROJECT_SOURCE_DIR}/include INCLUDE_LIST)

add_subdirectory(src)
