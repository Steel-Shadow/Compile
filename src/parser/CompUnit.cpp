//
// Created by Steel_Shadow on 2023/10/11.
//

#include "CompUnit.h"

#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace Parser;

CompUnit::CompUnit() {
    while (curLexType == NodeType::CONSTTK || curLexType == NodeType::INTTK) {
        if (lexer.peek(1).first == NodeType::IDENFR && lexer.peek(2).first == NodeType::LPARENT
            || curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        decls.push_back(make_unique<Decl>(Decl()));
    }

    while (curLexType == NodeType::VOIDTK || curLexType == NodeType::INTTK) {
        if (curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        funcDefs.push_back(make_unique<FuncDef>(FuncDef()));
    }

    if (curLexType == NodeType::INTTK) {
        mainFuncDef = make_unique<MainFuncDef>(MainFuncDef());
    } else { Error::raise_error(); }

    output(NodeType::CompUnit);
}
