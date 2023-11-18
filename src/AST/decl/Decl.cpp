//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Decl.h"

#include "Def.h"
#include "frontend/error/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

std::unique_ptr<Decl> Decl::parse() {
    auto n = std::make_unique<Decl>();

    if (Lexer::curLexType == NodeType::CONSTTK) {
        Lexer::next();
        n->cons = true;
    } else {
        n->cons = false;
    }

    n->btype = Btype::parse();

    int row = Lexer::curRow;
    n->defs.push_back(Def::parse(n->cons));

    while (Lexer::curLexType == NodeType::COMMA) {
        Lexer::next();
        row = Lexer::curRow;
        n->defs.push_back(Def::parse(n->cons));
    }

    singleLex(NodeType::SEMICN, row);

    if (n->cons) {
        output(NodeType::ConstDecl);
    } else {
        output(NodeType::VarDecl);
    }
    //    output(NodeType::Decl);
    return n;
}

const std::vector<std::unique_ptr<Def>>& Decl::getDefs() const {
    return defs;
}

void Decl::genIR(IR::BasicBlocks& bBlocks) {
    using namespace IR;
    for (auto& def : defs) {
        def->genIR(bBlocks);
    }
}

void Def::genIR(IR::BasicBlocks& bBlocks) {
    using namespace IR;

    auto var = std::make_unique<Var>(
        ident,
        SymTab::cur->getDepth(),
        cons,
        SymTab::find(ident)->dims
    );

    auto size = std::make_unique<ConstVal>(getArraySize());
    bBlocks.back()->addInst(Inst(
        Op::Alloca,
        nullptr,
        std::make_unique<Var>(*var),
        std::move(size)
    ));

    if (initVal) {
        if (dims.empty()) {
            // single value assign
            auto* p = dynamic_cast<ExpInitVal*>(initVal.get());
            auto value = p->exp->genIR(bBlocks);

            bBlocks.back()->addInst(Inst(
                Op::Assign,
                std::move(var),
                std::move(value),
                nullptr
            ));
        } else {
            // array init
            auto array = dynamic_cast<ArrayInitVal*>(initVal.get());

            int index = 0;
            for (auto& expInit : array->getFlatten()) {
                auto value = expInit->exp->genIR(bBlocks);

                index++;
                bBlocks.back()->addInst(Inst(
                    Op::Store,
                    std::move(value),
                    std::move(var),
                    std::make_unique<ConstVal>(index)));
            }
        }
    }
}

int Def::getArraySize() const {
    auto symbol = SymTab::find(ident);
    int size = 1;
    for (const auto& i : symbol->dims) {
        size *= i;
    }
    return size;
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

    if (Lexer::curLexType == NodeType::IDENFR) {
        ident = Lexer::curToken;
        Lexer::next();
    } else {
        Error::raise();
    }

    return ident;
}
