//
// Created by Steel_Shadow on 2023/10/12.
//
#ifndef COMPILER_STMT_H
#define COMPILER_STMT_H

#include <memory>
#include <vector>
#include <string>
#include <stack>
#include "middle/IR.h"

struct Cond;

struct LVal;

struct Exp;

/*-------------------------Block-----------------------------*/
// BlockItem → Decl | Stmt
struct BlockItem {
    virtual ~BlockItem() = default;

    static std::unique_ptr<BlockItem> parse();

    // generate IR to BasicBlocks
    virtual void genIR(IR::BasicBlocks& bBlocks) = 0;
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
struct Stmt : public BlockItem {
    static std::unique_ptr<Stmt> parse();

    static bool retVoid; // check return in FuncDef
};

/*-----------------------------------------------------------*/

// LVal '=' 'getint''('')'';' | LVal '=' Exp ';'
struct LValStmt : public Stmt {
    std::unique_ptr<LVal> lVal;

    static std::unique_ptr<LValStmt> parse();
};

// LVal '=' Exp ';'
struct AssignStmt : public LValStmt {
    std::unique_ptr<Exp> exp;

    static std::unique_ptr<AssignStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// [Exp] ';'
struct ExpStmt : public Stmt {
    std::unique_ptr<Exp> exp;

    static std::unique_ptr<ExpStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// Block → '{' { BlockItem } '}'
struct Block {
    std::vector<std::unique_ptr<BlockItem>> blockItems;

    const std::vector<std::unique_ptr<BlockItem>>& getBlockItems() const;

    static std::unique_ptr<Block> parse();

    static int lastRow; // show return error message

    void genIR(IR::BasicBlocks& basicBlocks);
};

// Block
struct BlockStmt : public Stmt {
    std::unique_ptr<Block> block;

    static std::unique_ptr<BlockStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
struct IfStmt : public Stmt {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Stmt> ifStmt;
    std::unique_ptr<Stmt> elseStmt;

    static std::unique_ptr<IfStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// 'break' ';'
struct BreakStmt : public Stmt {
    static std::unique_ptr<BreakStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// 'continue' ';'
struct ContinueStmt : public Stmt {
    static std::unique_ptr<ContinueStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// ForStmt → LVal '=' Exp
struct ForStmt {
    std::unique_ptr<LVal> lVal;
    std::unique_ptr<Exp> exp;

    static std::unique_ptr<ForStmt> parse();

    void genIR(IR::BasicBlocks& basicBlocks) const;
};

// 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
struct BigForStmt : public Stmt {
    std::unique_ptr<ForStmt> init;
    std::unique_ptr<Cond> cond;
    std::unique_ptr<ForStmt> iter;
    std::unique_ptr<Stmt> stmt;

    // for error handling
    static bool inFor;

    // stack of nested BigForStmt
    // used for break & continue
    static std::stack<IR::Label> stackEndLabel;
    static std::stack<IR::Label> stackIterLabel;

    static std::unique_ptr<BigForStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// 'return' [Exp] ';'
struct ReturnStmt : public Stmt {
    std::unique_ptr<Exp> exp;

    static std::unique_ptr<ReturnStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// | LVal '=' 'getint''('')'';'
struct GetIntStmt : public LValStmt {
    static std::unique_ptr<GetIntStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;
};

// 'printf''('FormatString{','Exp}')'';'
struct PrintStmt : public Stmt {
    std::string formatString;
    std::vector<std::unique_ptr<Exp>> exps;

    int numOfFormat; // error handling

    static std::unique_ptr<PrintStmt> parse();

    void genIR(IR::BasicBlocks& bBlocks) override;

private:
    void checkFormatString(const std::string& str);

    static std::vector<std::string> spilt(std::string input, const std::string& pattern);
};

#endif //COMPILER_STMT_H
