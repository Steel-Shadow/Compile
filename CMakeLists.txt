cmake_minimum_required(VERSION 3.15)

# C++17 does not work with MSVC in Lexer.cpp.
# C++20 MSVC works.
# But C++17 works on gcc and clang
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-v")

# make sure to use clang or gcc
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(MY_DEBUG)
endif ()

project(Compiler)

add_subdirectory(src)
