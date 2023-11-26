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

std::unique_ptr<IR::Module> CompUnit::genIR() const {
    using namespace IR;

    auto module = std::make_unique<Module>("Write by Steel Shadow");

    // maybe redundant, but still set it for safety
    SymTab::cur = &SymTab::global;

    for (auto &decl: decls) {
        for (auto &def: decl->getDefs()) {
            auto sym = SymTab::find(def->ident);

            auto globVar = GlobVar(sym->cons, sym->dims, sym->initVal);
            module->getGlobVars().emplace_back(def->ident, globVar);
        }
    }

    for (auto &funcDef: funcDefs) {
        module->getFunctions().push_back(funcDef->genIR());
    }
    module->getFunctions().push_back(mainFuncDef->genIR());
    return module;
}
