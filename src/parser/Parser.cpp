//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include <iostream>
#include "error/Error.h"
#include "lexer/Lexer.h"
#include "config.h"

void Parser::singleLex(NodeType type) {
    if (Lexer::curLexType == type) {
        Lexer::next();
    } else {
        Error::raise_error();
    }
}

void Parser::output(NodeType type) {
#ifdef STDOUT_PRINT_PARSER
    std::cout << "<" << typeToStr(type) << ">" << std::endl;
#endif
#ifdef FILE_PRINT_PARSER
    Lexer::outFileStream << "<" << typeToStr(type) << ">" << std::endl;
#endif
}

