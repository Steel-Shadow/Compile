//
// Created by Steel_Shadow on 2023/9/25.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Error.h"
#include "Lexer.h"
#include "SynType.h"
#include <iostream>

class Parser {
private:
    static Parser *instance;

    explicit Parser(std::ofstream *o, Lexer &lexer);

    Lexer &lexer;

    std::ofstream *outFileStream;

public:
    // Singleton
    static Parser *getInstance(Lexer &lexer, std::ofstream *o = nullptr);

    Parser(Parser const &) = delete;

    void operator=(Parser const &) = delete;
    // Singleton

    // CompUnit→{Decl}{FuncDef}MainFuncDef
    void CompUnit();

    // Decl→ConstDecl|VarDecl
    void Decl();

    // FuncDef→FuncTypeIdent'('[FuncFParams]')'Block
    void FuncDef();

    // MainFuncDef→'int''main''('')'Block
    void MainFuncDef();

    // ConstDecl→'const'BTypeConstDef{','ConstDef}';'
    void ConstDecl();
};


#endif //COMPILER_PARSER_H
