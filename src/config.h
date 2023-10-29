//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_CONFIG_H
#define COMPILER_CONFIG_H

//#define ERROR
#define CODEGEN

#if defined(ERROR)
#define FILE_PRINT_ERROR
#else
//#define FILEOUT_LEXER
//#define FILEOUT_PARSER
#endif

#if defined(CODEGEN)
#define FILEOUT_MIPS
#endif

#if defined(MY_DEBUG)
#define STDOUT_ERROR
//#define STDOUT_LEXER
//#define STDOUT_PARSER
#endif


#endif //COMPILER_CONFIG_H
