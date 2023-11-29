//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Decl.h"

#include "Def.h"
#include "errorHandler/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

std::unique_ptr<Decl> Decl::parse() {
    auto n = std::make_unique<Decl>();

    if (Lexer::curLexType == LexType::CONSTTK) {
        Lexer::next();
        n->cons = true;
    } else {
        n->cons = false;
    }

    n->btype = Btype::parse();

    int row = Lexer::curRow;
    n->defs.push_back(Def::parse(n->cons, toType(n->btype->type)));

    while (Lexer::curLexType == LexType::COMMA) {
        Lexer::next();
        row = Lexer::curRow;
        n->defs.push_back(Def::parse(n->cons, toType(n->btype->type)));
    }

    singleLex(LexType::SEMICN, row);

    if (n->cons) {
        output(AST::ConstDecl);
    } else {
        output(AST::VarDecl);
    }
    //    output(NodeType::Decl);
    return n;
}

const std::vector<std::unique_ptr<Def>> &Decl::getDefs() const {
    return defs;
}

void Decl::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    for (auto &def: defs) {
        def->genIR(bBlocks, toType(btype->type));
    }
}

int Def::getArraySize() const {
    auto symbol = SymTab::find(ident);
    int size = 1;
    for (const auto &i: symbol->dims) {
        size *= i;
    }
    return size;
}

std::unique_ptr<Btype> Btype::parse() {
    auto n = std::make_unique<Btype>();

    singleLex(LexType::INTTK);
    n->type = LexType::INTTK;
    //    output(NodeType::Btype);
    return n;
}

std::string Ident::parse() {
    std::string ident;

    if (Lexer::curLexType == LexType::IDENFR) {
        ident = Lexer::curToken;
        Lexer::next();
    } else {
        Error::raise();
    }

    return ident;
}
