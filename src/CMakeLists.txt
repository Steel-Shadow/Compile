aux_source_directory(. SRC_LIST)

aux_source_directory(${PROJECT_SOURCE_DIR}/include INCLUDE_LIST)

aux_source_directory(error ERROR_LIST)
aux_source_directory(lexer LEXER_LIST)

# only for show *.h in IDE
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable(
        ${PROJECT_NAME}

        ${SRC_LIST}

        ${INCLUDE_LIST}

        ${ERROR_LIST}
        ${LEXER_LIST}
        
        ${HEADERS}
)

# ./config.h 以及子目录的 *.h
target_include_directories(${PROJECT_NAME} PRIVATE . error lexer ${PROJECT_SOURCE_DIR}/include)
