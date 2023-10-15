//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_CONFIG_H
#define COMPILER_CONFIG_H


//TODO: In error, temporary turn off.
//#define FILE_PRINT_LEXER
//#define FILE_PRINT_PARSER

#define ERROR

// print in std::cout
#ifdef MY_STDOUT
#define STDOUT_PRINT_LEXER
#define STDOUT_PRINT_PARSER
#endif


#endif //COMPILER_CONFIG_H
