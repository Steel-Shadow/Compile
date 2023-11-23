//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <vector>
#include "frontend/lexer/NodeType.h"

struct Symbol;

using Dimensions = std::vector<int>;
using Param = std::pair<std::string, Dimensions>;

enum class SymType {
    Value,
    // const var
    Func,
    Param
};


// all information in a Symbol (also redundant info)
// use SymType type to distinguish
struct Symbol {
    // Value -> a | a[...] | a[...][...]
    // Func -> func
    // Param -> p | p[] | p[][...]
    SymType type;

    // value & array. dims also share for param
    bool cons{false}; // const | var
    std::vector<int> dims; // At most 2 dimensions in our work.
    std::vector<int> initVal; // for const Value


    // func
    NodeType reType{NodeType::LEX_EMPTY};
    std::vector<Param> params; // only save dimension

    Symbol(bool cons, const std::vector<int> &dims, const std::vector<int> &initVal = {}); // const var
    Symbol(NodeType reType, const std::vector<Param> &params); // func
    explicit Symbol(const std::vector<int> &dims); // param
};

#endif //COMPILER_SYMBOL_H
