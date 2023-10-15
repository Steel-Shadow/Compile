//
// Created by Steel_Shadow on 2023/10/12.
//

#include "Stmt.h"

#include "lexer/NodeType.h"
#include "parser/decl/Decl.h"
#include "error/Error.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

using namespace Parser;

std::unique_ptr<Block> Block::parse() {
    auto n = std::make_unique<Block>();

    singleLex(NodeType::LBRACE);

    while (Lexer::curLexType != NodeType::RBRACE) {
        n->blockItems.push_back(BlockItem::parse());
    }

    Lexer::next();

    output(NodeType::Block);
    return n;
}

std::unique_ptr<BlockItem> BlockItem::parse() {
    std::unique_ptr<BlockItem> n;

    // Maybe error when neither Decl nor Stmt. But it's too complicated.
    if (Lexer::curLexType == NodeType::CONSTTK || Lexer::curLexType == NodeType::INTTK) {
        n = Decl::parse();
    } else {
        n = Stmt::parse();
    }

    // output(NodeType::BlockItem);
    return n;
}

std::unique_ptr<Stmt> Stmt::parse() {
    std::unique_ptr<Stmt> n;

    switch (Lexer::curLexType) {
        case NodeType::LBRACE:
            n = Block::parse();
            break;
        case NodeType::IFTK:
            n = IfStmt::parse();
            break;
        case NodeType::BREAKTK:
            n = BreakStmt::parse();
            break;
        case NodeType::CONTINUETK:
            n = ContinueStmt::parse();
            break;
        case NodeType::FORTK:
            n = BigForStmt::parse();
            break;
        case NodeType::RETURNTK:
            n = ReturnStmt::parse();
            break;
        case NodeType::PRINTFTK:
            n = PrintStmt::parse();
            break;
        case NodeType::SEMICN:
            Lexer::next();
            break;
        case NodeType::IDENFR:
            //find '=' to distinguish LVal and Exp
            if (Lexer::findAssignBeforeSemicolon()) {
                n = LValStmt::parse();
            } else {
                n = ExpStmt::parse();
            }
            break;
        default:
            n = ExpStmt::parse();
            break;
    }

    output(NodeType::Stmt);
    return n;
}

std::unique_ptr<IfStmt> IfStmt::parse() {
    auto n = std::make_unique<IfStmt>();

    Lexer::next();
    singleLex(NodeType::LPARENT);

    n->cond = Cond::parse();
    singleLex(NodeType::RPARENT);
    n->ifStmt = Stmt::parse();

    if (Lexer::curLexType == NodeType::ELSETK) {
        Lexer::next();
        n->ifStmt = Stmt::parse();
    }

    return n;
}

std::unique_ptr<BigForStmt> BigForStmt::parse() {
    auto n = std::make_unique<BigForStmt>();

    Lexer::next();
    singleLex(NodeType::LPARENT);

    if (Lexer::curLexType != NodeType::SEMICN) {
        n->init = ForStmt::parse();
    }
    singleLex(NodeType::SEMICN);

    if (Lexer::curLexType != NodeType::SEMICN) {
        n->cond = Cond::parse();
    }
    singleLex(NodeType::SEMICN);

    if (Lexer::curLexType != NodeType::RPARENT) {
        n->iter = ForStmt::parse();
    }
    singleLex(NodeType::RPARENT);

    n->stmt = Stmt::parse();
    return n;
}

std::unique_ptr<ForStmt> ForStmt::parse() {
    auto n = std::make_unique<ForStmt>();

    n->lVal = LVal::parse();
    singleLex(NodeType::ASSIGN);
    n->exp = Exp::parse(false);

    output(NodeType::ForStmt);
    return n;
}

std::unique_ptr<BreakStmt> BreakStmt::parse() {
    Lexer::next();
    singleLex(NodeType::SEMICN);
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<ContinueStmt> ContinueStmt::parse() {
    Lexer::next();
    singleLex(NodeType::SEMICN);
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<ReturnStmt> ReturnStmt::parse() {
    auto n = std::make_unique<ReturnStmt>();

    Lexer::next();

    if (Lexer::curLexType == NodeType::SEMICN) {
        Lexer::next();
    } else {
        n->exp = Exp::parse(false);
        singleLex(NodeType::SEMICN);
    }

    return n;
}

std::unique_ptr<PrintStmt> PrintStmt::parse() {
    auto n = std::make_unique<PrintStmt>();

    Lexer::next();

    singleLex(NodeType::LPARENT);

    if (Lexer::curLexType == NodeType::STRCON) {
        n->formatString = Lexer::curToken;
        Lexer::next();
    } else { Error::raise_error(); }

    while (Lexer::curLexType == NodeType::COMMA) {
        Lexer::next();
        n->exps.push_back(Exp::parse(false));
    }

    singleLex(NodeType::RPARENT);
    singleLex(NodeType::SEMICN);

    return n;
}

std::unique_ptr<LValStmt> LValStmt::parse() {
    std::unique_ptr<LValStmt> n;

    auto lVal = LVal::parse();
    singleLex(NodeType::ASSIGN);

    if (Lexer::curLexType == NodeType::GETINTTK) {
        n = GetintStmt::parse();
        n->lVal = std::move(lVal);
    } else {
        n = AssignStmt::parse();
        n->lVal = std::move(lVal);
    }

    return n;
}

std::unique_ptr<GetintStmt> GetintStmt::parse() {
    auto n = std::make_unique<GetintStmt>();

    singleLex(NodeType::GETINTTK);
    singleLex(NodeType::LPARENT);
    singleLex(NodeType::RPARENT);
    singleLex(NodeType::SEMICN);

    return n;
}

std::unique_ptr<AssignStmt> AssignStmt::parse() {
    auto n = std::make_unique<AssignStmt>();

    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN);

    return n;
}

std::unique_ptr<ExpStmt> ExpStmt::parse() {
    auto n = std::make_unique<ExpStmt>();

    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN);

    return n;
}
