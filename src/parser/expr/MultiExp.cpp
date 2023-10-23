//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"

#include "parser/Parser.h"

using namespace Parser;

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

    while (Lexer::curLexType == NodeType::MULT || Lexer::curLexType == NodeType::DIV ||
           Lexer::curLexType == NodeType::MOD) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(UnaryExp::parse());
        output(NodeType::MulExp);
    }

    return n;
}

int MulExp::evaluate() {
    int val = first->evaluate();
    for (int i = 0; i < ops.size(); ++i) {
        auto op = ops[i];
        auto e = elements[i]->evaluate();
        if (op == NodeType::MULT) {
            val *= e;
        } else if (op == NodeType::DIV) {
            val /= e;
        } else if (op == NodeType::MOD) {
            val %= e;
        }
    }
    return val;
}

size_t MulExp::getRank() {
    if (!elements.empty()) {
        return 0;
    }
    return first->getRank();
}

std::string MulExp::getIdent() {
    if (!elements.empty()) {
        return "";
    }
    return first->getIdent();
}

std::unique_ptr<AddExp> AddExp::parse() {
    auto n = std::make_unique<AddExp>();

    n->first = MulExp::parse();
    output(NodeType::AddExp);

    while (Lexer::curLexType == NodeType::PLUS || Lexer::curLexType == NodeType::MINU) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(MulExp::parse());
        output(NodeType::AddExp);
    }

    return n;
}

int AddExp::evaluate() {
    int val = first->evaluate();
    for (int i = 0; i < ops.size(); ++i) {
        auto op = ops[i];
        auto e = elements[i]->evaluate();
        if (op == NodeType::PLUS) {
            val += e;
        } else if (op == NodeType::MINU) {
            val -= e;
        }
    }
    return val;
}

size_t AddExp::getRank() {
    if (!elements.empty()) {
        return 0;
    }
    return first->getRank();
}

std::string AddExp::getIdent() {
    if (!elements.empty()) {
        return "";
    }
    return first->getIdent();
}

std::unique_ptr<RelExp> RelExp::parse() {
    auto n = std::make_unique<RelExp>();

    n->first = AddExp::parse();
    output(NodeType::RelExp);

    while (Lexer::curLexType == NodeType::LSS || Lexer::curLexType == NodeType::GRE ||
           Lexer::curLexType == NodeType::LEQ || Lexer::curLexType == NodeType::GEQ) {
        n->ops.push_back(Lexer::curLexType);
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

    while (Lexer::curLexType == NodeType::EQL || Lexer::curLexType == NodeType::NEQ) {
        n->ops.push_back(Lexer::curLexType);
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

    while (Lexer::curLexType == NodeType::AND) {
        n->ops.push_back(Lexer::curLexType);
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

    while (Lexer::curLexType == NodeType::OR) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(LAndExp::parse());
        output(NodeType::LOrExp);
    }

    return n;
}
