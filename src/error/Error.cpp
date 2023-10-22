//
// Created by Steel_Shadow on 2023/9/23.
//

#include "Error.h"

#include <iostream>

#include "config.h"

std::ofstream Error::errorFileStream;

void Error::raise(char code, int row) {
#ifdef STDOUT_ERROR
    std::cout << row << " " << code << std::endl;
#endif
#ifdef FILE_PRINT_ERROR
    errorFileStream << row << " " << code << std::endl;
#endif
}

// My error, which is not defined in course tasks.
void Error::raise(const std::string &mes) {
#ifdef STDOUT_ERROR
    std::cout << "Error: " << mes << " "
              << Lexer::curRow << "," << Lexer::column[0]
              << "---------------------------------------"
              << std::endl;
//    exit(-1);
#endif
}
