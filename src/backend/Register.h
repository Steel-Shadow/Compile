//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef REGISTER_H
#define REGISTER_H

#include <queue>
#include <map>
#include "middle/IR.h"

namespace MIPS {
enum class Register {
    zero,

    // used by assembler, I can use too, but must confirm that it don't conflict with pseudo MIPS
    at,

    // return value of function
    v0,
    v1,

    // parameter of function
    a0,
    a1,
    a2,
    a3,

    // $t0-$t{MAX_TEMP_REGS-1} is used in tempToRegs(IR::Temp.id -> real register)
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

    // if freeTempRegs is empty, use $t8 to store/load temp into/from stack
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

// at least 4 for correctness
constexpr int MAX_TEMP_REGS = 8;

extern std::map<int, Register> tempToRegs;
extern std::queue<Register> freeTempRegs;

Register newReg(const IR::Temp *temp);

Register getReg(const IR::Temp *temp);

// if freeTempRegs is empty (reg==$t8), we should store temp on stack
void checkTempReg(const IR::Temp *temp, Register reg);

void clearTempRegs();

std::string regToString(Register reg);

}

#endif //REGISTER_H
