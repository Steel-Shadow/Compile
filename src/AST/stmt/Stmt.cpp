//
// Created by Steel_Shadow on 2023/10/12.
//

#include "Stmt.h"

#include "AST/decl/Decl.h"
#include "backend/Instruction.h"
#include "errorHandler/Error.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

int Block::lastRow;

std::unique_ptr<Block> Block::parse() {
    auto n = std::make_unique<Block>();

    singleLex(LexType::LBRACE);

    while (Lexer::curLexType != LexType::RBRACE) {
        auto i = BlockItem::parse();
        n->blockItems.push_back(std::move(i));
    }

    lastRow = Lexer::curRow;
    Lexer::next(); // }

    output(AST::Block);
    return n;
}

const std::vector<std::unique_ptr<BlockItem>> &Block::getBlockItems() const {
    return blockItems;
}

std::unique_ptr<BlockItem> BlockItem::parse() {
    std::unique_ptr<BlockItem> n;

    // Maybe error when neither Decl nor Stmt. But it's too complicated.
    if (Lexer::curLexType == LexType::CONSTTK || Lexer::curLexType == LexType::INTTK) {
        n = Decl::parse();
    } else {
        n = Stmt::parse();
    }

    // output(AST::BlockItem);
    return n;
}

void Block::genIR(IR::BasicBlocks &basicBlocks) const {
    using namespace IR;
    for (auto &i: blockItems) {
        i->genIR(basicBlocks);
    }
}

bool Stmt::retVoid;

std::unique_ptr<Stmt> Stmt::parse() {
    std::unique_ptr<Stmt> n;

    switch (Lexer::curLexType) {
        case LexType::LBRACE:
            n = BlockStmt::parse();
            break;
        case LexType::IFTK:
            n = IfStmt::parse();
            break;
        case LexType::BREAKTK:
            n = BreakStmt::parse();
            break;
        case LexType::CONTINUETK:
            n = ContinueStmt::parse();
            break;
        case LexType::FORTK:
            n = BigForStmt::parse();
            break;
        case LexType::RETURNTK:
            n = ReturnStmt::parse();
            break;
        case LexType::PRINTFTK:
            n = PrintStmt::parse();
            break;
        case LexType::SEMICN:
            n = std::make_unique<ExpStmt>();
            Lexer::next();
            break;
        case LexType::IDENFR:
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

    output(AST::Stmt);
    return n;
}

std::unique_ptr<IfStmt> IfStmt::parse() {
    auto n = std::make_unique<IfStmt>();

    Lexer::next();

    SymTab::deepIn();
    singleLex(LexType::LPARENT);

    int row = Lexer::curRow;
    n->cond = Cond::parse();
    singleLex(LexType::RPARENT, row);
    n->ifStmt = Stmt::parse();

    if (Lexer::curLexType == LexType::ELSETK) {
        Lexer::next();
        n->elseStmt = Stmt::parse();
    }

    SymTab::deepOut(); // IfStmt
    return n;
}

void IfStmt::genIR(IR::BasicBlocks &bBlocks) {
    SymTab::iterIn();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::InStack, nullptr, nullptr, nullptr));

    auto trueBranch = std::make_unique<IR::BasicBlock>("IfTrueBranch");
    auto falseBranch = std::make_unique<IR::BasicBlock>("IfFalseBranch");
    auto ifEnd = std::make_unique<IR::BasicBlock>("IfEnd");

    cond->genIR(bBlocks, trueBranch->label, falseBranch->label);

    bBlocks.emplace_back(std::move(trueBranch));
    ifStmt->genIR(bBlocks);
    bBlocks.back()->addInst(IR::Inst(IR::Op::Br, nullptr, std::make_unique<IR::Label>(ifEnd->label), nullptr));

    bBlocks.emplace_back(std::move(falseBranch));
    if (elseStmt) {
        elseStmt->genIR(bBlocks);
    }

    bBlocks.emplace_back(std::move(ifEnd));

    bBlocks.back()->addInst(IR::Inst(
            IR::Op::OutStack, nullptr, nullptr, nullptr));
    SymTab::iterOut();
}

int BigForStmt::inForDepth = 0;

std::unique_ptr<BigForStmt> BigForStmt::parse() {
    auto n = std::make_unique<BigForStmt>();

    inForDepth++;

    Lexer::next();
    singleLex(LexType::LPARENT);

    SymTab::deepIn();

    if (Lexer::curLexType != LexType::SEMICN) {
        n->init = ForStmt::parse();
    }
    singleLex(LexType::SEMICN);

    if (Lexer::curLexType != LexType::SEMICN) {
        n->cond = Cond::parse();
    }
    singleLex(LexType::SEMICN);

    if (Lexer::curLexType != LexType::RPARENT) {
        n->iter = ForStmt::parse();
    }
    singleLex(LexType::RPARENT);

    n->stmt = Stmt::parse();

    inForDepth--;
    SymTab::deepOut(); // ForStmt
    return n;
}

std::stack<IR::Label> BigForStmt::stackEndLabel{};
std::stack<IR::Label> BigForStmt::stackIterLabel{};

void BigForStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;

    SymTab::iterIn();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::InStack, nullptr, nullptr, nullptr));
    if (init) {
        init->genIR(bBlocks);
    }

    auto forBodyBlock = std::make_unique<BasicBlock>("ForBody");
    auto forIterCondBlock = std::make_unique<BasicBlock>("ForIter");
    auto forEndBlock = std::make_unique<BasicBlock>("ForEnd");

    stackEndLabel.push(forEndBlock->label);
    stackIterLabel.push(forIterCondBlock->label);

    // use unique_ptr after move
    auto pForBodyBlock = forBodyBlock.get();
    // auto pForIterCondBlock = forIterCondBlock.get();

    if (cond) {
        cond->genIR(bBlocks, forBodyBlock->label, forEndBlock->label);
    }

    bBlocks.push_back(std::move(forBodyBlock));
    if (stmt) {
        stmt->genIR(bBlocks);
    }

    bBlocks.push_back(std::move(forIterCondBlock));
    if (iter) {
        iter->genIR(bBlocks);
    }
    if (cond) {
        cond->genIR(bBlocks, pForBodyBlock->label, forEndBlock->label);
    } else {
        bBlocks.back()->addInst(Inst(Op::Br, nullptr, std::make_unique<Label>(pForBodyBlock->label), nullptr));
    }

    bBlocks.push_back(std::move(forEndBlock));

    stackEndLabel.pop();
    stackIterLabel.pop();

    SymTab::iterOut();
    bBlocks.back()->addInst(IR::Inst(
            IR::Op::OutStack, nullptr, nullptr, nullptr));
}

std::unique_ptr<ForStmt> ForStmt::parse() {
    auto n = std::make_unique<ForStmt>();

    n->lVal = LVal::parse();
    singleLex(LexType::ASSIGN);
    n->exp = Exp::parse(false);

    output(AST::ForStmt);
    return n;
}

void ForStmt::genIR(IR::BasicBlocks &basicBlocks) const {
    using namespace IR;
    auto t = exp->genIR(basicBlocks);

    auto sym = SymTab::find(lVal->ident);
    auto irLVal = std::make_unique<Var>(lVal->ident,
                                        SymTab::findDepth(lVal->ident),
                                        sym->cons,
                                        sym->dims,
                                        sym->type);
    basicBlocks.back()->addInst(Inst(IR::Op::Store,
                                     std::move(t),
                                     std::move(irLVal),
                                     nullptr));
}

std::unique_ptr<BreakStmt> BreakStmt::parse() {
    int row = Lexer::curRow;

    if (BigForStmt::inForDepth == 0) {
        Error::raise('m', row);
    }

    Lexer::next();
    singleLex(LexType::SEMICN, row);
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

    if (BigForStmt::inForDepth == 0) {
        Error::raise('m', row);
    }

    Lexer::next();
    singleLex(LexType::SEMICN, row);
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

    if (Lexer::curLexType == LexType::SEMICN) {
        Lexer::next();
    } else {
        if (Stmt::retVoid) {
            Error::raise('f');
        }
        int row = Lexer::curRow; // error handle
        n->exp = Exp::parse(false);
        singleLex(LexType::SEMICN, row);
    }

    return n;
}

bool ReturnStmt::inMainGen;

void ReturnStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    if (exp) {
        auto temp = exp->genIR(bBlocks);
        bBlocks.back()->addInst(Inst(inMainGen ? Op::RetMain : Op::Ret,
                                     nullptr,
                                     std::move(temp),
                                     nullptr));
    } else {
        bBlocks.back()->addInst(Inst(inMainGen ? Op::RetMain : Op::Ret,
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

    singleLex(LexType::LPARENT);

    if (Lexer::curLexType == LexType::STRCON) {
        n->checkFormatString(Lexer::curToken);
        n->formatString = Lexer::curToken;
        Lexer::next();
    } else {
        Error::raise();
    }

    int numOfExp = 0;
    while (Lexer::curLexType == LexType::COMMA) {
        Lexer::next();
        numOfExp++;
        n->exps.push_back(Exp::parse(false));
    }

    if (numOfExp != n->numOfFormat) {
        Error::raise('l', row);
    }

    singleLex(LexType::RPARENT, row);
    singleLex(LexType::SEMICN, row);
    return n;
}

void PrintStmt::addStr(const IR::BasicBlocks &bBlocks, std::string &buffer) {
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

    std::vector<std::unique_ptr<Temp>> args(exps.size());
    for (int i = 0; i < exps.size(); ++i) {
        args[i] = exps[i]->genIR(bBlocks);
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
}

std::unique_ptr<LValStmt> LValStmt::parse() {
    std::unique_ptr<LValStmt> n;

    int row = Lexer::curRow;
    auto lVal = LVal::parse();

    auto sym = SymTab::find(lVal->getIdent());
    if (sym && sym->cons) {
        Error::raise('h', row);
    }

    singleLex(LexType::ASSIGN);

    if (Lexer::curLexType == LexType::GETINTTK) {
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

    singleLex(LexType::GETINTTK);
    singleLex(LexType::LPARENT);
    singleLex(LexType::RPARENT, row);
    singleLex(LexType::SEMICN, row); // ugly error handling!

    return n;
}

void GetIntStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    bBlocks.back()->addInst(Inst(IR::Op::GetInt,
                                 nullptr,
                                 nullptr,
                                 nullptr));

    auto rValue = std::make_unique<Temp>(-static_cast<int>(MIPS::Register::v0), Type::Int);

    auto [symbol, depth] = SymTab::findInGen(lVal->ident);
    auto var = std::make_unique<IR::Var>(
            lVal->ident,
            depth,
            symbol->cons,
            symbol->dims,
            symbol->type,
            symbol->symType);

    if (lVal->dims.empty()) {
        bBlocks.back()->addInst(Inst(IR::Op::Store,
                                     std::move(rValue),
                                     std::move(var),
                                     nullptr));
    } else {
        int constOffset;
        std::unique_ptr<Temp> dynamicOffset;
        bool getNonConstIndex = lVal->getOffset(constOffset, dynamicOffset, bBlocks, symbol->dims);
        if (getNonConstIndex) {
            bBlocks.back()->addInst(Inst(IR::Op::StoreDynamic,
                                         std::move(rValue),
                                         std::move(var),
                                         std::move(dynamicOffset)));
        } else {
            bBlocks.back()->addInst(Inst(IR::Op::Store,
                                         std::move(rValue),
                                         std::move(var),
                                         std::make_unique<ConstVal>(constOffset, Type::Int)));
        }
    }
}

std::unique_ptr<AssignStmt> AssignStmt::parse() {
    auto n = std::make_unique<AssignStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(LexType::SEMICN, row);

    return n;
}

void AssignStmt::genIR(IR::BasicBlocks &bBlocks) {
    using namespace IR;
    auto rValue = exp->genIR(bBlocks);

    auto [symbol, depth] = SymTab::findInGen(lVal->ident);
    auto var = std::make_unique<IR::Var>(
            lVal->ident,
            depth,
            symbol->cons,
            symbol->dims,
            symbol->type,
            symbol->symType);

    if (lVal->dims.empty()) {
        bBlocks.back()->addInst(Inst(IR::Op::Store,
                                     std::move(rValue),
                                     std::move(var),
                                     nullptr));
    } else {
        int constOffset;
        std::unique_ptr<Temp> dynamicOffset;
        bool getNonConstIndex = lVal->getOffset(constOffset, dynamicOffset, bBlocks, symbol->dims);
        if (getNonConstIndex) {
            bBlocks.back()->addInst(Inst(IR::Op::StoreDynamic,
                                         std::move(rValue),
                                         std::move(var),
                                         std::move(dynamicOffset)));
        } else {
            bBlocks.back()->addInst(Inst(IR::Op::Store,
                                         std::move(rValue),
                                         std::move(var),
                                         std::make_unique<ConstVal>(constOffset, Type::Int)));
        }
    }
}

std::unique_ptr<ExpStmt> ExpStmt::parse() {
    auto n = std::make_unique<ExpStmt>();

    int row = Lexer::curRow;
    n->exp = Exp::parse(false);
    singleLex(LexType::SEMICN, row);

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
