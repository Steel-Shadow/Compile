//
// Created by Steel_Shadow on 2023/10/11.
//

#include "CompUnit.h"

#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace Parser;
using namespace CodeGen;

std::unique_ptr<CompUnit> CompUnit::parse() {
    auto n = std::make_unique<CompUnit>();

    while (Lexer::curLexType == NodeType::CONSTTK || Lexer::curLexType == NodeType::INTTK) {
        if (Lexer::peek(1).first == NodeType::IDENFR && Lexer::peek(2).first == NodeType::LPARENT
            || Lexer::curLexType == NodeType::INTTK && Lexer::peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->decls.push_back(Decl::parse());
    }

    while (Lexer::curLexType == NodeType::VOIDTK || Lexer::curLexType == NodeType::INTTK) {
        if (Lexer::curLexType == NodeType::INTTK && Lexer::peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->funcDefs.push_back(FuncDef::parse());
    }

    n->mainFuncDef = MainFuncDef::parse();

    output(NodeType::CompUnit);

    return n;
}

std::unique_ptr<CodeGen::Module> CompUnit::codeGen() {
    auto module = std::make_unique<Module>("Made from Steel Shadow");

    for (auto &decl: decls) {
        for (auto &def: decl->getDefs()) {
            auto &i = def->getInitVal();
            module->getGlobals().push_back(std::make_pair(
                    def->getIdent(), i->evaluate()));
        }
    }

    for (auto &funcDef: funcDefs) {
        module->getFunctions().push_back(funcDef->codeGen());
    }

    return module;
}
