//
// Created by Steel_Shadow on 2023/10/15.
//

#include "Symbol.h"

Symbol::Symbol(bool cons, const std::vector<int> &dims, const std::vector<int> &initVal) :
        type(SymType::Value),
        cons(cons),
        dims(dims),
        initVal(initVal) {
}

Symbol::Symbol(NodeType reType, const std::vector<Param> &params) :
        type(SymType::Func),
        reType(reType),
        params(params) {
}

Symbol::Symbol(const std::vector<int> &dims) :
        type(SymType::Param),
        dims(dims) {
}
