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
    [[nodiscard]] NodeType getType() const;

    static std::unique_ptr<FuncType> parse();
};

// FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
class FuncFParam {
    std::unique_ptr<Btype> type;
    std::string ident;
    std::vector<std::unique_ptr<Exp>> dims; // if [], dims[0]=nullptr

public:
    static std::unique_ptr<FuncFParam> parse();

    [[nodiscard]] const std::string &getId() const;

    std::vector<int> getDims();
};

// FuncFParams → FuncFParam { ',' FuncFParam }
class FuncFParams {
    std::vector<std::unique_ptr<FuncFParam>> funcFParams;

public:
    static std::unique_ptr<FuncFParams> parse();

    [[nodiscard]] std::vector<Dimensions> getDimsVec() const;
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
    [[nodiscard]] const std::vector<std::unique_ptr<Exp>> &getParams() const;

public:
    static std::unique_ptr<FuncRParams> parse();
};

#endif //COMPILER_FUNC_H
