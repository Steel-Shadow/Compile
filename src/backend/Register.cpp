//
// Created by Steel_Shadow on 2023/11/15.
//

#include "tools/CircularQueue.h"
#include "Register.h"

#include "frontend/error/Error.h"

using namespace MIPS;

int MIPS::curRealReg = 0;
CircularQueue<std::pair<int, Register>, 4> MIPS::circularTempRegs;

Register MIPS::newReg(IR::Temp* temp) {
    static int index = 0;
    index = (index + 1) % MAX_TEMP_REGS;
    curRealReg = index + static_cast<int>(Register::t0);

    circularTempRegs.enqueue(std::make_pair(temp->id, static_cast<Register>(curRealReg)));
    return static_cast<Register>(curRealReg);
}

Register MIPS::getReg(IR::Temp* temp) {
    for (int i = 0; i < MAX_TEMP_REGS; i++) {
        if (circularTempRegs.getData()[i].first == temp->id) {
            return circularTempRegs.getData()[i].second;
        }
    }
    Error::raise("IR::Temp not map to a valid MIPS::Register");
    return Register::zero;
}

std::string MIPS::regToString(Register reg) {
    switch (reg) {
    case Register::zero: return "$zero";
    case Register::at: return "$at";
    case Register::v0: return "$v0";
    case Register::v1: return "$v1";
    case Register::a0: return "$a0";
    case Register::a1: return "$a1";
    case Register::a2: return "$a2";
    case Register::a3: return "$a3";
    case Register::t0: return "$t0";
    case Register::t1: return "$t1";
    case Register::t2: return "$t2";
    case Register::t3: return "$t3";
    case Register::t4: return "$t4";
    case Register::t5: return "$t5";
    case Register::t6: return "$t6";
    case Register::t7: return "$t7";
    case Register::s0: return "$s0";
    case Register::s1: return "$s1";
    case Register::s2: return "$s2";
    case Register::s3: return "$s3";
    case Register::s4: return "$s4";
    case Register::s5: return "$s5";
    case Register::s6: return "$s6";
    case Register::s7: return "$s7";
    case Register::t8: return "$t8";
    case Register::t9: return "$t9";
    case Register::k0: return "$k0";
    case Register::k1: return "$k1";
    case Register::gp: return "$gp";
    case Register::sp: return "$sp";
    case Register::fp: return "$fp";
    case Register::ra: return "$ra";
    case Register::none: return "";
    default: return "unknown";
    }
}
