//
// Created by Steel_Shadow on 2023/10/12.
//

#include "Stmt.h"
#include "lexer/NodeType.h"
#include "parser/Parser.h"
#include "parser/decl/Decl.h"
#include "error/Error.h"

using namespace Parser;

std::unique_ptr<Block> Block::parse() {
    auto n = std::make_unique<Block>();

    singleLex(NodeType::LBRACE);

    while (curLexType != NodeType::RBRACE) {
        n->blockItems.push_back(BlockItem::parse());
    }

    lexer.next();

    output(NodeType::Block);
    return n;
}

std::unique_ptr<BlockItem> BlockItem::parse() {
    std::unique_ptr<BlockItem> n;

    // Maybe error when neither Decl nor Stmt. But it's too complicated.
    if (curLexType == NodeType::CONSTTK || curLexType == NodeType::INTTK) {
        n = Decl::parse();
    } else {
        n = Stmt::parse();
    }

    // output(NodeType::BlockItem);
    return n;
}

std::unique_ptr<Stmt> Stmt::parse() {
    std::unique_ptr<Stmt> n;

    switch (curLexType) {
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
            lexer.next();
            break;
        case NodeType::IDENFR:
            //find '=' to distinguish LVal and Exp
            if (lexer.findAssignBeforeSemicolon()) {
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

    lexer.next();
    singleLex(NodeType::LPARENT);

    n->cond = Cond::parse(false);
    singleLex(NodeType::RPARENT);
    n->ifStmt = Stmt::parse();

    if (curLexType == NodeType::ELSETK) {
        lexer.next();
        n->ifStmt = Stmt::parse();
    }

    return n;
}

std::unique_ptr<BigForStmt> BigForStmt::parse() {
    auto n = std::make_unique<BigForStmt>();

    lexer.next();
    singleLex(NodeType::LPARENT);

    if (curLexType != NodeType::SEMICN) {
        n->init = ForStmt::parse();
    }
    singleLex(NodeType::SEMICN);

    if (curLexType != NodeType::SEMICN) {
        n->cond = Cond::parse(false);
    }
    singleLex(NodeType::SEMICN);

    if (curLexType != NodeType::RPARENT) {
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
    lexer.next();
    singleLex(NodeType::SEMICN);
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<ContinueStmt> ContinueStmt::parse() {
    lexer.next();
    singleLex(NodeType::SEMICN);
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<ReturnStmt> ReturnStmt::parse() {
    auto n = std::make_unique<ReturnStmt>();

    lexer.next();
    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN);

    return n;
}

std::unique_ptr<PrintStmt> PrintStmt::parse() {
    auto n = std::make_unique<PrintStmt>();

    lexer.next();

    singleLex(NodeType::LPARENT);

    if (curLexType == NodeType::STRCON) {
        n->formatString = lexer.peek().second;
        lexer.next();
    } else { Error::raise_error(); }

    while (curLexType == NodeType::COMMA) {
        lexer.next();
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

    if (curLexType == NodeType::GETINTTK) {
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
