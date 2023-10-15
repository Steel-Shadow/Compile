//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Def.h"

#include "Decl.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

using namespace Parser;

std::unique_ptr<Def> Def::parse(bool cons) {
    auto n = std::make_unique<Def>();
    n->cons = cons;
    n->ident = Ident::parse();

    while (Lexer::curLexType == NodeType::LBRACK) {
        Lexer::next();

        n->dims.push_back(Exp::parse(true));

        singleLex(NodeType::RBRACK);
    }

    if (cons || Lexer::curLexType == NodeType::ASSIGN) {
        Lexer::next();
        n->initVal = InitVal::parse(cons);
    }

    if (cons) {
        output(NodeType::ConstDef);
    } else {
        output(NodeType::VarDef);
    }

    return n;
}
