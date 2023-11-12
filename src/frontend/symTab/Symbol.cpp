//
// Created by Steel_Shadow on 2023/10/15.
//

#include "Symbol.h"

Symbol::Symbol(bool cons, const std::vector<int> &dims) :
        type(SymType::Value),
        cons(cons), dims(dims) {}

Symbol::Symbol(NodeType reType, const std::vector<Dimensions> &params) :
        type(SymType::Func),
        reType(reType),
        params(params) {}

Symbol::Symbol(const std::vector<int> &dims) :
        type(SymType::Param),
        dims(dims) {}

