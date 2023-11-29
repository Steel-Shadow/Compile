//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST/AST.h"
#include "frontend/lexer/Lexer.h"

// Specific parser method is distributed in respective class.
namespace Parser {
// check the type and Lexer::next()
void singleLex(LexType type, int row = Lexer::curRow);

void output(AST type);
}

#endif
