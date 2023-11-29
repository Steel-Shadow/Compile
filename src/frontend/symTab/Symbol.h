//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <vector>
#include "frontend/lexer/LexType.h"

struct Symbol;

using Param = std::pair<std::string, Symbol *>;

enum class SymType {
    // const var
    Value,

    Func,
    Param
};

enum class Type {
    Void,
    Int,
    IntPtr,
};

Type toType(LexType type);

int sizeOfType(Type type);

// all information in a Symbol (also redundant info)
// use SymType type to distinguish
struct Symbol {
    // Value -> a | a[...] | a[...][...]
    // Func -> func
    // Param -> p | p[] | p[][...]
    SymType symType;

    // Value's type | Func's return type
    Type type;

    // value & array. dims also share for param
    bool cons{false}; // const | var
    std::vector<int> dims; // At most 2 dimensions in our work.
    std::vector<int> initVal; // for const Value

    // func
    std::vector<Param> params;

    // const var
    Symbol(bool cons, Type type, const std::vector<int> &dims, const std::vector<int> &initVal);
    // func
    Symbol(Type reType, const std::vector<Param> &params);
    // param
    explicit Symbol(Type type, std::vector<int> dims);
};

#endif
