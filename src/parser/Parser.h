//
// Created by Steel_Shadow on 2023/9/25.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Error.h"
#include "Lexer.h"

class Parser {
private:
    static Parser *instance;

    Parser() = default;

public:
    // Singleton
    static Parser *getInstance();

    Parser(Parser const &) = delete;

    void operator=(Parser const &) = delete;
    // Singleton
};


#endif //COMPILER_PARSER_H
