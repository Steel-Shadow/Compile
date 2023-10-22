//
// Created by Steel_Shadow on 2023/10/12.
//
#ifndef COMPILER_STMT_H
#define COMPILER_STMT_H

#include <memory>
#include <vector>
#include <string>
#include "lexer/NodeType.h"

class Cond;

class LVal;

class Exp;

/*-------------------------Block-----------------------------*/
// BlockItem → Decl | Stmt
class BlockItem {
public:
    virtual ~BlockItem() = default;

public:
    static std::unique_ptr<BlockItem> parse();
};

// Stmt → LVal '=' Exp ';'
// | [Exp] ';'
// | Block
// | 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
// | 'break' ';' | 'continue' ';'
// | 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
// | 'return' [Exp] ';'
// | LVal '=' 'getint''('')'';'
// | 'printf''('FormatString{','Exp}')'';'
class Stmt : public BlockItem {

public:
    static std::unique_ptr<Stmt> parse();

    static bool retVoid;
};
/*-----------------------------------------------------------*/

// LVal '=' 'getint''('')'';' | LVal '=' Exp ';'
class LValStmt : public Stmt {
    std::unique_ptr<LVal> lVal;

public:
    static std::unique_ptr<LValStmt> parse();
};

// LVal '=' Exp ';'
class AssignStmt : public LValStmt {
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<AssignStmt> parse();
};

// [Exp] ';'
class ExpStmt : public Stmt {
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<ExpStmt> parse();
};

// Block → '{' { BlockItem } '}'
class Block {
    std::vector<std::unique_ptr<BlockItem>> blockItems;
public:
    const std::vector<std::unique_ptr<BlockItem>> &getBlockItems() const;

public:
    static std::unique_ptr<Block> parse();

    [[maybe_unused]]NodeType getReType();

    static int lastRow;
};

// Block
class BlockStmt : public Stmt {
    std::unique_ptr<Block> block;

public:
    static std::unique_ptr<BlockStmt> parse();
};

// 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
class IfStmt : public Stmt {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Stmt> ifStmt;
    std::unique_ptr<Stmt> elseStmt;

public:
    static std::unique_ptr<IfStmt> parse();
};

// 'break' ';'
class BreakStmt : public Stmt {
public:
    static std::unique_ptr<BreakStmt> parse();
};

// 'continue' ';'
class ContinueStmt : public Stmt {
public:
    static std::unique_ptr<ContinueStmt> parse();
};

// ForStmt → LVal '=' Exp
class ForStmt {
    std::unique_ptr<LVal> lVal;
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<ForStmt> parse();
};

// 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
class BigForStmt : public Stmt {
    std::unique_ptr<ForStmt> init;
    std::unique_ptr<Cond> cond;
    std::unique_ptr<ForStmt> iter;
    std::unique_ptr<Stmt> stmt;

public:
    static bool inFor;

    static std::unique_ptr<BigForStmt> parse();
};

// 'return' [Exp] ';'
class ReturnStmt : public Stmt {
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<ReturnStmt> parse();

    [[nodiscard]] const std::unique_ptr<Exp> &getExp() const;
};

// | LVal '=' 'getint''('')'';'
class GetintStmt : public LValStmt {

public:
    static std::unique_ptr<GetintStmt> parse();
};

// 'printf''('FormatString{','Exp}')'';'
class PrintStmt : public Stmt {
    std::string formatString;
    std::vector<std::unique_ptr<Exp>> exps;

    int numOfFormat;

public:
    static std::unique_ptr<PrintStmt> parse();

private:
    void checkFormatString(std::string str);

    void checkFormatNum();
};

#endif //COMPILER_STMT_H
