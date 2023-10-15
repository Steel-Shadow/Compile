//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Func.h"
#include "error/Error.h"

#include "parser/Parser.h"
#include "lexer/Lexer.h"

using namespace Parser;

std::unique_ptr<FuncDef> FuncDef::parse() {
    auto n = std::make_unique<FuncDef>();

    n->funcType = FuncType::parse();
    n->ident = Ident::parse();

    singleLex(NodeType::LPARENT);
    if (Lexer::curLexType != NodeType::RPARENT) {
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

    if (Lexer::curLexType == NodeType::VOIDTK || Lexer::curLexType == NodeType::INTTK) {
        n->type = Lexer::curLexType;
        Lexer::next();
    } else { Error::raise_error(); }

    output(NodeType::FuncType);
    return n;
}

std::unique_ptr<FuncFParams> FuncFParams::parse() {
    auto n = std::make_unique<FuncFParams>();

    n->funcFParams.push_back(FuncFParam::parse());

    while (Lexer::curLexType == NodeType::COMMA) {
        Lexer::next();
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
    if (Lexer::curLexType == NodeType::LBRACK) {
        Lexer::next();

        singleLex(NodeType::RBRACK);

        while (Lexer::curLexType == NodeType::LBRACK) {
            Lexer::next();

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
    while (Lexer::curLexType == NodeType::COMMA) {
        Lexer::next();

        n->params.push_back(Exp::parse(false));
    }

    output(NodeType::FuncRParams);
    return n;
}
