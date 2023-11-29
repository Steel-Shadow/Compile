//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Parser.h"

#include "errorHandler/Error.h"
#include "config.h"

void Parser::singleLex(LexType type, int row) {
    if (Lexer::curLexType == type) {
        Lexer::next();
    } else {
        if (type == LexType::SEMICN) {
            Error::raise('i', row);
        } else if (type == LexType::RPARENT) {
            Error::raise('j', row);
        } else if (type == LexType::RBRACK) {
            Error::raise('k', row);
        } else {
            Error::raise(std::string("Miss singleLex ") + toString(type));
        }
    }
}

void Parser::output(AST type) {
#ifdef STDOUT_PARSER
    std::cout << "<" << toString(type) << ">" << '\n';
#endif
#ifdef FILEOUT_PARSER
    Lexer::outFileStream << "<" << toString((type)) << ">" << '\n';
#endif
}

