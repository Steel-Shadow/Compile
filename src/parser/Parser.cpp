//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include "error/Error.h"
#include <iostream>

//todo: bad practice! can't determine in main!
Lexer &Parser::lexer = Lexer::getInstance("testfile.txt", "output.txt");

NodeType &Parser::curLexType = lexer.getLexType();

void Parser::singleLex(NodeType type) {
    if (curLexType == type) {
        lexer.next();
    } else {
        Error::raise_error();
    }
}

void Parser::output(NodeType type) {
#ifdef MY_DEBUG
    std::cout << "<" << typeToStr(type) << ">" << std::endl;
#endif
    lexer.getOutFileStream() << "<" << typeToStr(type) << ">" << std::endl;
}

