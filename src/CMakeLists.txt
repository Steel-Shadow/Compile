aux_source_directory(. SRC_LIST)

aux_source_directory(${PROJECT_SOURCE_DIR}/include INCLUDE_SOURCES)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# only for show *.h in IDE
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(
        ${PROJECT_NAME}

        ${SOURCES}
        ${HEADERS}
        ${INCLUDE_SOURCES}
)

# include *.h
target_include_directories(${PROJECT_NAME} PRIVATE . error lexer ${PROJECT_SOURCE_DIR}/include)
