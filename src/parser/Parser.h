//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer/NodeType.h"
#include "lexer/Lexer.h"

namespace Parser {
    // [lexer curLexType] are only for shorter code in parser, not necessary.
    auto &lexer = Lexer::getInstance();
    // [lexer curLexType] are only for shorter code in parser, not necessary.
    auto curLexType = lexer.getLexType();

    std::string Ident();

    void singleLex(NodeType type);

    // don't use/print in <BlockItem>, <Decl>, <BType>
    void output(NodeType type);
};

// Only int.
class Btype {
public:
    Btype();
};

#endif //COMPILER_PARSER_H
