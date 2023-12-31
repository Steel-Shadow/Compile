//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef REGISTER_H
#define REGISTER_H

#include "middle/IR.h"
#include <map>
#include <queue>


#include <unordered_map>

namespace MIPS {
// @formatter:off
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
    t8,
    t9,

    k0,
    k1,
    gp,
    sp,

    // if freeTempRegs is empty, use $fp to store/load temp into/from stack
    fp,

    ra,
    none,
}; // @formatter:on


// at least 5 for correctness
constexpr int MAX_TEMP_REGS = 8;
constexpr int MAX_VAR_REGS = 8;

extern std::map<int, Register> tempToRegs;
extern std::queue<Register> freeTempRegs;

extern std::map<IR::Var, Register> varToRegs;
extern std::queue<Register> freeVarRegs;

Register newReg(const IR::Temp *temp);

Register getReg(const IR::Temp *temp);

// if freeTempRegs is empty (reg==$t8), we should store temp on stack
void checkTempReg(const IR::Temp *temp, Register reg);

void clearRegs();

std::string regToString(Register reg);

} // namespace MIPS

#endif
