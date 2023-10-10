//
// Created by Steel_Shadow on 2023/9/23.
//

#include "Error.h"

#include <iostream>

#include "lexer/Lexer.h"

void Error::raise_error() {
    std::cout << "Error at "
              << Lexer::getInstance()->row[0] + 1
              << ","
              << Lexer::getInstance()->column[0]
              << "---------------------------------------"
              << std::endl;
    exit(-1);
}
