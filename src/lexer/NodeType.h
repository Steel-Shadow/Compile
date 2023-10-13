//
// Created by Steel_Shadow on 2023/9/26.
//

#ifndef COMPILER_NODETYPE_H
#define COMPILER_NODETYPE_H

#include <string>

enum class NodeType {
    // LexType
    LEX_EMPTY,
    COMMENT,

    IDENFR,
    INTCON,
    STRCON,

    MAINTK,
    CONSTTK,
    INTTK,
    BREAKTK,
    CONTINUETK,
    IFTK,
    ELSETK,
    AND,
    OR,
    FORTK,
    GETINTTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    VOIDTK,
    MULT,
    DIV,
    MOD,
    LEQ,
    LSS,
    GEQ,
    GRE,
    EQL,
    NEQ,
    NOT,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,

    LEX_END,

    // SynType
    CompUnit,
    Decl,
    FuncDef,
    MainFuncDef,
    FuncType,
    ConstDecl,
    VarDecl,
    ConstDef,
    ConstExp,
    ConstInitVal,
    VarDef,
    InitVal,
    FuncFParams,
    FuncFParam,
    Btype,
    Block,
    BlockItem,
    Stmt,
    ForStmt,
    Exp,
    Cond,
    LVal,
    PrimaryExp,
    Number,
    UnaryExp,
    UnaryOp,
    FuncRParams,
    MulExp,
    AddExp,
    RelExp,
    EqExp,
    LAndExp,
    LOrExp,
};

std::string typeToStr(NodeType type);

#endif //COMPILER_NODETYPE_H
