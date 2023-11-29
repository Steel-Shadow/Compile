//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"
#include "frontend/parser/Parser.h"

using namespace Parser;

std::unique_ptr<Cond> Cond::parse() {
    auto n = std::make_unique<Cond>();

    n->lorExp = LOrExp::parse();

    output(AST::Cond);
    return n;
}

void Cond::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    lorExp->genIR(basicBlocks, trueBranch, falseBranch);
}

std::unique_ptr<MulExp> MulExp::parse() {
    auto n = std::make_unique<MulExp>();

    n->first = UnaryExp::parse();
    output(AST::MulExp);

    while (Lexer::curLexType == LexType::MULT || Lexer::curLexType == LexType::DIV ||
           Lexer::curLexType == LexType::MOD) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(UnaryExp::parse());
        output(AST::MulExp);
    }

    return n;
}

int MulExp::evaluate() const {
    int val = first->evaluate();
    for (int i = 0; i < ops.size(); ++i) {
        auto op = ops[i];
        auto e = elements[i]->evaluate();
        if (op == LexType::MULT) {
            val *= e;
        } else if (op == LexType::DIV) {
            val /= e;
        } else if (op == LexType::MOD) {
            val %= e;
        }
    }
    return val;
}

std::unique_ptr<AddExp> AddExp::parse() {
    auto n = std::make_unique<AddExp>();

    n->first = MulExp::parse();
    output(AST::AddExp);

    while (Lexer::curLexType == LexType::PLUS || Lexer::curLexType == LexType::MINU) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(MulExp::parse());
        output(AST::AddExp);
    }

    return n;
}

int AddExp::evaluate() const {
    int val = first->evaluate();
    for (int i = 0; i < ops.size(); ++i) {
        auto op = ops[i];
        auto e = elements[i]->evaluate();
        if (op == LexType::PLUS) {
            val += e;
        } else if (op == LexType::MINU) {
            val -= e;
        }
    }
    return val;
}

std::unique_ptr<RelExp> RelExp::parse() {
    auto n = std::make_unique<RelExp>();

    n->first = AddExp::parse();
    output(AST::RelExp);

    while (Lexer::curLexType == LexType::LSS || Lexer::curLexType == LexType::GRE ||
           Lexer::curLexType == LexType::LEQ || Lexer::curLexType == LexType::GEQ) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(AddExp::parse());
        output(AST::RelExp);
    }

    return n;
}

std::unique_ptr<EqExp> EqExp::parse() {
    auto n = std::make_unique<EqExp>();

    n->first = RelExp::parse();
    output(AST::EqExp);

    while (Lexer::curLexType == LexType::EQL || Lexer::curLexType == LexType::NEQ) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(RelExp::parse());
        output(AST::EqExp);
    }

    return n;
}

void EqExp::genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const {
    auto t = MultiExp::genIR(basicBlocks);
    // todo: 中间代码优化:哪种写法好呢？
    // 目标代码优化：合并最后一条计算指令与跳转指令
    basicBlocks.back()->addInst(IR::Inst(IR::Op::Bif1,
                                         nullptr,
                                         std::move(t),
                                         std::make_unique<IR::Label>(trueBranch)));
    basicBlocks.back()->addInst(IR::Inst(IR::Op::Br,
                                         nullptr,
                                         std::make_unique<IR::Label>(falseBranch),
                                         nullptr));
    // basicBlocks.back()->addInst(IR::Inst(IR::Op::Bif0,
    //                                      nullptr,
    //                                      std::move(t),
    //                                      std::make_unique<IR::Label>(falseBranch)));
    // basicBlocks.back()->addInst(IR::Inst(IR::Op::Br,
    //                                      nullptr,
    //                                      std::make_unique<IR::Label>(trueBranch),
    //                                      nullptr));
}

std::unique_ptr<LAndExp> LAndExp::parse() {
    auto n = std::make_unique<LAndExp>();

    n->first = EqExp::parse();
    output(AST::LAndExp);

    while (Lexer::curLexType == LexType::AND) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(EqExp::parse());
        output(AST::LAndExp);
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
    output(AST::LOrExp);

    while (Lexer::curLexType == LexType::OR) {
        n->ops.push_back(Lexer::curLexType);
        Lexer::next();
        n->elements.push_back(LAndExp::parse());
        output(AST::LOrExp);
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
