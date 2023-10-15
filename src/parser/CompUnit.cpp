//
// Created by Steel_Shadow on 2023/10/11.
//

#include "CompUnit.h"

#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace Parser;

std::unique_ptr<CompUnit> CompUnit::parse() {
    auto n = std::make_unique<CompUnit>();

    while (Lexer::curLexType == NodeType::CONSTTK || Lexer::curLexType == NodeType::INTTK) {
        if (Lexer::peek(1).first == NodeType::IDENFR && Lexer::peek(2).first == NodeType::LPARENT
            || Lexer::curLexType == NodeType::INTTK && Lexer::peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->decls.push_back(Decl::parse());
    }

    while (Lexer::curLexType == NodeType::VOIDTK || Lexer::curLexType == NodeType::INTTK) {
        if (Lexer::curLexType == NodeType::INTTK && Lexer::peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->funcDefs.push_back(FuncDef::parse());
    }

    n->mainFuncDef = MainFuncDef::parse();

    output(NodeType::CompUnit);

    return n;
}
