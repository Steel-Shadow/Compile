//
// Created by Steel_Shadow on 2023/9/26.
//

#ifndef COMPILER_LEXTYPE_H
#define COMPILER_LEXTYPE_H

#include <string>

enum class LexType {
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
};

#endif //COMPILER_LEXTYPE_H
