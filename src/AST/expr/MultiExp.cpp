//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"
#include "frontend/parser/Parser.h"

using namespace Parser;

std::unique_ptr<Cond> Cond::parse() {
    auto n = std::make_unique<Cond>();

    n->lorExp = LOrExp::parse();

    output(NodeType::Cond);
    return n;
}

void Cond::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    lorExp->genIR(basicBlocks, trueBranch, falseBranch);
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

int MulExp::evaluate() const {
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

int AddExp::evaluate() const {
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

void EqExp::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    auto t = MultiExp::genIR(basicBlocks);
    basicBlocks.back()->addInst(IR::Inst(IR::Op::Bif1,
                                         nullptr,
                                         std::move(t),
                                         std::make_unique<IR::Label>(trueBranch)));
    basicBlocks.back()->addInst(IR::Inst(IR::Op::Br,
                                         nullptr,
                                         std::make_unique<IR::Label>(falseBranch),
                                         nullptr));
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

void LAndExp::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    if (elements.empty()) {
        first->genIR(basicBlocks, trueBranch, falseBranch);
    } else {
        auto firstBasicBlock = std::make_unique<IR::BasicBlock>("EqExp");
        first->genIR(basicBlocks, firstBasicBlock->label, falseBranch);
        basicBlocks.emplace_back(std::move(firstBasicBlock));

        for (auto e = elements.begin(); e != std::prev(elements.end()); ++e) {
            auto newBasicBlock = std::make_unique<IR::BasicBlock>("EqExp");
            (*e)->genIR(basicBlocks, newBasicBlock->label, falseBranch);
            basicBlocks.emplace_back(std::move(newBasicBlock));
        }

        elements.back()->genIR(basicBlocks, trueBranch, falseBranch);
    }
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

void LOrExp::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    if (elements.empty()) {
        first->genIR(basicBlocks, trueBranch, falseBranch);
    } else {
        auto firstBasicBlock = std::make_unique<IR::BasicBlock>("LAndExp");
        first->genIR(basicBlocks, trueBranch, firstBasicBlock->label);
        basicBlocks.emplace_back(std::move(firstBasicBlock));

        for (int i = 0; !elements.empty() && i < elements.size() - 1; ++i) {
            auto newBasicBlock = std::make_unique<IR::BasicBlock>("LAndExp");
            elements[i]->genIR(basicBlocks, trueBranch, newBasicBlock->label);
            basicBlocks.emplace_back(std::move(newBasicBlock));
        }

        elements.back()->genIR(basicBlocks, trueBranch, falseBranch);
    }
}
