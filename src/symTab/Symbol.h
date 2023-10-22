//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H


#include <vector>
#include "lexer/NodeType.h"

enum class SymType {
    Value, // const var
    Func,
    Param
};

typedef std::vector<int> Dimensions;

struct Symbol {
    // Value -> a | a[...] | a[...][...]
    // Func -> func
    // Param -> p | p[] | p[][...]
    SymType type;

    // value & array. dims also share for param
    bool cons{false}; // const | var
    std::vector<int> dims; // At most 2 dimensions in our work.

    // func
    NodeType reType{NodeType::LEX_EMPTY};

    std::vector<Dimensions> params; // only save dimension

    Symbol(bool cons, const std::vector<int> &dims); // const var
    Symbol(NodeType reType, const std::vector<Dimensions> &params); // func
    explicit Symbol(const std::vector<int> &dims); // param
};


#endif //COMPILER_SYMBOL_H
