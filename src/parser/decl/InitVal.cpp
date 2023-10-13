//
// Created by Steel_Shadow on 2023/10/11.
//

#include "InitVal.h"

#include "parser/expr/Exp.h"
#include "Compiler.h"

using namespace Parser;
using namespace Lexer;

std::unique_ptr<InitVal> InitVal::parse(bool cons) {
    std::unique_ptr<InitVal> n;

    if (curLexType == NodeType::LBRACE) {
        n = ArrayInitVal::parse(cons);
    } else {
        n = ExpInitVal::parse(cons);
    }

    if (cons) {
        output(NodeType::ConstInitVal);
    } else {
        output(NodeType::InitVal);
    }

    return n;
}

std::unique_ptr<ExpInitVal> ExpInitVal::parse(bool cons) {
    auto n = std::make_unique<ExpInitVal>();
    n->cons = cons;

    n->exp = Exp::parse(cons);
    return n;
}

std::unique_ptr<ArrayInitVal> ArrayInitVal::parse(bool cons) {
    auto n = std::make_unique<ArrayInitVal>();
    n->cons = cons;
    singleLex(NodeType::LBRACE);

    if (curLexType != NodeType::RBRACE) {
        n->array.push_back(InitVal::parse(cons));

        while (curLexType == NodeType::COMMA) {
            Lexer::next();
            n->array.push_back(InitVal::parse(cons));
        }
    }

    singleLex(NodeType::RBRACE);
    return n;
}
