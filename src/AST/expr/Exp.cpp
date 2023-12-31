//
// Created by Steel_Shadow on 2023/10/11.
//
#include "Exp.h"
#include "errorHandler/Error.h"
#include "frontend/parser/Parser.h"


using namespace Parser;

std::unique_ptr<Exp> Exp::parse(bool cons) {
    auto n = std::make_unique<Exp>();
    n->cons = cons;
    n->addExp = AddExp::parse();

    if (cons) {
        output(AST::ConstExp);
    } else {
        output(AST::Exp);
    }
    return n;
}

bool Exp::getNonConstValueInEvaluate = false;

int Exp::evaluate() const {
    Exp::getNonConstValueInEvaluate = false;
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

Type Exp::getType() const {
    return addExp->getType();
}

LVal *Exp::getLVal() const {
    return addExp->getLVal();
}

int BaseUnaryExp::evaluate() {
    Exp::getNonConstValueInEvaluate = true;
    return 0;
}
