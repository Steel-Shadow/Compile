//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Exp.h"
#include "frontend/parser/Parser.h"
#include "frontend/error/Error.h"

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

int Exp::evaluate() const {
    if (!cons) {
//        Error::raise("evaluate non-const Exp");
    }
    return addExp->evaluate();
}

size_t Exp::getRank() const {
    return addExp->getRank();
}

std::string Exp::getIdent() const {
    return addExp->getIdent();
}

std::unique_ptr<IR::Temp> Exp::genIR(IR::BasicBlocks &bBlocks) const {
    return addExp->genIR(bBlocks);
}

LVal *Exp::getLVal() const {
    return addExp->getLVal();
}

int BaseUnaryExp::evaluate() {
    Error::raise("evaluate non-Number BaseUnaryExp");
    return 0;
}
