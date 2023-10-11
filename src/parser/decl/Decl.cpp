//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Decl.h"
#include "lexer/NodeType.h"
#include "parser/Parser.h"

using namespace Parser;

Def::Def(bool isConst) : isConst(isConst) {
    ident = Ident();

    while (curLexType == NodeType::LBRACK) {
        singleLex(NodeType::LBRACK);

        dims.push_back(make_unique<Exp>(Exp(true)));

        singleLex(NodeType::RBRACK);
    }

    if (isConst || curLexType == NodeType::ASSIGN) {
        singleLex(NodeType::ASSIGN);
        initVal = make_unique<InitVal>(InitVal(isConst));
    }

    if (isConst) {
        output(NodeType::ConstDef);
    } else {
        output(NodeType::VarDef);
    }
}

Decl::Decl() {
    if (curLexType == NodeType::CONSTTK) {
        singleLex(NodeType::CONSTTK);
        isConst = true;
    } else { isConst = false; }

    Btype();

    defs.push_back(make_unique<Def>(Def(isConst)));

    while (curLexType == NodeType::COMMA) {
        singleLex(NodeType::COMMA);
        defs.push_back(make_unique<Def>(Def(isConst)));
    }

    singleLex(NodeType::SEMICN);

    if (isConst) {
        output(NodeType::ConstDecl);
    } else {
        output(NodeType::VarDecl);
    }
//    output(NodeType::Decl);
}