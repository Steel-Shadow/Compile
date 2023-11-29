//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"
#include "AST/decl/Decl.h"
#include "AST/func/Func.h"
#include "backend/Register.h"
#include "errorHandler/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

std::unique_ptr<LVal> LVal::parse() {
    auto n = std::make_unique<LVal>();

    n->ident = Ident::parse();
    if (!SymTab::find(n->ident)) {
        Error::raise('c');
    }

    while (Lexer::curLexType == LexType::LBRACK) {
        Lexer::next();
        int row = Lexer::curRow;
        n->dims.push_back(Exp::parse(false));
        singleLex(LexType::RBRACK, row);
    }

    output(AST::LVal);
    return n;
}

size_t LVal::getRank() {
    return dims.size();
}

std::string LVal::getIdent() {
    return ident;
}

std::unique_ptr<IR::Temp> LVal::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto symbol = SymTab::find(ident);
    auto var = std::make_unique<IR::Var>(
        ident,
        SymTab::findDepth(ident),
        symbol->cons,
        symbol->dims,
        symbol->type);

    auto res = std::make_unique<Temp>(symbol->type);
    bBlocks.back()->addInst(Inst(IR::Op::Load,
                                 std::make_unique<Temp>(*res),
                                 std::move(var),
                                 nullptr
    ));
    //todo: 数组元素定位
    return res;
}

int LVal::evaluate() {
    auto sym = SymTab::find(ident);
    if (sym == nullptr) {
        Error::raise("LVal not found in evaluate()");
        return 0;
    } else if (!sym->cons) {
        Error::raise("Non-const LVal in evaluate()");
    } else if (sym->dims.empty()) {
        return sym->initVal[0];
    } else {
        Error::raise("Array element in evaluate()");
    }
    return 0;
}

std::unique_ptr<PrimaryExp> PrimaryExp::parse() {
    std::unique_ptr<PrimaryExp> n;
    if (Lexer::curLexType == LexType::LPARENT) {
        n = PareExp::parse();
    } else if (Lexer::curLexType == LexType::IDENFR) {
        n = LVal::parse();
    } else if (Lexer::curLexType == LexType::INTCON) {
        n = Number::parse();
    }
    output(AST::PrimaryExp);
    return n;
}

size_t PrimaryExp::getRank() {
    if (auto p = dynamic_cast<LVal *>(this)) {
        return p->getRank();
    }
    return 0;
}

std::string PrimaryExp::getIdent() {
    if (auto p = dynamic_cast<LVal *>(this)) {
        return p->getIdent();
    }
    return "";
}

std::unique_ptr<PareExp> PareExp::parse() {
    auto n = std::make_unique<PareExp>();

    Lexer::next();
    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(LexType::RPARENT, row);

    return n;
}

std::unique_ptr<IR::Temp> PareExp::genIR(IR::BasicBlocks &bBlocks) {
    return exp->genIR(bBlocks);
}

int PareExp::evaluate() {
    return exp->evaluate();
}

std::unique_ptr<Number> Number::parse() {
    auto n = std::make_unique<Number>();

    if (Lexer::curLexType == LexType::INTCON) {
        n->intConst = std::stoi(Lexer::curToken);
        Lexer::next();
    } else {
        Error::raise();
    }

    output(AST::Number);
    return n;
}

int Number::evaluate() {
    return intConst;
}

std::unique_ptr<IR::Temp> Number::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto n = std::make_unique<Temp>(Type::Int);

    bBlocks.back()->addInst(Inst(Op::LoadImd,
                                 std::make_unique<Temp>(*n),
                                 std::make_unique<ConstVal>(intConst, Type::Int),
                                 nullptr));
    return n;
}

std::unique_ptr<UnaryExp> UnaryExp::parse() {
    auto n = std::make_unique<UnaryExp>();

    bool getBaseUnaryExp = false;
    while (!getBaseUnaryExp) {
        // UnaryExp → {UnaryOp} ( PrimaryExp | Ident '(' [FuncRParams] ')' )
        switch (Lexer::curLexType) {
            case LexType::PLUS:
            case LexType::MINU:
            case LexType::NOT:
                // UnaryOp → '+' | '−' | '!'
                n->ops.push_back(Lexer::curLexType);
                Lexer::next();

                output(AST::UnaryOp);
                break;

            case LexType::LPARENT:
            case LexType::INTCON:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                n->baseUnaryExp = PrimaryExp::parse();
                getBaseUnaryExp = true;

                output(AST::UnaryExp);
                break;

            case LexType::IDENFR:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                // LVal → Ident {'[' Exp ']'}

                // Ident '(' [FuncRParams] ')'
                if (Lexer::peek(1).first == LexType::LPARENT) {
                    n->baseUnaryExp = FuncCall::parse();
                } else {
                    n->baseUnaryExp = PrimaryExp::parse();
                }
                getBaseUnaryExp = true;

                output(AST::UnaryExp);
                break;

            default:
                Error::raise();
        }
    }

    for (int i = 0; i < n->ops.size(); i++) {
        output(AST::UnaryExp);
    }

    return n;
}

int UnaryExp::evaluate() const {
    int val = baseUnaryExp->evaluate();

    for (auto op: ops) {
        if (op == LexType::MINU) {
            val = -val;
        }
    }

    return val;
}

size_t UnaryExp::getRank() const {
    if (auto p = dynamic_cast<PrimaryExp *>(baseUnaryExp.get())) {
        return p->getRank();
    }
    return 0;
}

std::string UnaryExp::getIdent() const {
    if (auto p = dynamic_cast<PrimaryExp *>(baseUnaryExp.get())) {
        return p->getIdent();
    }
    if (auto p = dynamic_cast<FuncCall *>(baseUnaryExp.get())) {
        return p->getIdent();
    }
    return "";
}

std::unique_ptr<IR::Temp> UnaryExp::genIR(IR::BasicBlocks &bBlocks) const {
    using namespace IR;
    auto res = baseUnaryExp->genIR(bBlocks);

    bool negative = false;
    for (LexType op: ops) {
        if (op == LexType::MINU) {
            negative = !negative;
        }
    }
    if (negative) {
        auto negRes = std::make_unique<Temp>(res->type);
        bBlocks.back()->addInst(Inst(
            Op::Neg,
            std::make_unique<Temp>(*negRes),
            std::move(res),
            nullptr
        ));
        return negRes;
    }

    return res;
}

LVal *UnaryExp::getLVal() const {
    if (auto lVal = dynamic_cast<LVal *>(baseUnaryExp.get())) {
        return lVal;
    }
    return nullptr;
}

std::unique_ptr<FuncCall> FuncCall::parse() {
    auto n = std::make_unique<FuncCall>();

    int row = Lexer::curRow;
    n->ident = Ident::parse();

    Symbol *funcSym = SymTab::find(n->ident);
    if (!funcSym) {
        Error::raise('c', row);
    }

    singleLex(LexType::LPARENT);

    if (Lexer::curLexType != LexType::RPARENT) {
        n->funcRParams = FuncRParams::parse();
        checkParams(n, row, funcSym); // SymTab error handle
    }

    singleLex(LexType::RPARENT, row);
    return n;
}

void FuncCall::checkParams(const std::unique_ptr<FuncCall> &n, int row, const Symbol *funcSym) {
    auto &realParams = n->funcRParams->params;
    // check number of realParams
    if (realParams.size() != funcSym->params.size()) {
        Error::raise('d', row);
    } else {
        // check type of params
        for (int i = 0; i < realParams.size(); i++) {
            auto &rParam = realParams[i];

            size_t formalRank = funcSym->params[i].second.size();
            size_t symRank;

            auto lVal = rParam->getLVal();
            size_t indexRank = lVal ? lVal->getRank() : 0;

            std::string ident = lVal ? lVal->getIdent() : "";
            if (ident.empty()) {
                // rParam is Exp (neither LVal nor FuncCall)
                symRank = 0;
            } else {
                auto sym = SymTab::find(ident);
                if (sym->symType == SymType::Func) {
                    // FuncCall
                    // void | int
                    symRank = sym->type == Type::Void ? -1 : 0;
                } else {
                    // LVal
                    symRank = sym->dims.size();
                }
            }

            if (symRank - indexRank != formalRank) {
                Error::raise('e', row);
            }
        }
    }
}

std::unique_ptr<IR::Temp> FuncCall::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto funcSym = SymTab::find(ident);

    if (funcRParams) {
        int i = 0;
        for (auto rParam = funcRParams->params.rbegin(); rParam != funcRParams->params.rend(); ++rParam, ++i) {
            auto name = (*rParam)->getIdent();

            auto symbol = SymTab::find(name); // LVal / FuncCall
            size_t formalRank = funcSym->params[i].second.size();

            if (formalRank == 0) {
                // single LVal (not array)
                // load Var from memory to Temp
                auto t = (*rParam)->genIR(bBlocks);
                bBlocks.back()->addInst(Inst(Op::PushParam,
                                             nullptr,
                                             std::move(t),
                                             nullptr));
            } else {
                // array (pass param by address)
                auto var = std::make_unique<Var>(
                    name,
                    SymTab::findDepth(name),
                    symbol->cons,
                    symbol->dims,
                    symbol->type);
                /*todo: 代码生成2 函数参数为数组
                 * int a[2][2] = {{1,2},{3,4}};
                 * void f(int p[]){}
                 * ...
                 * f(a[1]);
                 * 遍历实参下标，计算函数实参的offset
                 * 代码生成2
                 */
                LVal *lVar = (*rParam)->getLVal();
                int offset;
                bBlocks.back()->addInst(Inst(
                    Op::PushParam,
                    nullptr,
                    std::move(var),
                    std::make_unique<ConstVal>(offset, symbol->type)
                ));
            }
        }
    }

    bBlocks.back()->addInst(Inst(IR::Op::Call,
                                 nullptr,
                                 std::make_unique<Label>(ident, true),
                                 nullptr));

    if (funcSym->type == Type::Int) {
        auto temp = std::make_unique<Temp>(Type::Int);
        bBlocks.back()->addInst(Inst(IR::Op::NewMove,
                                     std::make_unique<Temp>(*temp),
                                     std::make_unique<Temp>(-static_cast<int>(MIPS::Register::v0), Type::Int),
                                     nullptr));
        return temp;
    } else {
        // reType == NodeType::VOIDTK
        return nullptr;
    }
}

const std::string &FuncCall::getIdent() const {
    return ident;
}
