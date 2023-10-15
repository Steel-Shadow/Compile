//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Exp.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

using namespace Parser;

std::unique_ptr<Exp> Exp::parse(bool cons) {
    auto n = std::make_unique<Exp>();
    n->cons = cons;
    n->addExp = AddExp::parse();

    if (cons) {
        output(NodeType::ConstExp);
    } else {
        output(NodeType::Exp);
    }
    return n;
}
