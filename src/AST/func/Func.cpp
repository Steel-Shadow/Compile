//
// Created by Steel_Shadow on 2023/10/11.
//

#include "Func.h"
#include "frontend/error/Error.h"

#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

// const array can't be param
std::unique_ptr<FuncDef> FuncDef::parse() {
    auto n = std::make_unique<FuncDef>();

    n->funcType = FuncType::parse();

    int row = Lexer::curRow;
    n->ident = Ident::parse();
    if (SymTab::reDefine(n->ident)) {
        Error::raise('b', row);
    }

    SymTab::deepIn();

    std::vector<Dimensions> params;

    singleLex(NodeType::LPARENT);
    if (Lexer::curLexType != NodeType::RPARENT) {
        n->funcFParams = FuncFParams::parse();
        params = n->funcFParams->getDimsVec();
    }
    singleLex(NodeType::RPARENT, row);

    SymTab::add(n->ident, n->funcType->getType(),
                params,
                SymTab::cur->getPrev());

    Stmt::retVoid = (n->funcType->getType() == NodeType::VOIDTK);
    n->block = Block::parse();

    if (!Stmt::retVoid) {
        if (n->block->getBlockItems().empty() ||
            !dynamic_cast<ReturnStmt*>(n->block->getBlockItems().back().get())) {
            // In fact, we should check "return;"
            // But it's not included in our work.
            Error::raise('g', Block::lastRow);
        }
    }

    SymTab::deepOut(); // FuncDef
    output(NodeType::FuncDef);
    return n;
}

std::unique_ptr<MainFuncDef> MainFuncDef::parse() {
    auto n = std::make_unique<MainFuncDef>();

    singleLex(NodeType::INTTK);
    singleLex(NodeType::MAINTK);

    SymTab::deepIn();

    int row = Lexer::curRow;
    singleLex(NodeType::LPARENT);
    singleLex(NodeType::RPARENT, row);

    Stmt::retVoid = false;
    n->block = Block::parse();

    if (!Stmt::retVoid) {
        if (n->block->getBlockItems().empty() ||
            !dynamic_cast<ReturnStmt*>(n->block->getBlockItems().back().get())) {
            // In fact, we should check "return;"
            // But it's not included in our work.
            Error::raise('g', Block::lastRow);
        }
    }

    SymTab::deepOut(); // MainFuncDef
    output(NodeType::MainFuncDef);
    return n;
}

std::unique_ptr<IR::Function> MainFuncDef::genIR() const {
    using namespace IR;
    auto function = std::make_unique<Function>("main", Function::convertType(NodeType::INTTK),
                                               std::vector<Dimensions>());

    SymTab::iterIn();
    Function::idAllocator = 0;
    BasicBlocks bBlocks;
    bBlocks.emplace_back(std::make_unique<BasicBlock>("main", true));
    block->genIR(bBlocks);
    function->setBasicBlocks(bBlocks);
    SymTab::iterOut();

    return function;
}

std::unique_ptr<FuncType> FuncType::parse() {
    auto n = std::make_unique<FuncType>();

    if (Lexer::curLexType == NodeType::VOIDTK || Lexer::curLexType == NodeType::INTTK) {
        n->type = Lexer::curLexType;
        Lexer::next();
    } else {
        Error::raise();
    }

    output(NodeType::FuncType);
    return n;
}

NodeType FuncType::getType() const {
    return type;
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

std::vector<Dimensions> FuncFParams::getDimsVec() const {
    std::vector<Dimensions> raws;
    raws.reserve(funcFParams.size());
    for (auto& i : funcFParams) {
        raws.push_back(i->getDims());
    }
    return raws;
}

std::unique_ptr<FuncFParam> FuncFParam::parse() {
    auto n = std::make_unique<FuncFParam>();

    n->type = Btype::parse();

    int row = Lexer::curRow; // error handle
    n->ident = Ident::parse();
    if (SymTab::reDefine(n->ident)) {
        Error::raise('b', row);
    }

    // ['[' ']' { '[' ConstExp ']' }]
    if (Lexer::curLexType == NodeType::LBRACK) {
        row = Lexer::curRow;
        Lexer::next();
        n->dims.push_back(nullptr); // p[] is not p!
        singleLex(NodeType::RBRACK, row);

        while (Lexer::curLexType == NodeType::LBRACK) {
            Lexer::next();

            row = Lexer::curRow;
            n->dims.push_back(Exp::parse(true));
            singleLex(NodeType::RBRACK, row);
        }
    }

    SymTab::add(n->getId(), n->getDims());
    output(NodeType::FuncFParam);
    return n;
}

const std::string& FuncFParam::getId() const {
    return ident;
}

std::vector<int> FuncFParam::getDims() {
    std::vector<int> dimsValue;
    dimsValue.reserve(dims.size());
    for (auto& i : dims) {
        dimsValue.push_back(i == nullptr ? 0 : i->evaluate());
    }
    return dimsValue;
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

std::unique_ptr<IR::Function> FuncDef::genIR() {
    using namespace IR;
    NodeType reType = funcType->getType();
    auto params = SymTab::find(ident)->params;
    auto function = std::make_unique<Function>(ident, Function::convertType(reType), params);

    BasicBlocks bBlocks;
    Function::idAllocator = 0;

    SymTab::iterIn();

    bBlocks.emplace_back(std::make_unique<BasicBlock>(ident, true));
    bBlocks.back()->addInst(Inst(Op::InStack, nullptr, nullptr, nullptr));
    block->genIR(bBlocks);

    bBlocks.back()->addInst(Inst(Op::OutStack, nullptr, nullptr, nullptr));
    SymTab::iterOut();

    function->setBasicBlocks(bBlocks);
    return function;
}
