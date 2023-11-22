//
// Created by Steel_Shadow on 2023/10/12.
//

#include <regex>
#include "Stmt.h"

#include "AST/decl/Decl.h"
#include "frontend/error/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

int Block::lastRow;

std::unique_ptr<Block> Block::parse() {
    auto n = std::make_unique<Block>();

    singleLex(NodeType::LBRACE);

    while (Lexer::curLexType != NodeType::RBRACE) {
        auto i = BlockItem::parse();
        n->blockItems.push_back(std::move(i));
    }

    lastRow = Lexer::curRow;
    Lexer::next(); // }

    output(NodeType::Block);
    return n;
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

void Block::genIR(IR::BasicBlocks &basicBlocks) {
    using namespace IR;
    for (auto &i: blockItems) {
        i->genIR(basicBlocks);
    }
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
            n = std::make_unique<ExpStmt>();
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

    SymTab::deepOut(); // IfStmt
    return n;
}

void IfStmt::genIR(IR::BasicBlocks &bBlocks) {
    SymTab::iterIn();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::InStack, nullptr, nullptr, nullptr));

    // todo: 代码生成2

    bBlocks.back()->addInst(IR::Inst(
            IR::Op::OutStack, nullptr, nullptr, nullptr));
    SymTab::iterOut();
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
    SymTab::deepOut(); // ForStmt
    return n;
}

std::stack<IR::Label> BigForStmt::stackEndLabel{};
std::stack<IR::Label> BigForStmt::stackIterLabel{};

void BigForStmt::genIR(IR::BasicBlocks &bBlocks) {
    SymTab::iterIn();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::InStack, nullptr, nullptr, nullptr));

    using namespace IR;
    init->genIR(bBlocks);

    auto forCondBlock = std::make_unique<BasicBlock>("ForCond");
    auto forIterBlock = std::make_unique<BasicBlock>("ForIter");
    auto forEndBlock = std::make_unique<BasicBlock>("ForEnd");

    stackEndLabel.push(forEndBlock->label);
    stackIterLabel.push(forIterBlock->label);

    auto pForCondBlock = forCondBlock.get();
    auto pForIterBlock = forIterBlock.get();

    bBlocks.push_back(std::move(forCondBlock));
    auto tempCond = cond->genIR(bBlocks);
    pForCondBlock->addInst(Inst(IR::Op::Bif0,
                                nullptr,
                                std::move(tempCond),
                                std::make_unique<Label>(forEndBlock->label)));

    stmt->genIR(bBlocks);

    bBlocks.push_back(std::move(forIterBlock));
    iter->genIR(bBlocks);
    pForIterBlock->addInst(Inst(IR::Op::Br,
                                nullptr,
                                std::make_unique<Label>(pForCondBlock->label),
                                nullptr));

    bBlocks.push_back(std::move(forEndBlock));

    stackEndLabel.pop();
    stackIterLabel.pop();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::OutStack, nullptr, nullptr, nullptr));
    SymTab::iterOut();
}

std::unique_ptr<ForStmt> ForStmt::parse() {
    auto n = std::make_unique<ForStmt>();

    n->lVal = LVal::parse();
    singleLex(NodeType::ASSIGN);
    n->exp = Exp::parse(false);

    output(NodeType::ForStmt);
    return n;
}

void ForStmt::genIR(IR::BasicBlocks &basicBlocks) const {
    using namespace IR;
    auto t = exp->genIR(basicBlocks);

    auto sym = SymTab::find(lVal->ident);
    auto irLVal = std::make_unique<Var>(lVal->ident,
                                        SymTab::findDepth(lVal->ident),
                                        sym->cons,
                                        sym->dims);
    basicBlocks.back()->addInst(Inst(IR::Op::Assign,
                                     std::move(irLVal),
                                     std::move(t),
                                     nullptr));
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

void BreakStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    bBlocks.back()->addInst(Inst(IR::Op::Br,
                                 nullptr,
                                 std::make_unique<Label>(BigForStmt::stackEndLabel.top()),
                                 nullptr));
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

void ContinueStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    bBlocks.back()->addInst(Inst(IR::Op::Br,
                                 nullptr,
                                 std::make_unique<Label>(BigForStmt::stackIterLabel.top()),
                                 nullptr));
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

void ReturnStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    if (exp) {
        auto temp = exp->genIR(bBlocks);
        bBlocks.back()->addInst(Inst(IR::Op::Ret,
                                     nullptr,
                                     std::move(temp),
                                     nullptr));
    } else {
        bBlocks.back()->addInst(Inst(IR::Op::Ret,
                                     nullptr,
                                     nullptr,
                                     nullptr));
    }
}

void PrintStmt::checkFormatString(const std::string &str) {
    // skip begin and end " "
    for (int i = 1; i < str.length() - 1; i++) {
        char c = str[i];

        if (c == '\\') {
            if (str[++i] != 'n') {
                Error::raise('a');
            }
        } else if (c == '%') {
            if (str[++i] != 'd') {
                Error::raise('a');
            } else {
                numOfFormat++;
            }
        } else if (!(c == 32 || c == 33 || c >= 40 && c <= 126)) {
            Error::raise('a');
        }
    }
}

std::unique_ptr<PrintStmt> PrintStmt::parse() {
    auto n = std::make_unique<PrintStmt>();

    int row = Lexer::curRow;
    Lexer::next();

    singleLex(NodeType::LPARENT);

    if (Lexer::curLexType == NodeType::STRCON) {
        n->checkFormatString(Lexer::curToken);
        n->formatString = Lexer::curToken;
        Lexer::next();
    } else {
        Error::raise();
    }

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

void PrintStmt::addStr(IR::BasicBlocks &bBlocks, std::string &buffer) {
    if (buffer.empty()) {
        return;
    }

    IR::Str::MIPS_strings.push_back('\"' + buffer + '\"');
    buffer.clear();
    bBlocks.back()->addInst(IR::Inst(IR::Op::PrintStr,
                                     nullptr,
                                     std::make_unique<IR::Str>(),
                                     nullptr));
}

void PrintStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    bBlocks.back()->addInst(Inst(IR::Op::InStack, nullptr, nullptr, nullptr));

    std::vector<std::unique_ptr<Temp>> args(exps.size());
    for (int i = 0; i < exps.size(); ++i) {
        args[i] = exps[i]->genIR(bBlocks);
//        if (i < 4) {
//            // $a0 - $a3
//            auto t = dynamic_cast<Temp *>((bBlocks.back()->instructions.back().res.get()));
//            t->id = -(i + static_cast<int>(MIPS::Register::a0));
//            args[i]->id = -(i + static_cast<int>(MIPS::Register::a0));
//        } else {
//            bBlocks.back()->addInst(Inst(Op::PushParam,
//                                         nullptr,
//                                         std::make_unique<Temp>(*args[i]),
//                                         nullptr));
//        }
    }

    // string | %d
    std::string buffer;
    // skip \" in formatString
    for (int i = 1, j = 0; i < formatString.length() - 1; i++) {
        if (formatString[i] == '%') {
            i++;
            addStr(bBlocks, buffer);

            bBlocks.back()->addInst(Inst(IR::Op::PrintInt,
                                         nullptr,
                                         std::move(args[j++]),
                                         nullptr));
        } else {
            buffer += formatString[i];
        }
    }
    addStr(bBlocks, buffer);
    bBlocks.back()->addInst(Inst(IR::Op::OutStack, nullptr, nullptr, nullptr));
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
        n = GetIntStmt::parse();
        n->lVal = std::move(lVal);
    } else {
        n = AssignStmt::parse();
        n->lVal = std::move(lVal);
    }

    return n;
}

std::unique_ptr<GetIntStmt> GetIntStmt::parse() {
    auto n = std::make_unique<GetIntStmt>();

    int row = Lexer::curRow;

    singleLex(NodeType::GETINTTK);
    singleLex(NodeType::LPARENT);
    singleLex(NodeType::RPARENT, row);
    singleLex(NodeType::SEMICN, row); // ugly error handling!

    return n;
}

void GetIntStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto sym = SymTab::find(lVal->ident);
    auto irLVal = std::make_unique<Var>(lVal->ident,
                                        SymTab::findDepth(lVal->ident),
                                        sym->cons,
                                        sym->dims);

    bBlocks.back()->addInst(Inst(IR::Op::GetInt,
                                 std::move(irLVal),
                                 nullptr,
                                 nullptr));
}

std::unique_ptr<AssignStmt> AssignStmt::parse() {
    auto n = std::make_unique<AssignStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN, row);

    return n;
}

void AssignStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto t = exp->genIR(bBlocks);

    auto symbol = SymTab::find(lVal->ident);
    auto var = std::make_unique<IR::Var>(
            lVal->ident,
            SymTab::findDepth(lVal->ident),
            symbol->cons,
            symbol->dims);

    bBlocks.back()->addInst(Inst(IR::Op::Assign,
                                 std::move(var),
                                 std::move(t),
                                 nullptr));
}

std::unique_ptr<ExpStmt> ExpStmt::parse() {
    auto n = std::make_unique<ExpStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(NodeType::SEMICN, row);

    return n;
}

void ExpStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    if (exp) {
        exp->genIR(bBlocks);
    }
}

std::unique_ptr<BlockStmt> BlockStmt::parse() {
    auto n = std::make_unique<BlockStmt>();

    SymTab::deepIn();
    n->block = Block::parse();

    SymTab::deepOut(); // BlockStmt

    return n;
}

void BlockStmt::genIR(IR::BasicBlocks &bBlocks) {
    SymTab::iterIn();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::InStack, nullptr, nullptr, nullptr));

    block->genIR(bBlocks);

    bBlocks.back()->addInst(IR::Inst(
            IR::Op::OutStack, nullptr, nullptr, nullptr));
    SymTab::iterOut();
}
