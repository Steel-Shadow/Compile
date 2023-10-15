//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer/NodeType.h"

// Specific parser method is distributed in respective class.
namespace Parser {
    // check the type and Lexer::next()
    void singleLex(NodeType type);

    // don't use/print in <BlockItem>, <Decl>, <BType>
    void output(NodeType type);
}

#endif //COMPILER_PARSER_H
