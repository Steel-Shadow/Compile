//
// Created by Steel_Shadow on 2023/10/12.
//
#include "lexer/NodeType.h"
#include "Exp.h"

#include "Compiler.h"

using namespace Parser;
using namespace Lexer;

std::unique_ptr<Cond> Cond::parse() {
    auto n = std::make_unique<Cond>();

    n->lorExp = LOrExp::parse();

    output(NodeType::Cond);
    return n;
}

std::unique_ptr<MulExp> MulExp::parse() {
    auto n = std::make_unique<MulExp>();

    n->first = UnaryExp::parse();
    output(NodeType::MulExp);

    while (curLexType == NodeType::MULT || curLexType == NodeType::DIV || curLexType == NodeType::MOD) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(UnaryExp::parse());
        output(NodeType::MulExp);
    }

    return n;
}

std::unique_ptr<AddExp> AddExp::parse() {
    auto n = std::make_unique<AddExp>();

    n->first = MulExp::parse();
    output(NodeType::AddExp);

    while (curLexType == NodeType::PLUS || curLexType == NodeType::MINU) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(MulExp::parse());
        output(NodeType::AddExp);
    }

    return n;
}

std::unique_ptr<RelExp> RelExp::parse() {
    auto n = std::make_unique<RelExp>();

    n->first = AddExp::parse();
    output(NodeType::RelExp);

    while (curLexType == NodeType::LSS || curLexType == NodeType::GRE ||
           curLexType == NodeType::LEQ || curLexType == NodeType::GEQ) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(AddExp::parse());
        output(NodeType::RelExp);
    }

    return n;
}

std::unique_ptr<EqExp> EqExp::parse() {
    auto n = std::make_unique<EqExp>();

    n->first = RelExp::parse();
    output(NodeType::EqExp);

    while (curLexType == NodeType::EQL || curLexType == NodeType::NEQ) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(RelExp::parse());
        output(NodeType::EqExp);
    }

    return n;
}

std::unique_ptr<LAndExp> LAndExp::parse() {
    auto n = std::make_unique<LAndExp>();

    n->first = EqExp::parse();
    output(NodeType::LAndExp);

    while (curLexType == NodeType::AND) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(EqExp::parse());
        output(NodeType::LAndExp);
    }

    return n;
}

std::unique_ptr<LOrExp> LOrExp::parse() {
    auto n = std::make_unique<LOrExp>();

    n->first = LAndExp::parse();
    output(NodeType::LOrExp);

    while (curLexType == NodeType::OR) {
        n->ops.push_back(curLexType);
        Lexer::next();
        n->elements.push_back(LAndExp::parse());
        output(NodeType::LOrExp);
    }

    return n;
}
