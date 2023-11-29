//
// Created by Steel_Shadow on 2023/10/15.
//

#include "Symbol.h"

#include "errorHandler/Error.h"

Type toType(LexType type) {
    switch (type) {
        case LexType::INTTK:
            return Type::Int;
        case LexType::VOIDTK:
            return Type::Void;
        default:
            Error::raise("Bad Btype to IR");
            return Type::Void;
    }
}

int sizeOfType(Type type) {
    switch (type) {
        case Type::Void:
            return 0;
        case Type::Int:
        case Type::IntPtr:
            return 4;
        default:
            Error::raise("Bad Type in sizeOfType(...)");
            return 0;
    }
}

Symbol::Symbol(bool cons, Type type, const std::vector<int> &dims, const std::vector<int> &initVal) :
    symType(SymType::Value),
    type(type),
    cons(cons),
    dims(dims),
    initVal(initVal) {}

Symbol::Symbol(Type reType, const std::vector<Param> &params) :
    symType(SymType::Func),
    type(reType),
    params(params) {}

Symbol::Symbol(Type type, std::vector<int> dims) :
    symType(SymType::Param),
    type(type),
    dims(std::move(dims)) {}
