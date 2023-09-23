aux_source_directory(. SRC_LIST)
aux_source_directory(lexer LEXER_LIST)

add_executable(
        ${PROJECT_NAME}

        ${SRC_LIST}
        ${LEXER_LIST}
        ${INCLUDE_LIST}
)
