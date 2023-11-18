//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include "frontend/error/Error.h"
#include "config.h"

void Parser::singleLex(NodeType type, int row) {
    if (Lexer::curLexType == type) {
        Lexer::next();
    } else {
        if (type == NodeType::SEMICN) {
            Error::raise('i', row);
        } else if (type == NodeType::RPARENT) {
            Error::raise('j', row);
        } else if (type == NodeType::RBRACK) {
            Error::raise('k', row);
        } else {
            Error::raise(std::string("Miss singleLex ") + typeToStr(type));
        }
    }
}

void Parser::output(NodeType type) {
#ifdef STDOUT_PARSER
    std::cout << "<" << typeToStr(type) << ">" << '\n';
#endif
#ifdef FILEOUT_PARSER
    Lexer::outFileStream << "<" << typeToStr(type) << ">" << '\n';
#endif
}

