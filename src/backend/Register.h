//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef REGISTER_H
#define REGISTER_H
#include <stack>

#include "middle/IR.h"
#include "tools/CircularQueue.h"

namespace MIPS {
enum class Register {
    zero,
    at,
    v0,
    v1,
    a0,
    a1,
    a2,
    a3,
    t0,
    t1,
    t2,
    t3,
    t4,
    t5,
    t6,
    t7,
    s0,
    s1,
    s2,
    s3,
    s4,
    s5,
    s6,
    s7,
    t8,
    t9,
    k0,
    k1,
    gp,
    sp,
    fp,
    ra,
    none,
};

constexpr int MAX_TEMP_REGS = 4;

// $t0-$t3 (8-11)
extern int curRealReg;

// %n(IR::Temp) -> $t0-$t3
extern CircularQueue<std::pair<int, Register>, MAX_TEMP_REGS> circularTempRegs;

Register newReg(IR::Temp* temp);
Register getReg(IR::Temp* temp);

std::string regToString(Register reg);
}

#endif //REGISTER_H
