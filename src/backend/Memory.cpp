//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Memory.h"

using namespace MIPS;

std::unordered_map<IR::Var, int> StackMemory::varToOffset;

int StackMemory::curOffset;
std::stack<int> StackMemory::offsetStack{};

int MIPS::getStackOffset(IR::Var &var) {
    return StackMemory::varToOffset[var];
}
