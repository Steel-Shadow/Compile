//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_DEF_H
#define COMPILER_DEF_H

#include <string>
#include <vector>
#include <memory>

#include "frontend/parser/expr/Exp.h"
#include "InitVal.h"

// ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
struct Def {
    bool cons;

    std::string ident;

    std::vector<std::unique_ptr<Exp>> dims; // dim is ConstExp
    std::unique_ptr<InitVal> initVal;

    const std::unique_ptr<InitVal> &getInitVal() const;
    // can be empty for VarDef(cons=false)

    static std::unique_ptr<Def> parse(bool cons);

    const std::string &getIdent() const;

    void genIR(IR::BasicBlocks &bBlocks);

    int getArraySize() const;
};

#endif //COMPILER_DEF_H
