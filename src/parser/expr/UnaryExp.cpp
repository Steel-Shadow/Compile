//
// Created by Steel_Shadow on 2023/10/12.
//
#include "Exp.h"
#include "parser/decl/Decl.h"
#include "parser/Parser.h"
#include "error/Error.h"
#include "parser/func/Func.h"

using namespace Parser;

std::unique_ptr<LVal> LVal::parse() {
    auto n = std::make_unique<LVal>();
    n->ident = Ident::parse();

    while (curLexType == NodeType::LBRACK) {
        lexer.next();
        n->dims.push_back(Exp::parse(false)); //todo: Check all const settings!
        singleLex(NodeType::RBRACK);
    }

    output(NodeType::LVal);
    return n;
}

std::unique_ptr<PrimaryExp> PrimaryExp::parse(bool cons) {
    auto n = std::make_unique<PrimaryExp>();
    n->cons = cons;

    if (curLexType == NodeType::LPARENT) {
        n = PareExp::parse(cons);
    } else if (curLexType == NodeType::IDENFR) {
        n = LVal::parse();
    } else if (curLexType == NodeType::INTCON) {
        n = Number::parse();
    }

    output(NodeType::PrimaryExp);
    return n;
}

std::unique_ptr<PareExp> PareExp::parse(bool cons) {
    auto n = std::make_unique<PareExp>();
    n->cons = cons;

    lexer.next();
    n->exp = Exp::parse(cons);
    singleLex(NodeType::RPARENT);

    return n;
}

std::unique_ptr<Number> Number::parse() {
    auto n = std::make_unique<Number>();

    if (curLexType == NodeType::INTCON) {
        n->intConst = std::stoi(lexer.peek().second);
        lexer.next();
    } else { Error::raise_error(); }

    output(NodeType::Number);
    return n;
}

std::unique_ptr<UnaryExp> UnaryExp::parse(bool cons) {
    auto n = std::make_unique<UnaryExp>();

    bool getBaseUnaryExp = false;
    while (!getBaseUnaryExp) {
        // UnaryExp → {UnaryOp} ( PrimaryExp | Ident '(' [FuncRParams] ')' )
        switch (curLexType) {
            case NodeType::PLUS:
            case NodeType::MINU:
            case NodeType::NOT:
                // UnaryOp → '+' | '−' | '!'
                n->unaryOps.push_back(curLexType);
                lexer.next();

                output(NodeType::UnaryOp);
                break;

            case NodeType::LPARENT:
            case NodeType::INTCON:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                n->baseUnaryExp = PrimaryExp::parse(cons);
                getBaseUnaryExp = true;
                break;

            case NodeType::IDENFR:
                // PrimaryExp → '(' Exp ')' | LVal | Number
                // LVal → Ident {'[' Exp ']'}

                // Ident '(' [FuncRParams] ')'
                if (lexer.peek(1).first == NodeType::LPARENT) {
                    n->baseUnaryExp = FuncCall::parse();
                } else {
                    n->baseUnaryExp = PrimaryExp::parse(cons);
                }
                getBaseUnaryExp = true;
                break;

            default:
                Error::raise_error();
        }

        output(NodeType::UnaryExp);
    }

    return n;
}

std::unique_ptr<FuncCall> FuncCall::parse() {
    auto n = std::make_unique<FuncCall>();

    n->ident = Ident::parse();
    singleLex(NodeType::LPARENT);

    if (curLexType != NodeType::RPARENT) {
        n->funcRParams = FuncRParams::parse();
    }

    singleLex(NodeType::RPARENT);
    return n;
}
