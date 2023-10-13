//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_EXP_H
#define COMPILER_EXP_H

#include <memory>
#include <vector>
#include "lexer/NodeType.h"

class FuncRParams;

class BaseUnaryExp {
public:
    virtual ~BaseUnaryExp() = default;
};

// UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
// change syntax to
// UnaryExp → {UnaryOp} ( PrimaryExp | Ident '(' [FuncRParams] ')' )
// Note: UnaryOp is not a separate class!
class UnaryExp {
    bool cons;

    std::vector<NodeType> unaryOps;
    std::unique_ptr<BaseUnaryExp> baseUnaryExp;

public:
    static std::unique_ptr<UnaryExp> parse(bool cons);
};

// PrimaryExp → '(' Exp ')' | LVal | Number
class PrimaryExp : public BaseUnaryExp {
protected:
    bool cons;

public:
    static std::unique_ptr<PrimaryExp> parse(bool cons);
};

// Ident '(' [FuncRParams] ')'
class FuncCall : public BaseUnaryExp {
    std::string ident;
    std::unique_ptr<FuncRParams> funcRParams;
public:
    static std::unique_ptr<FuncCall> parse();
};

class Exp;

// '(' Exp ')'
class PareExp : public PrimaryExp {
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<PareExp> parse(bool cons);
};

// LVal → Ident {'[' Exp ']'}
class LVal : public PrimaryExp {
    std::string ident;
    std::vector<std::unique_ptr<Exp>> dims;

public:
    static std::unique_ptr<LVal> parse();
};

// Number → IntConst
class Number : public PrimaryExp {
    int intConst;

public:
    static std::unique_ptr<Number> parse();
};

template<class T>
class MultiExp {
protected:
    bool cons{false};
    std::unique_ptr<T> first;
    std::vector<NodeType> ops;
    std::vector<std::unique_ptr<T>> elements;
    //we don't need the destructor
    //public:
    //    virtual ~MultiExp() = default;
};

// MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
class MulExp : MultiExp<UnaryExp> {
public:
    static std::unique_ptr<MulExp> parse(bool cons);
};

// AddExp → MulExp | AddExp ('+' | '−') MulExp
class AddExp : MultiExp<MulExp> {
public:
    static std::unique_ptr<AddExp> parse(bool cons);
};

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
class RelExp : MultiExp<AddExp> {
public:
    static std::unique_ptr<RelExp> parse(bool cons);
};

// EqExp → RelExp | EqExp ('==' | '!=') RelExp
class EqExp : MultiExp<RelExp> {
public:
    static std::unique_ptr<EqExp> parse(bool cons);
};

// LAndExp → EqExp | LAndExp '&&' EqExp
class LAndExp : MultiExp<EqExp> {
public:
    static std::unique_ptr<LAndExp> parse(bool cons);
};

// LOrExp → LAndExp | LOrExp '||' LAndExp
class LOrExp : MultiExp<LAndExp> {
public:
    static std::unique_ptr<LOrExp> parse(bool cons);
};

// Cond → LOrExp
class Cond {
    std::unique_ptr<LOrExp> lorExp;

public:
    static std::unique_ptr<Cond> parse(bool cons);
};

// Exp → AddExp
class Exp {
protected:
    bool cons;

private:
    std::unique_ptr<AddExp> addExp;

public:
    static std::unique_ptr<Exp> parse(bool cons);
};


#endif //COMPILER_EXP_H
