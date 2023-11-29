//
// Created by Steel_Shadow on 2023/9/26.
//

#ifndef COMPILER_NODETYPE_H
#define COMPILER_NODETYPE_H

#include <string>

enum class LexType {
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
};

std::string toString(LexType type);

#endif
