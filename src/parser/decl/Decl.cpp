//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Decl.h"

#include "Def.h"
#include "error/Error.h"
#include "Compiler.h"

using namespace Parser;
using namespace Lexer;

std::unique_ptr<Decl> Decl::parse() {
    auto n = std::make_unique<Decl>();

    if (curLexType == NodeType::CONSTTK) {
        Lexer::next();
        n->cons = true;
    } else { n->cons = false; }

    n->btype = Btype::parse();

    n->defs.push_back(Def::parse(n->cons));

    while (curLexType == NodeType::COMMA) {
        Lexer::next();
        n->defs.push_back(Def::parse(n->cons));
    }

    singleLex(NodeType::SEMICN);

    if (n->cons) {
        output(NodeType::ConstDecl);
    } else {
        output(NodeType::VarDecl);
    }
//    output(NodeType::Decl);
    return n;
}

std::unique_ptr<Btype> Btype::parse() {
    auto n = std::make_unique<Btype>();

    singleLex(NodeType::INTTK);
    n->type = NodeType::INTTK;
//    output(NodeType::Btype);
    return n;
}

std::string Ident::parse() {
    std::string ident;

    if (curLexType == NodeType::IDENFR) {
        ident = Lexer::peek().second;
        Lexer::next();
    } else { Error::raise_error(); }

    return ident;
}
