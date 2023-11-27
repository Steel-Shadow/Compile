//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_EXP_H
#define COMPILER_EXP_H

#include "frontend/lexer/NodeType.h"
#include "frontend/symTab/Symbol.h"
#include "middle/IR.h"

#include <memory>
#include <vector>

struct FuncRParams;
struct Exp;

struct BaseUnaryExp {
    virtual ~BaseUnaryExp() = default;

    // override in Number
    virtual int evaluate();

    virtual std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) = 0;
};

// PrimaryExp → '(' Exp ')' | LVal | Number
struct PrimaryExp : public BaseUnaryExp {
    static std::unique_ptr<PrimaryExp> parse();

    virtual size_t getRank();

    virtual std::string getIdent();
};

// LVal → Ident {'[' Exp ']'}
struct LVal : public PrimaryExp {
    std::string ident;
    std::vector<std::unique_ptr<Exp>> dims;

    static std::unique_ptr<LVal> parse();

    size_t getRank() override;

    std::string getIdent() override;

    // Load a Var to Temp
    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) override;

    int evaluate() override;
};

// UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
// change syntax to
// UnaryExp → {UnaryOp} ( PrimaryExp | Ident '(' [FuncRParams] ')' )
// Note: UnaryOp is not a separate class!
struct UnaryExp {
    std::vector<NodeType> ops;
    std::unique_ptr<BaseUnaryExp> baseUnaryExp;

    static std::unique_ptr<UnaryExp> parse();

    int evaluate() const;

    size_t getRank() const;

    std::string getIdent() const;

    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) const;

    LVal *getLVal() const;
};

// Ident '(' [FuncRParams] ')'
struct FuncCall : public BaseUnaryExp {
    std::string ident;
    std::unique_ptr<FuncRParams> funcRParams;

    const std::string &getIdent() const;

    static std::unique_ptr<FuncCall> parse();

    static void checkParams(const std::unique_ptr<FuncCall> &n, int row, const Symbol *funcSym);

    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) override;
};

struct PareExp : public PrimaryExp {
    // '(' Exp ')'
    std::unique_ptr<Exp> exp;

    static std::unique_ptr<PareExp> parse();

    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) override;

    int evaluate() override;
};

// Number → IntConst
struct Number : public PrimaryExp {
    int intConst{};

    static std::unique_ptr<Number> parse();

    int evaluate() override;

    // load immediate number in a separate MIPS instruction
    // can be optimized into calculate instruction (backend)
    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) override;
};

template<class T>
struct MultiExp {
    std::unique_ptr<T> first;
    std::vector<NodeType> ops;
    std::vector<std::unique_ptr<T>> elements;

    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) const {
        using namespace IR;
        auto lastRes = first->genIR(bBlocks);
        for (int i = 0; i < ops.size(); i++) {
            auto &e = elements[i];
            auto t = e->genIR(bBlocks);
            auto res = std::make_unique<Temp>();
            auto resCopy = std::make_unique<Temp>(*res);
            bBlocks.back()->addInst(Inst(
                    NodeTypeToIROp(ops[i]),
                    std::move(res),
                    std::move(lastRes),
                    std::move(t)
                    ));
            lastRes = std::move(resCopy);
        }
        return lastRes;
    }

    std::string getIdent() {
        if (!elements.empty()) {
            return "";
        }
        return first->getIdent();
    }

    LVal *getLVal() const {
        if (!elements.empty()) {
            return nullptr;
        }
        return first->getLVal();
    }

    size_t getRank() {
        if (!elements.empty()) {
            return 0;
        }
        return first->getRank();
    }
};

// MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
struct MulExp : MultiExp<UnaryExp> {
    static std::unique_ptr<MulExp> parse();

    int evaluate() const;
};

// AddExp → MulExp | AddExp ('+' | '−') MulExp
struct AddExp : MultiExp<MulExp> {
    static std::unique_ptr<AddExp> parse();

    int evaluate() const;
};

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
struct RelExp : MultiExp<AddExp> {
    static std::unique_ptr<RelExp> parse();
};

// EqExp → RelExp | EqExp ('==' | '!=') RelExp
struct EqExp : MultiExp<RelExp> {
    static std::unique_ptr<EqExp> parse();
    void genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const;
};

// LAndExp → EqExp | LAndExp '&&' EqExp
struct LAndExp : MultiExp<EqExp> {
    static std::unique_ptr<LAndExp> parse();
    void genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const;
};

// LOrExp → LAndExp | LOrExp '||' LAndExp
struct LOrExp : MultiExp<LAndExp> {
    static std::unique_ptr<LOrExp> parse();
    void genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const;
};

// Cond → LOrExp
struct Cond {
    std::unique_ptr<LOrExp> lorExp;

    static std::unique_ptr<Cond> parse();

    void genIR(IR::BasicBlocks &basicBlocks, IR::Label &trueBranch, IR::Label &falseBranch) const;
};

// Exp → AddExp
// ConstExp → AddExp
// Ident in ConstExp must be const.
struct Exp {
    bool cons;

    std::unique_ptr<AddExp> addExp;

    static std::unique_ptr<Exp> parse(bool cons);

    int evaluate() const;

    // get the indexRank of LVal
    // 0 if the Exp is not a single LVal
    size_t getRank() const;

    // get ident of LVal or FuncCall
    // if the Exp is not a single LVal or FuncCall, return ""
    std::string getIdent() const;

    std::unique_ptr<IR::Temp> genIR(IR::BasicBlocks &bBlocks) const;

    // FuncCall rParam is an array
    LVal *getLVal() const;
};

#endif  // COMPILER_EXP_H
