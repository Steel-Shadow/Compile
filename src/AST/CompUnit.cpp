//
// Created by Steel_Shadow on 2023/10/11.
//

#include "CompUnit.h"

#include "frontend/lexer/Lexer.h"
#include "frontend/parser/Parser.h"
#include "frontend/symTab/SymTab.h"

using namespace Parser;

std::unique_ptr<CompUnit> CompUnit::parse() {
    auto n = std::make_unique<CompUnit>();

    while (Lexer::curLexType == LexType::CONSTTK || Lexer::curLexType == LexType::INTTK) {
        if (Lexer::peek(1).first == LexType::IDENFR && Lexer::peek(2).first == LexType::LPARENT
            || Lexer::curLexType == LexType::INTTK && Lexer::peek(1).first == LexType::MAINTK) {
            break;
        }
        n->decls.push_back(Decl::parse());
    }

    while (Lexer::curLexType == LexType::VOIDTK || Lexer::curLexType == LexType::INTTK) {
        if (Lexer::curLexType == LexType::INTTK && Lexer::peek(1).first == LexType::MAINTK) {
            break;
        }
        n->funcDefs.push_back(FuncDef::parse());
    }

    n->mainFuncDef = MainFuncDef::parse();

    output(AST::CompUnit);

    return n;
}

std::unique_ptr<IR::Module> CompUnit::genIR() const {
    using namespace IR;

    auto module = std::make_unique<Module>("Write by Steel Shadow");

    // maybe redundant, but still set it for safety
    SymTab::cur = &SymTab::global;

    for (auto &decl: decls) {
        for (auto &def: decl->getDefs()) {
            auto sym = SymTab::find(def->ident);

            auto globVar = GlobVar(sym->cons, sym->dims, sym->initVal);
            module->addGlobVar(def->ident, globVar);
        }
    }

    for (auto &funcDef: funcDefs) {
        module->addFunction(funcDef->genIR());
    }

    module->setMainFunction(mainFuncDef->genIR());

    return module;
}
