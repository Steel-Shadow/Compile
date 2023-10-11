//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_COMPUNIT_H
#define COMPILER_COMPUNIT_H

#include <vector>
#include <memory>

#include "decl/Decl.h"
#include "func/Func.h"

using namespace std;

// CompUnit → {Decl} {FuncDef} MainFuncDef
class CompUnit {
    vector<unique_ptr<Decl> > decls;
    vector<unique_ptr<FuncDef>> funcDefs;
    unique_ptr<MainFuncDef> mainFuncDef;

public:
    CompUnit();
};

#endif //COMPILER_COMPUNIT_H
