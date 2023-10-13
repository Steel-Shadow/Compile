//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Func.h"
#include "parser/decl/Decl.h"
#include "parser/Parser.h"
#include "error/Error.h"

using namespace Parser;

std::unique_ptr<FuncDef> FuncDef::parse() {
    auto n = std::make_unique<FuncDef>();

    n->funcType = FuncType::parse();
    n->ident = Ident::parse();

    singleLex(NodeType::LPARENT);
    if (curLexType != NodeType::RPARENT) {
        n->funcFParams = FuncFParams::parse();
    }
    singleLex(NodeType::RPARENT);

    n->block = Block::parse();

    output(NodeType::FuncDef);
    return n;
}

std::unique_ptr<MainFuncDef> MainFuncDef::parse() {
    auto n = std::make_unique<MainFuncDef>();

    singleLex(NodeType::INTTK);
    singleLex(NodeType::MAINTK);
    singleLex(NodeType::LPARENT);
    singleLex(NodeType::RPARENT);

    n->block = Block::parse();

    output(NodeType::MainFuncDef);
    return n;
}

std::unique_ptr<FuncType> FuncType::parse() {
    auto n = std::make_unique<FuncType>();

    if (curLexType == NodeType::VOIDTK || curLexType == NodeType::INTTK) {
        n->type = curLexType;
        lexer.next();
    } else { Error::raise_error(); }

    output(NodeType::FuncType);
    return n;
}

std::unique_ptr<FuncFParams> FuncFParams::parse() {
    auto n = std::make_unique<FuncFParams>();

    n->funcFParams.push_back(FuncFParam::parse());

    while (curLexType == NodeType::COMMA) {
        lexer.next();
        n->funcFParams.push_back(FuncFParam::parse());
    }

    output(NodeType::FuncFParams);
    return n;
}

std::unique_ptr<FuncFParam> FuncFParam::parse() {
    auto n = std::make_unique<FuncFParam>();

    n->type = Btype::parse();
    n->ident = Ident::parse();

    // ['[' ']' { '[' ConstExp ']' }]
    if (curLexType == NodeType::LBRACK) {
        lexer.next();

        singleLex(NodeType::RBRACK);

        while (curLexType == NodeType::LBRACK) {
            lexer.next();

            n->dims.push_back(Exp::parse(true));
            singleLex(NodeType::RBRACK);
        }
    }

    output(NodeType::FuncFParam);
    return n;
}

std::unique_ptr<FuncRParams> FuncRParams::parse() {
    auto n = std::make_unique<FuncRParams>();

    n->params.push_back(Exp::parse(false));
    while (curLexType == NodeType::COMMA) {
        lexer.next();

        n->params.push_back(Exp::parse(false));
    }

    output(NodeType::FuncRParams);
    return n;
}
