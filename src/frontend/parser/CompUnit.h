//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_COMPUNIT_H
#define COMPILER_COMPUNIT_H

#include <vector>
#include <memory>

#include "decl/Decl.h"
#include "func/Func.h"
#include "middle/IR.h"

// CompUnit → {Decl} {FuncDef} MainFuncDef
struct CompUnit {
    std::vector<std::unique_ptr<Decl>> decls;
    std::vector<std::unique_ptr<FuncDef>> funcDefs;
    std::unique_ptr<MainFuncDef> mainFuncDef;

    static std::unique_ptr<CompUnit> parse();

    std::unique_ptr<IR::Module> genIR();
};

#endif //COMPILER_COMPUNIT_H
