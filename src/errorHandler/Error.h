//
// Created by Steel_Shadow on 2023/9/23.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <string>
#include "frontend/lexer/Lexer.h"

class Error {
public:
    static std::ofstream errorFileStream;

    static void raise(char code, int row = Lexer::curRow);

    static void raise(const std::string &mes = "unnamed");
};


#endif
