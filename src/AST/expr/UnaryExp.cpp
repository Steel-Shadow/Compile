//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"
#include "AST/decl/Decl.h"
#include "AST/func/Func.h"
#include "backend/Register.h"
#include "backend/Memory.h"
#include "errorHandler/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

#include <iostream>

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
    auto [symbol, depth] = SymTab::findInGen(ident);
    auto var = std::make_unique<IR::Var>(
        ident,
        depth,
        symbol->cons,
        symbol->dims,
        symbol->type,
        symbol->symType);

    // TODO: 给LVal分配寄存器s0-s7
    auto varReg = MIPS::StackMemory::varToReg.find(*var);
    if (varReg != MIPS::StackMemory::varToReg.end()) {
        // var is mapped to a reg($a1-$a3 / $s0-$s7)
        MIPS::Reg reg = varReg->second;
        return std::make_unique<Temp>(reg, var->type);
    } else {
        auto res = std::make_unique<Temp>(symbol->type);
        if (symbol->type == Type::IntPtr) {
            auto addr = std::make_unique<Temp>(Type::Int);
            bBlocks.back()->addInst(Inst(IR::Op::Load,
                                         std::make_unique<Temp>(*addr),
                                         std::move(var),
                                         nullptr));
            int constOffset;
            std::unique_ptr<Temp> dynamicOffset;
            bool getNonConstIndex = getOffset(constOffset, dynamicOffset, bBlocks, symbol->dims);
            if (getNonConstIndex) {
                auto addrElem = std::make_unique<Temp>(Type::Int);
                bBlocks.back()->addInst(Inst(IR::Op::Add,
                                             std::make_unique<Temp>(*addrElem),
                                             std::move(addr),
                                             std::move(dynamicOffset)));
                bBlocks.back()->addInst(Inst(IR::Op::LoadPtr,
                                             std::make_unique<Temp>(*res),
                                             std::move(addrElem),
                                             nullptr));
            } else {
                bBlocks.back()->addInst(Inst(IR::Op::LoadPtr,
                                             std::make_unique<Temp>(*res),
                                             std::move(addr),
                                             std::make_unique<ConstVal>(constOffset, Type::Int)));
            }
            return res;
        } else {
            if (dims.empty()) {
                bBlocks.back()->addInst(Inst(IR::Op::Load,
                                             std::make_unique<Temp>(*res),
                                             std::move(var),
                                             nullptr
                ));
            } else {
                int constOffset;
                std::unique_ptr<Temp> dynamicOffset;
                bool getNonConstIndex = getOffset(constOffset, dynamicOffset, bBlocks, symbol->dims);
                if (getNonConstIndex) {
                    bBlocks.back()->addInst(Inst(IR::Op::LoadDynamic,
                                                 std::make_unique<Temp>(*res),
                                                 std::move(var),
                                                 std::move(dynamicOffset)));
                } else {
                    bBlocks.back()->addInst(Inst(IR::Op::Load,
                                                 std::make_unique<Temp>(*res),
                                                 std::move(var),
                                                 std::make_unique<ConstVal>(constOffset, Type::Int)));
                }
            }
            return res;
        }
    }
}

int LVal::evaluate() {
    auto sym = SymTab::find(ident);
    if (sym == nullptr) {
        Error::raise("LVal not found in evaluate()");
        return 0;
    } else if (!sym->cons) {
        Exp::getNonConstValueInEvaluate = true;
        // Non-const LVal in evaluate()
        return 0;
    } else if (sym->dims.empty()) {
        return sym->initVal[0];
    } else {
        Exp::getNonConstValueInEvaluate = true;
        // Const Array element in evaluate()
        return 0;
    }
}

bool LVal::getOffset(int &constOffset, std::unique_ptr<IR::Temp> &dynamicOffset, IR::BasicBlocks &bBlocks, const std::vector<int> &symDims) const {
    constOffset = 0;
    auto product = 1;
    bool getNonConstIndex = false;
    for (int i = static_cast<int>(symDims.size()) - 1; i >= 0; --i) {
        if (i != static_cast<int>(symDims.size()) - 1) {
            product *= symDims[i + 1];
        }

        int constIndex = 0;
        std::unique_ptr<IR::Temp> dynamicIndex;

        if (i < dims.size()) {
            constIndex = dims[i]->evaluate();
            if (Exp::getNonConstValueInEvaluate) {
                getNonConstIndex = true;
            }
            if (getNonConstIndex) {
                dynamicIndex = dims[i]->genIR(bBlocks);
            }
        }

        if (!getNonConstIndex) {
            constOffset += constIndex * product;
        } else {
            if (dynamicOffset == nullptr) {
                dynamicOffset = std::make_unique<IR::Temp>(Type::Int);
                bBlocks.back()->addInst(IR::Inst(
                    IR::Op::LoadImd,
                    std::make_unique<IR::Temp>(*dynamicOffset),
                    std::make_unique<IR::ConstVal>(constOffset, Type::Int),
                    nullptr
                ));
            }
            auto dynamicIndexTimesProduct = std::make_unique<IR::Temp>(Type::Int);

            if (product == 1) {
                dynamicIndexTimesProduct = std::move(dynamicIndex);
            } else {
                bBlocks.back()->addInst(IR::Inst(
                    IR::Op::MulImd,
                    std::make_unique<IR::Temp>(*dynamicIndexTimesProduct),
                    std::move(dynamicIndex),
                    std::make_unique<IR::ConstVal>(product, Type::Int)));
            }

            auto res = std::make_unique<IR::Temp>(Type::Int);
            bBlocks.back()->addInst(IR::Inst(
                IR::Op::Add,
                std::make_unique<IR::Temp>(*res),
                std::move(dynamicOffset),
                std::move(dynamicIndexTimesProduct)));
            dynamicOffset = std::move(res);

        }
    }

    if (getNonConstIndex) {
        bBlocks.back()->addInst(IR::Inst(IR::Op::Mult4,
                                         std::make_unique<IR::Temp>(*dynamicOffset),
                                         std::make_unique<IR::Temp>(*dynamicOffset),
                                         nullptr));
    }

    return getNonConstIndex;
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

    for (LexType op: ops) {
        if (op == LexType::MINU) {
            auto negRes = std::make_unique<Temp>(res->type);
            bBlocks.back()->addInst(Inst(
                Op::Neg,
                std::make_unique<Temp>(*negRes),
                std::move(res),
                nullptr
            ));
            res = std::move(negRes);
        } else if (op == LexType::NOT) {
            auto notRes = std::make_unique<Temp>(res->type);
            bBlocks.back()->addInst(Inst(
                Op::Not,
                std::make_unique<Temp>(*notRes),
                std::move(res),
                nullptr
            ));
            res = std::move(notRes);
        }
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

            size_t formalRank = funcSym->params[i].second->dims.size();
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
    bBlocks.back()->addInst(Inst(Op::InStack, nullptr, nullptr, nullptr));

    if (funcRParams) {
        for (int i = static_cast<int>(funcRParams->params.size()) - 1, paramIndex = 0; i >= 0; --i, ++paramIndex) {
            auto &rParam = funcRParams->params[i];
            auto name = rParam->getIdent();

            auto symbol = SymTab::find(name); // LVal / FuncCall
            size_t formalRank = funcSym->params[i].second->dims.size();

            if (formalRank == 0) {
                // single LVal (not array)
                // load Var from memory to Temp
                auto t = rParam->genIR(bBlocks);
                bBlocks.back()->addInst(Inst(Op::PushParam,
                                             std::make_unique<ConstVal>(paramIndex, Type::Void),
                                             std::move(t),
                                             nullptr));
            } else {
                // array (pass param by address)
                auto var = std::make_unique<Var>(
                    name,
                    SymTab::findDepth(name),
                    symbol->cons,
                    symbol->dims,
                    symbol->type,
                    symbol->symType);

                int constOffset;
                std::unique_ptr<Temp> dynamicOffset;
                bool getNonConstIndex = rParam->getLVal()->getOffset(
                    constOffset, dynamicOffset, bBlocks, symbol->dims);

                if (getNonConstIndex) {
                    bBlocks.back()->addInst(Inst(
                        Op::PushAddressParam,
                        std::make_unique<ConstVal>(paramIndex, Type::Void),
                        std::move(var),
                        std::move(dynamicOffset)));
                } else {
                    bBlocks.back()->addInst(Inst(
                        Op::PushAddressParam,
                        std::make_unique<ConstVal>(paramIndex, Type::Void),
                        std::move(var),
                        std::make_unique<ConstVal>(constOffset, Type::Int)));
                }
            }
        }
    }

    bBlocks.back()->addInst(Inst(IR::Op::Call,
                                 nullptr,
                                 std::make_unique<Label>(ident, true),
                                 nullptr));
    bBlocks.back()->addInst(Inst(Op::OutStack, nullptr, nullptr, nullptr));

    if (funcSym->type == Type::Int) {
        auto temp = std::make_unique<Temp>(Type::Int);
        bBlocks.back()->addInst(Inst(IR::Op::TempMove,
                                     std::make_unique<Temp>(*temp),
                                     std::make_unique<Temp>(MIPS::Reg::v0, Type::Int),
                                     nullptr));
        return temp;
    } else {
        // reType == LexType::VOIDTK
        return nullptr;
    }
}

const std::string &FuncCall::getIdent() const {
    return ident;
}
