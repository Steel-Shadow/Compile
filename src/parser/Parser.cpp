//
// Created by Steel_Shadow on 2023/9/25.
//

#include "Parser.h"

Parser *Parser::instance = nullptr;

Parser::Parser(std::ofstream *o) : outFileStream(o) {}

Parser *Parser::getInstance(std::ofstream *o) {
    if (instance == nullptr) {
        instance = new Parser(o);
    }

    return instance;
}

void Parser::CompUnit() {
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