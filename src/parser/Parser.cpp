//
// Created by Steel_Shadow on 2023/9/25.
//

#include "Parser.h"

Parser *Parser::instance = nullptr;

Parser::Parser(std::ofstream *o, Lexer &lexer) : outFileStream(o), lexer(lexer) {}

Parser *Parser::getInstance(Lexer &lexer, std::ofstream *o) {
    if (instance == nullptr) {
        instance = new Parser(o, lexer);
    }

    return instance;
}

void Parser::CompUnit() {
//    todo: parser
//    while()
    Decl();
    FuncDef();
    MainFuncDef();
}

void Parser::Decl() {

}


void Parser::FuncDef() {

}

void Parser::MainFuncDef() {

}

void Parser::ConstDecl() {

}