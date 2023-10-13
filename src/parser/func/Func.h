//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_FUNC_H
#define COMPILER_FUNC_H

#include <memory>
#include <vector>

#include "lexer/NodeType.h"
#include "parser/decl/Decl.h"

// FuncType → 'void' | 'int'
class FuncType {
    NodeType type;
public:
    static std::unique_ptr<FuncType> parse();
};

// FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
class FuncFParam {
    std::unique_ptr<Btype> type;
    std::string ident;
    std::vector<std::unique_ptr<Exp>> dims;

public:
    static std::unique_ptr<FuncFParam> parse();
};

// FuncFParams → FuncFParam { ',' FuncFParam }
class FuncFParams {
    std::vector<std::unique_ptr<FuncFParam>> funcFParams;
public:
    static std::unique_ptr<FuncFParams> parse();
};

//FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
class FuncDef {
    std::unique_ptr<FuncType> funcType;
    std::string ident;
    std::unique_ptr<FuncFParams> funcFParams;
    std::unique_ptr<Block> block;

public:
    static std::unique_ptr<FuncDef> parse();
};

// MainFuncDef→'int''main''('')'Block
class MainFuncDef {
    std::unique_ptr<Block> block;

public:
    static std::unique_ptr<MainFuncDef> parse();
};


// FuncRParams → Exp { ',' Exp }
class FuncRParams {
    std::vector<std::unique_ptr<Exp>> params;

public:
    static std::unique_ptr<FuncRParams> parse();
};

#endif //COMPILER_FUNC_H
