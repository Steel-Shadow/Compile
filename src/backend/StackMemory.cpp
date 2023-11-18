//
// Created by Steel_Shadow on 2023/11/15.
//

#include "StackMemory.h"
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
