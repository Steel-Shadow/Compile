//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Def.h"

#include "parser/Parser.h"
#include "Decl.h"

using namespace Parser;

std::unique_ptr<Def> Def::parse(bool cons) {
    auto n = std::make_unique<Def>();
    n->cons = cons;
    n->ident = Ident::parse();

    while (curLexType == NodeType::LBRACK) {
        lexer.next();

        n->dims.push_back(Exp::parse(true));

        singleLex(NodeType::RBRACK);
    }

    if (cons || curLexType == NodeType::ASSIGN) {
        lexer.next();
        n->initVal = InitVal::parse(cons);
    }

    if (cons) {
        output(NodeType::ConstDef);
    } else {
        output(NodeType::VarDef);
    }

    return n;
}
