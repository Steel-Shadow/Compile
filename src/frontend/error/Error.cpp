//
// Created by Steel_Shadow on 2023/9/23.
//

#include "Error.h"

#include <iostream>

#include "config.h"

std::ofstream Error::errorFileStream;

void Error::raise(char code, int row) {
#ifdef STDOUT_ERROR
    std::cout << row << " " << code << '\n';
#endif
#ifdef FILEOUT_ERROR
    errorFileStream << row << " " << code << '\n';
#endif
}

// My error, which is not defined in course tasks.
void Error::raise(const std::string& mes) {
#ifdef STDOUT_ERROR
    std::cout << "error: " << mes << " "
        // << Lexer::curRow << "," << Lexer::column[0]
        << "---------------------------------------"
        << '\n';
    // exit(-1);
#endif
}
