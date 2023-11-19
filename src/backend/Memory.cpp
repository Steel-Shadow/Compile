//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Memory.h"
using namespace MIPS;

std::unordered_map<IR::Var*, int> StackMemory::varToOffset;

int StackMemory::curOffset;
std::stack<int> StackMemory::offsetStack{{0}};

void StackMemory::Allocate() {
    offsetStack.push(curOffset);
}

void StackMemory::Deallocate() {
    curOffset = offsetStack.top();
    offsetStack.pop();
}

int MIPS::getGlobOffset(IR::Var* var) {
    // todo: 代码生成二 数组长度计算
    int i = data_segment - gp_init;
    for (auto [name,globVar] : IR::Module::getGlobVars()) {
        if (name == var->name) {
            return i;
        }
        i += 4;
    }
    return -1;
}

int MIPS::getStackOffset(IR::Var* var) {
    return StackMemory::varToOffset[var];
}
