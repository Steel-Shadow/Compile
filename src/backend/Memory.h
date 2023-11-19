//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef STACKMEMORY_H
#define STACKMEMORY_H
#include <stack>
#include <unordered_map>

#include "middle/IR.h"

namespace MIPS {
inline int gp_init = 0x1000'8000;
inline int data_segment = 0x1001'0000;

int getGlobOffset(IR::Var& var);
int getStackOffset(IR::Var& var);


// MIPS memory
// 0 -> 4GB
// ra sp t0 t1 t2 t3  p0(Function parameter) p1 ...
// 0  1  2  3  4  5   6=(6+0)                7=(6+1)
//       -----------
//       MAX_TEMP_REGS
//
// when generating MIPS form IR,
// if we get inst.op == InStack/outStack,
// push/pop curOffset into/from stack<int> offsetStack
namespace StackMemory {
    // clear when generating MIPS for a new Function
    extern std::unordered_map<IR::Var, int> varToOffset;

    extern int curOffset;
    extern std::stack<int> offsetStack;

    void Allocate();
    void Deallocate();
}
}

#endif //STACKMEMORY_H
