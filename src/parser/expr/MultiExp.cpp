//
// Created by Steel_Shadow on 2023/10/12.
//
#include "lexer/NodeType.h"
#include "parser/Parser.h"
#include "Exp.h"

using namespace Parser;

std::unique_ptr<Cond> Cond::parse(bool cons) {
    auto n = std::make_unique<Cond>();

    n->lorExp = LOrExp::parse(cons);

    output(NodeType::Cond);
    return n;
}

std::unique_ptr<MulExp> MulExp::parse(bool cons) {
    auto n = std::make_unique<MulExp>();
    n->cons = cons;

    n->first = UnaryExp::parse(cons);
    output(NodeType::MulExp);

    while (curLexType == NodeType::MULT || curLexType == NodeType::DIV || curLexType == NodeType::MOD) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(UnaryExp::parse(cons));
        output(NodeType::MulExp);
    }

    return n;
}

std::unique_ptr<AddExp> AddExp::parse(bool cons) {
    auto n = std::make_unique<AddExp>();
    n->cons = cons;

    n->first = MulExp::parse(cons);
    output(NodeType::AddExp);

    while (curLexType == NodeType::PLUS || curLexType == NodeType::MINU) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(MulExp::parse(cons));
        output(NodeType::AddExp);
    }

    return n;
}

std::unique_ptr<RelExp> RelExp::parse(bool cons) {
    auto n = std::make_unique<RelExp>();
    n->cons = cons;

    n->first = AddExp::parse(cons);
    output(NodeType::RelExp);

    while (curLexType == NodeType::LSS || curLexType == NodeType::GRE ||
           curLexType == NodeType::LEQ || curLexType == NodeType::GEQ) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(AddExp::parse(cons));
        output(NodeType::RelExp);
    }

    return n;
}

std::unique_ptr<EqExp> EqExp::parse(bool cons) {
    auto n = std::make_unique<EqExp>();
    n->cons = cons;

    n->first = RelExp::parse(cons);
    output(NodeType::EqExp);

    while (curLexType == NodeType::EQL || curLexType == NodeType::NEQ) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(RelExp::parse(cons));
        output(NodeType::EqExp);
    }

    return n;
}

std::unique_ptr<LAndExp> LAndExp::parse(bool cons) {
    auto n = std::make_unique<LAndExp>();
    n->cons = cons;

    n->first = EqExp::parse(cons);
    output(NodeType::LAndExp);

    while (curLexType == NodeType::AND) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(EqExp::parse(cons));
        output(NodeType::LAndExp);
    }

    return n;
}

std::unique_ptr<LOrExp> LOrExp::parse(bool cons) {
    auto n = std::make_unique<LOrExp>();
    n->cons = cons;

    n->first = LAndExp::parse(cons);
    output(NodeType::LOrExp);

    while (curLexType == NodeType::OR) {
        n->ops.push_back(curLexType);
        lexer.next();
        n->elements.push_back(LAndExp::parse(cons));
        output(NodeType::LOrExp);
    }

    return n;
}
