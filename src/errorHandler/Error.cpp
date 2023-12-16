//
// Created by Steel_Shadow on 2023/9/23.
//

#include "Error.h"

#include <iostream>

#include "config.h"

bool Error::hasError = false;
std::ofstream Error::errorFileStream;

void Error::raise(char code, int row) {
    hasError = true;
#ifdef STDOUT_ERROR
    std::cout << row << " " << code << '\n';
#endif
#ifdef FILEOUT_ERROR
    errorFileStream << row << " " << code << '\n';
#endif
}

// My error, which is not defined in course tasks.
void Error::raise(const std::string &mes) {
    hasError = true;
#ifdef STDOUT_ERROR
    std::cout << "error: " << mes << " "
            << "---------------------------------------\n";
#endif
#ifdef FILEOUT_ERROR
    errorFileStream << "error: " << mes << " "
            << "---------------------------------------\n";
#endif
    // exit(-1);
}
