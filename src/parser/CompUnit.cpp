//
// Created by Steel_Shadow on 2023/10/11.
//

#include "CompUnit.h"

#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace Parser;

std::unique_ptr<CompUnit> CompUnit::parse() {
    auto n = std::make_unique<CompUnit>();

    while (curLexType == NodeType::CONSTTK || curLexType == NodeType::INTTK) {
        if (lexer.peek(1).first == NodeType::IDENFR && lexer.peek(2).first == NodeType::LPARENT
            || curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->decls.push_back(Decl::parse());
    }

    while (curLexType == NodeType::VOIDTK || curLexType == NodeType::INTTK) {
        if (curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->funcDefs.push_back(FuncDef::parse());
    }

    n->mainFuncDef = MainFuncDef::parse();

    output(NodeType::CompUnit);

    return n;
}
