//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef STACKMEMORY_H
#define STACKMEMORY_H

#include "middle/IR.h"

#include <stack>
#include <unordered_map>

namespace MIPS {
constexpr int gp_init = 0x1000'8000;
constexpr int data_segment = 0x1001'0000;

int getStackOffset(const IR::Var *var);

// MIPS stack memory map
// In funcCall, StackMemory::curOffset = 0
// 0 -> 4GB
//                                       $sp
//                                        ↓
//   s7 ...  s1  s0 | t7  ... t1 t0 ra sp | p0(parameter) p1 ... pN-1 |
//  -18 ... -12 -11 | -10 ... -4 -3 -2 -1 | 0             1  ... N-1  |
// -------------
// MAX_TEMP_REGS = 8
// MAX_VAR_REGS = 8
//
// when generating MIPS form IR,
// if we get inst.op == InStack/outStack (BigForStmt IfStmt BlockStmt),
// push/pop curOffset into/from stack<int> offsetStack
namespace StackMemory {
// clear when generating MIPS for a new Function
extern std::unordered_map<IR::Var, int> varToOffset;

extern int curOffset;
extern std::stack<int> offsetStack;
} // namespace StackMemory
} // namespace MIPS

#endif //STACKMEMORY_H
