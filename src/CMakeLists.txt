aux_source_directory(. SRC_LIST)

aux_source_directory(${PROJECT_SOURCE_DIR}/include INCLUDE_LIST)

aux_source_directory(error ERROR_LIST)
aux_source_directory(lexer LEXER_LIST)

add_executable(
        ${PROJECT_NAME}

        ${SRC_LIST}

        ${ERROR_LIST}
        ${LEXER_LIST}

        ${INCLUDE_LIST}
)

# ./config.h 以及子目录的 *.h
target_include_directories(${PROJECT_NAME} PRIVATE . error lexer)
