//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"

#include "parser/decl/Decl.h"
#include "error/Error.h"
#include "parser/func/Func.h"
#include "parser/Parser.h"
#include "symTab/SymTab.h"

using namespace Parser;

std::unique_ptr<LVal> LVal::parse() {
    auto n = std::make_unique<LVal>();

    n->ident = Ident::parse();
    if (!SymTab::find(n->ident)) {
        Error::raise('c');
    }

    while (Lexer::curLexType == NodeType::LBRACK) {
        Lexer::next();
        int row = Lexer::curRow;
        n->dims.push_back(Exp::parse(false));
        singleLex(NodeType::RBRACK, row);
    }

    output(NodeType::LVal);
    return n;
}

size_t LVal::getRank() {
    return dims.size();
}

std::string LVal::getIdent() {
    return ident;
}

std::unique_ptr<PrimaryExp> PrimaryExp::parse() {
    auto n = std::make_unique<PrimaryExp>();

    if (Lexer::curLexType == NodeType::LPARENT) {
        n = PareExp::parse();
    } else if (Lexer::curLexType == NodeType::IDENFR) {
        n = LVal::parse();
    } else if (Lexer::curLexType == NodeType::INTCON) {
        n = Number::parse();
    }

    output(NodeType::PrimaryExp);
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
    singleLex(NodeType::RPARENT, row);

    return n;
}

std::unique_ptr<Number> Number::parse() {
    auto n = std::make_unique<Number>();

    if (Lexer::curLexType == NodeType::INTCON) {
        n->intConst = std::stoi(Lexer::curToken);
        Lexer::next();
    } else { Error::raise(); }

    output(NodeType::Number);
    return n;
}

int Number::evaluate() {
    return intConst;
}

std::unique_ptr<UnaryExp> UnaryExp::parse() {
    auto n = std::make_unique<UnaryExp>();

    bool getBaseUnaryExp = false;
    while (!getBaseUnaryExp) {
        // UnaryExp → {UnaryOp} ( PrimaryExp | Ident '(' [FuncRParams] ')' )
        switch (Lexer::curLexType) {
            case NodeType::PLUS:
            case NodeType::MINU:
            case NodeType::NOT:
                // UnaryOp → '+' | '−' | '!'
                n->ops.push_back(Lexer::curLexType);
                Lexer::next();

                output(NodeType::UnaryOp);
                break;

            case NodeType::LPARENT:
            case NodeType::INTCON:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                n->baseUnaryExp = PrimaryExp::parse();
                getBaseUnaryExp = true;

                output(NodeType::UnaryExp);
                break;

            case NodeType::IDENFR:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                // LVal → Ident {'[' Exp ']'}

                // Ident '(' [FuncRParams] ')'
                if (Lexer::peek(1).first == NodeType::LPARENT) {
                    n->baseUnaryExp = FuncCall::parse();
                } else {
                    n->baseUnaryExp = PrimaryExp::parse();
                }
                getBaseUnaryExp = true;

                output(NodeType::UnaryExp);
                break;

            default:
                Error::raise();
        }
    }

    for (int i = 0; i < n->ops.size(); i++) {
        output(NodeType::UnaryExp);
    }

    return n;
}

int UnaryExp::evaluate() {
    int val = baseUnaryExp->evaluate();

    for (auto op: ops) {
        if (op == NodeType::MINU) {
            val = -val;
        }
    }

    return val;
}

size_t UnaryExp::getRank() {
    if (auto p = dynamic_cast<PrimaryExp *>(baseUnaryExp.get())) {
        return p->getRank();
    }
    return 0;
}

std::string UnaryExp::getIdent() {
    if (auto p = dynamic_cast<PrimaryExp *>(baseUnaryExp.get())) {
        return p->getIdent();
    }
    if (auto p = dynamic_cast<FuncCall *>(baseUnaryExp.get())) {
        return p->getIdent();
    }
    return "";
}

std::unique_ptr<FuncCall> FuncCall::parse() {
    auto n = std::make_unique<FuncCall>();

    int row = Lexer::curRow;
    n->ident = Ident::parse();

    Symbol *funcSym = SymTab::find(n->ident);
    if (!funcSym) {
        Error::raise('c', row);
    }

    singleLex(NodeType::LPARENT);

    if (Lexer::curLexType != NodeType::RPARENT) {
        n->funcRParams = FuncRParams::parse();
        checkParams(n, row, funcSym); // SymTab error handle
    }

    singleLex(NodeType::RPARENT, row);
    return n;
}

std::string FuncCall::getIdent() {
    return ident;
}

void FuncCall::checkParams(const std::unique_ptr<FuncCall> &n, int row, const Symbol *funcSym) {
    auto &realParams = n->funcRParams->getParams();
    // check number of realParams
    if (realParams.size() != funcSym->params.size()) {
        Error::raise('d', row);
    } else {
        // check type of params
        for (int i = 0; i < realParams.size(); i++) {
            auto &rParam = realParams[i];

            size_t formalRank = funcSym->params[i].size();
            size_t symRank;
            size_t indexRank = rParam->getRank();

            std::string ident = rParam->getIdent();
            if (ident.empty()) {
                // rParam is Exp (neither LVal nor FuncCall)
                symRank = 0;
            } else {
                auto sym = SymTab::find(ident);
                if (sym->type == SymType::Func) {
                    // FuncCall
                    // void | int
                    symRank = sym->reType == NodeType::VOIDTK ? -1 : 0;
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

