//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_DEF_H
#define COMPILER_DEF_H

#include "InitVal.h"
#include "AST/expr/Exp.h"

#include <memory>
#include <string>
#include <vector>

// ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
struct Def {
    bool cons;

    std::string ident;

    std::vector<std::unique_ptr<Exp>> dims; // dim is ConstExp
    std::unique_ptr<InitVal> initVal;

    // can be empty for VarDef(cons=false)
    const std::unique_ptr<InitVal> &getInitVal() const;

    static std::unique_ptr<Def> parse(bool cons, Type type);

    const std::string &getIdent() const;

    void genIR(IR::BasicBlocks &bBlocks, Type type) const;

    int getArraySize() const;
};

#endif
