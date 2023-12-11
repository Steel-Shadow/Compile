//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Register.h"
#include "Instruction.h"
#include "Memory.h"
#include "MIPS.h"

using namespace MIPS;

// use template to dynamically generate
template<std::size_t... Indices>
constexpr auto genCleanRegs(std::index_sequence<Indices...>) {
    return std::queue<Reg>{{static_cast<Reg>(Indices + static_cast<size_t>(Reg::t0))...}};
}

template<std::size_t N>
constexpr auto cleanRegQueue() {
    return genCleanRegs<>(std::make_index_sequence<N>{});
}

std::map<int, Reg> MIPS::tempToRegs;
std::queue<Reg> MIPS::freeTempRegs = cleanRegQueue<MAX_TEMP_REGS>();

Reg MIPS::newReg(const IR::Temp *temp) {
    if (temp->id < 0) {
        return static_cast<Reg>(-temp->id);
    } else if (freeTempRegs.empty()) {
        return Reg::t8;
    } else {
        Reg r = freeTempRegs.front();
        freeTempRegs.pop();
        tempToRegs[temp->id] = r;
        return r;
    }
}

Reg MIPS::getReg(const IR::Temp *temp) {
    if (temp->id < 0) {
        return static_cast<Reg>(-temp->id);
    } else {
        auto tempToReg = tempToRegs.find(temp->id);
        if (tempToReg == tempToRegs.end()) {
            // tempToReg not found, temp has been stored in memory
            assemblies.push_back(std::make_unique<I_imm_Inst>(
                    Op::lw,
                    Reg::t8,
                    Reg::sp,
                    -StackMemory::varToOffset[IR::Var(temp->toString(), -1)]));
            return Reg::t8;
        } else {
            Reg t = tempToReg->second;
            freeTempRegs.push(tempToReg->second);
            tempToRegs.erase(temp->id);
            return t;
        }
    }
}

std::string MIPS::regToString(Reg reg) {
    switch (reg) {
        case Reg::zero:
            return "$zero";
        case Reg::at:
            return "$at";
        case Reg::v0:
            return "$v0";
        case Reg::v1:
            return "$v1";
        case Reg::a0:
            return "$a0";
        case Reg::a1:
            return "$a1";
        case Reg::a2:
            return "$a2";
        case Reg::a3:
            return "$a3";
        case Reg::t0:
            return "$t0";
        case Reg::t1:
            return "$t1";
        case Reg::t2:
            return "$t2";
        case Reg::t3:
            return "$t3";
        case Reg::t4:
            return "$t4";
        case Reg::t5:
            return "$t5";
        case Reg::t6:
            return "$t6";
        case Reg::t7:
            return "$t7";
        case Reg::s0:
            return "$s0";
        case Reg::s1:
            return "$s1";
        case Reg::s2:
            return "$s2";
        case Reg::s3:
            return "$s3";
        case Reg::s4:
            return "$s4";
        case Reg::s5:
            return "$s5";
        case Reg::s6:
            return "$s6";
        case Reg::s7:
            return "$s7";
        case Reg::t8:
            return "$t8";
        case Reg::t9:
            return "$t9";
        case Reg::k0:
            return "$k0";
        case Reg::k1:
            return "$k1";
        case Reg::gp:
            return "$gp";
        case Reg::sp:
            return "$sp";
        case Reg::fp:
            return "$fp";
        case Reg::ra:
            return "$ra";
        case Reg::none:
            return "";
        default:
            return "unknown";
    }
}

void MIPS::clearTempRegs() {
    tempToRegs.clear();
    freeTempRegs = cleanRegQueue<MAX_TEMP_REGS>();
}

void MIPS::checkTempReg(const IR::Temp *temp, Reg reg) {
    if (reg == Reg::t8) {
        // if freeTempRegs is empty (reg==$t8),
        // we should store temp on stack
        auto var = IR::Var(temp->toString(), -1);
        StackMemory::curOffset += wordSize;
        StackMemory::varToOffset[var] = StackMemory::curOffset;
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw,
                                                          Reg::t8,
                                                          Reg::sp,
                                                          -StackMemory::curOffset));
    }
}
