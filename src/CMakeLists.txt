aux_source_directory(${PROJECT_SOURCE_DIR}/include INCLUDE_SOURCES)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test)

# only for show *.h in IDE
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# all *.c *.cpp files in src
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(
        ${PROJECT_NAME}

        ${HEADERS}
        ${SOURCES}

        ${INCLUDE_SOURCES}
)

# include *.h
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_SOURCE_DIR}/include)
