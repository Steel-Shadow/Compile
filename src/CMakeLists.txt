aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

#aux_source_directory(error ERROR_LIST)
#aux_source_directory(lexer LEXER_LIST)

add_executable(
        ${PROJECT_NAME}

        ${SRC_LIST}

        ${INCLUDE_LIST}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
