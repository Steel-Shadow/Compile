//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Memory.h"

using namespace MIPS;

std::unordered_map<IR::Var, int> StackMemory::varToOffset;
std::unordered_map<IR::Var, Reg> StackMemory::varToReg;

int StackMemory::curOffset;
std::stack<int> StackMemory::offsetStack;

int MIPS::getStackOffset(const IR::Var *var) {
    return StackMemory::varToOffset[*var];
}
