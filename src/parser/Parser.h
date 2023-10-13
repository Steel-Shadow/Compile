//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer/NodeType.h"
#include "lexer/Lexer.h"

namespace Parser {
    // [lexer curLexType] are only for shorter code in parser, not necessary.
    extern Lexer &lexer;
    // [lexer curLexType] are only for shorter code in parser, not necessary.
    extern NodeType &curLexType;

    void singleLex(NodeType type);

    // don't use/print in <BlockItem>, <Decl>, <BType>
    void output(NodeType type);
};

#endif //COMPILER_PARSER_H
