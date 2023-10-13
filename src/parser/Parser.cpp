//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include "error/Error.h"
#include <iostream>
#include "Compiler.h"

void Parser::singleLex(NodeType type) {
    if (Lexer::curLexType == type) {
        Lexer::next();
    } else {
        Error::raise_error();
    }
}

void Parser::output(NodeType type) {
#ifdef MY_DEBUG
    std::cout << "<" << typeToStr(type) << ">" << std::endl;
#endif
    Lexer::outFileStream << "<" << typeToStr(type) << ">" << std::endl;
}

