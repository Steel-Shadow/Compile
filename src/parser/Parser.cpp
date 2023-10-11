//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include <iostream>

Btype::Btype() {
    Parser::singleLex(NodeType::INTTK);
}

std::string Parser::Ident() {
    std::string ident;

    if (curLexType == NodeType::IDENFR) {
        ident = lexer.peek().second;
        lexer.next();
    } else { Error::raise_error(); }

    return ident;
}

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

