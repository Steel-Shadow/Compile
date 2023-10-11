//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_DECL_H
#define COMPILER_DECL_H

#include <memory>
#include <string>
#include <vector>

#include "parser/expr/Exp.h"
#include "InitVal.h"

using namespace std;

// ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
class Def {
    bool isConst;

    string ident;
    vector<unique_ptr<Exp>> dims;
    unique_ptr<InitVal> initVal;

public:
    explicit Def(bool isConst);
};

// Decl → ConstDecl | VarDecl
// ConstDecl→'const' BType ConstDef {','ConstDef}';'
// VarDecl → BType VarDef { ',' VarDef } ';'
class Decl {
    bool isConst;

    vector<unique_ptr<Def>> defs;

public:
    Decl();
};


#endif //COMPILER_DECL_H
