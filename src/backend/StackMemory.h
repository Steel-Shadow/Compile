//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef STACKMEMORY_H
#define STACKMEMORY_H
#include <stack>
#include <unordered_map>

#include "middle/IR.h"

namespace MIPS {
// when generating MIPS form IR,
// if we get inst.op == InStack/outStack,
// push/pop curOffset into/from stack<int> offsetStack
namespace StackMemory {
    // clear when generating MIPS for a FuncDef
    extern std::unordered_map<IR::Var*, int> varToOffset;

    extern int curOffset;
    extern std::stack<int> offsetStack;

    void Allocate();
    void Deallocate();
}
}

#endif //STACKMEMORY_H
