//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Def.h"

#include "Decl.h"
#include "parser/Parser.h"
#include "symTab/SymTab.h"
#include "error/Error.h"

using namespace Parser;

std::unique_ptr<Def> Def::parse(bool cons) {
    auto n = std::make_unique<Def>();
    n->cons = cons;

    int row = Lexer::curRow;
    n->ident = Ident::parse();

    if (SymTab::reDefine(n->ident)) {
        Error::raise('b', row);
    }

    std::vector<int> dims; // SymTab dims

    while (Lexer::curLexType == NodeType::LBRACK) {
        Lexer::next();

        row = Lexer::curRow;
        auto exp = Exp::parse(true);
        dims.push_back(exp->evaluate());
        n->dims.push_back(std::move(exp));

        singleLex(NodeType::RBRACK, row);
    }

    if (cons || Lexer::curLexType == NodeType::ASSIGN) {
        Lexer::next();
        n->initVal = InitVal::parse(cons);
    }

    SymTab::add(n->ident, n->cons, dims);

    if (cons) {
        output(NodeType::ConstDef);
    } else {
        output(NodeType::VarDef);
    }
    return n;
}
