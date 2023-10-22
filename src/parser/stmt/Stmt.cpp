//
// Created by Steel_Shadow on 2023/10/12.
//

#include "Stmt.h"

#include "parser/decl/Decl.h"
#include "error/Error.h"
#include "parser/Parser.h"
#include "symTab/SymTab.h"

using namespace Parser;

int Block::lastRow;

std::unique_ptr<Block> Block::parse() {
    auto n = std::make_unique<Block>();

    singleLex(NodeType::LBRACE);

    while (Lexer::curLexType != NodeType::RBRACE) {
        auto i = BlockItem::parse();
        n->blockItems.push_back(std::move(i));
    }

    Block::lastRow = Lexer::curRow;
    Lexer::next(); // }

    output(NodeType::Block);
    return n;
}

// In our work, there must be a ReturnStmt
// at the end of a block if the return type
// of the function is not void.
NodeType Block::getReType() {
    if (blockItems.empty()) {
        return NodeType::VOIDTK;
    } else if (auto last = dynamic_cast<ReturnStmt *>(
            blockItems.back().get())) {
        return last->getExp() == nullptr ?
               NodeType::VOIDTK : NodeType::INTTK;
    } else {
        return NodeType::VOIDTK;
    }
}

const std::vector<std::unique_ptr<BlockItem>> &Block::getBlockItems() const {
    return blockItems;
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

bool Stmt::retVoid;

std::unique_ptr<Stmt> Stmt::parse() {
    std::unique_ptr<Stmt> n;

    switch (Lexer::curLexType) {
        case NodeType::LBRACE:
            n = BlockStmt::parse();
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

    SymTab::deepIn();
    singleLex(NodeType::LPARENT);

    int row = Lexer::curRow;
    n->cond = Cond::parse();
    singleLex(NodeType::RPARENT, row);
    n->ifStmt = Stmt::parse();

    if (Lexer::curLexType == NodeType::ELSETK) {
        Lexer::next();
        n->ifStmt = Stmt::parse();
    }

    SymTab::deepOut();
    return n;
}

bool BigForStmt::inFor;

std::unique_ptr<BigForStmt> BigForStmt::parse() {
    auto n = std::make_unique<BigForStmt>();

    inFor = true;

    Lexer::next();
    singleLex(NodeType::LPARENT);

    SymTab::deepIn();

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

    inFor = false;
    SymTab::deepOut();
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
    int row = Lexer::curRow;

    if (!BigForStmt::inFor) {
        Error::raise('m', row);
    }

    Lexer::next();
    singleLex(NodeType::SEMICN, row);
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<ContinueStmt> ContinueStmt::parse() {
    int row = Lexer::curRow;

    if (!BigForStmt::inFor) {
        Error::raise('m', row);
    }

    Lexer::next();
    singleLex(NodeType::SEMICN, row);
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<ReturnStmt> ReturnStmt::parse() {
    auto n = std::make_unique<ReturnStmt>();

    Lexer::next();

    if (Lexer::curLexType == NodeType::SEMICN) {
        Lexer::next();
    } else {
        if (Stmt::retVoid) {
            Error::raise('f');
        }
        int row = Lexer::curRow; // error handle
        n->exp = Exp::parse(false);
        singleLex(NodeType::SEMICN, row);
    }

    return n;
}

const std::unique_ptr<Exp> &ReturnStmt::getExp() const {
    return exp;
}

void PrintStmt::checkFormatString(std::string str) {
    // skip begin and end " "
    for (int i = 1; i < str.length() - 1; i++) {
        char c = str[i];

        if (c == '\\') {
            if (str[++i] != 'n') {
                Error::raise('a');
            }
        } else if (c == '%') {
            numOfFormat++;
            if (str[++i] != 'd') {
                Error::raise('a');
            }
        } else if (!(c == 32 || c == 33 || c >= 40 && c <= 126)) {
            Error::raise('a');
        }
    }
}

std::unique_ptr<PrintStmt> PrintStmt::parse() {
    auto n = std::make_unique<PrintStmt>();

    int row = Lexer::curRow; //todo: maybe wrong
    Lexer::next();

    singleLex(NodeType::LPARENT);

    if (Lexer::curLexType == NodeType::STRCON) {
        n->checkFormatString(Lexer::curToken);
        n->formatString = Lexer::curToken;
        Lexer::next();
    } else { Error::raise(); }

    int numOfExp = 0;
    while (Lexer::curLexType == NodeType::COMMA) {
        Lexer::next();
        numOfExp++;
        n->exps.push_back(Exp::parse(false));
    }

    if (numOfExp != n->numOfFormat) {
        Error::raise('l', row);
    }

    singleLex(NodeType::RPARENT, row);
    singleLex(NodeType::SEMICN, row);
    return n;
}

void PrintStmt::checkFormatNum() {

}

std::unique_ptr<LValStmt> LValStmt::parse() {
    std::unique_ptr<LValStmt> n;

    int row = Lexer::curRow;
    auto lVal = LVal::parse();

    auto sym = SymTab::find(lVal->getIdent());
    if (sym && sym->cons) {
        Error::raise('h', row);
    }

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

    int row = Lexer::curRow; // todo: maybe wrong

    singleLex(NodeType::GETINTTK);
    singleLex(NodeType::LPARENT);
    singleLex(NodeType::RPARENT, row);
    singleLex(NodeType::SEMICN, row); // ugly error handling!

    return n;
}

std::unique_ptr<AssignStmt> AssignStmt::parse() {
    auto n = std::make_unique<AssignStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN, row);

    return n;
}

std::unique_ptr<ExpStmt> ExpStmt::parse() {
    auto n = std::make_unique<ExpStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN, row);

    return n;
}

std::unique_ptr<BlockStmt> BlockStmt::parse() {
    auto n = std::make_unique<BlockStmt>();

    SymTab::deepIn();
    n->block = Block::parse();
    SymTab::deepOut();

    return n;
}
