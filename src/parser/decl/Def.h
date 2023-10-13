//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_DEF_H
#define COMPILER_DEF_H

#include <string>
#include <vector>
#include <memory>

#include "parser/expr/Exp.h"
#include "InitVal.h"

// ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
// VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
class Def {
    bool cons;

    std::string ident;
    std::vector<std::unique_ptr<Exp>> dims; // dim is ConstExp
    std::unique_ptr<InitVal> initVal; // can be empty for VarDef(cons=false)

public:
    static std::unique_ptr<Def> parse(bool cons);
};

#endif //COMPILER_DEF_H
