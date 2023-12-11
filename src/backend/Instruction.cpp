//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Instruction.h"

#include <string>
#include <utility>

#include "Register.h"
#include "Memory.h"
#include "frontend/symTab/SymTab.h"
#include "errorHandler/Error.h"

#include <string>

using namespace MIPS;

std::string MIPS::opToString(Op e) {
    switch (e) {
        case Op::none:
            return "none";
        case Op::addu:
            return "addu";
        case Op::subu:
            return "subu";
        case Op::mul:
            return "mul";
        case Op::div:
            return "div";
        case Op::mfhi:
            return "mfhi";
        case Op::and_:
            return "and";
        case Op::or_:
            return "or";
        case Op::sw:
            return "sw";
        case Op::lw:
            return "lw";
        case Op::li:
            return "li";
        case Op::syscall:
            return "syscall";
        case Op::move:
            return "move";
        case Op::la:
            return "la";
        case Op::j:
            return "j";
        case Op::jal:
            return "jal";
        case Op::addi:
            return "addi";
        case Op::jr:
            return "jr";
        case Op::slt:
            return "slt";
        case Op::sle:
            return "sle";
        case Op::sge:
            return "sge";
        case Op::sgt:
            return "sgt";
        case Op::seq:
            return "seq";
        case Op::sne:
            return "sne";
        case Op::bgtz:
            return "bgtz";
        case Op::beqz:
            return "beqz";
        case Op::add:
            return "add";
        case Op::sll:
            return "sll";
        case Op::bne:
            return "bne";
        default:
            Error::raise("unknown Op");
            return "unknown";
    }
}

Instruction::Instruction(Op op) :
    op(op) {}

R_Inst::R_Inst(Op op, Reg rd, Reg rs, Reg rt) :
    Instruction(op),
    rd(rd),
    rs(rs),
    rt(rt) {}

std::string R_Inst::toString() {
    return opToString(op) + '\t'
           + regToString(rd) + '\t'
           + regToString(rs) + '\t'
           + regToString(rt);
}

I_imm_Inst::I_imm_Inst(Op op, Reg rt, Reg rs, int immediate) :
    Instruction(op),
    rt(rt),
    rs(rs),
    immediate(immediate) {}

std::string I_imm_Inst::toString() {
    if (op == Op::lw || op == Op::sw) {
        return opToString(op) + '\t'
               + regToString(rt) + '\t'
               + std::to_string(immediate)
               + "(" + regToString(rs) + ")";
    } else {
        return opToString(op) + '\t'
               + regToString(rt) + '\t'
               + regToString(rs) + '\t'
               + std::to_string(immediate);
    }
}

I_label_Inst::I_label_Inst(Op op, Reg rs, Reg rt, Label label, int offset) :
    Instruction(op),
    rs(rs),
    rt(rt),
    label(std::move(label)),
    offset(offset) {}

std::string I_label_Inst::toString() {
    if (rt == Reg::none) {
        return opToString(op) + '\t'
               + regToString(rs) + '\t'
               + label.nameAndId
               + (offset == 0 ? "" : " + " + std::to_string(offset));
    } else {
        if (op == Op::bne)
            return opToString(op) + '\t'
                   + regToString(rs) + '\t'
                   + regToString(rt) + '\t'
                   + label.nameAndId;

        return opToString(op) + '\t'
               + regToString(rs) + '\t'
               + label.nameAndId
               + (offset == 0 ? "" : " + " + std::to_string(offset))
               + "(" + regToString(rt) + ")";
    }
}

J_Inst::J_Inst(Op op, Label label) :
    Instruction(op),
    label(std::move(label)) {}

std::string J_Inst::toString() {
    return opToString(op) + '\t'
           + label.nameAndId;
}

void MIPS::InStack(const IR::Inst &) {
    StackMemory::offsetStack.push(StackMemory::curOffset);
}


void MIPS::OutStack(const IR::Inst &) {
    StackMemory::curOffset = StackMemory::offsetStack.top();
    StackMemory::offsetStack.pop();
}

void MIPS::Store(const IR::Inst &inst) {
    auto value = dynamic_cast<IR::Temp *>(inst.res.get());
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());

    // const index of array
    int arrayOffset = 0;
    if (inst.arg2) {
        auto arg2 = dynamic_cast<IR::ConstVal *>(inst.arg2.get());
        arrayOffset = sizeOfType(var->type) * arg2->value;
    }

    if (var->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::sw, getReg(value), Reg::none, Label(var->name), arrayOffset));
    } else {
        if (var->symType == SymType::Param && !var->dims.empty()) {
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::t8, Reg::sp, -getStackOffset(var)));
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(value), Reg::t8, arrayOffset));
        } else {
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(value), Reg::sp, -getStackOffset(var) + arrayOffset));
        }
    }
}

void MIPS::StoreDynamic(const IR::Inst &inst) {
    auto value = dynamic_cast<IR::Temp *>(inst.res.get());
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());
    auto offset = dynamic_cast<IR::Temp *>(inst.arg2.get());

    if (var->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::sw, getReg(value), getReg(offset), Label(var->name)));
    } else {
        if (var->symType == SymType::Param && !var->dims.empty()) {
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::t8, Reg::sp, -getStackOffset(var)));
            assemblies.push_back(std::make_unique<R_Inst>(Op::add, Reg::t8, Reg::t8, getReg(offset)));
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(value), Reg::t8, 0));
        } else {
            assemblies.push_back(std::make_unique<R_Inst>(Op::add, Reg::t8, Reg::sp, getReg(offset)));
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(value), Reg::t8, -getStackOffset(var)));
        }
    }
}

void MIPS::Add(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::addu, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Sub(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Mul(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::mul, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Div(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::div, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Mod(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    assemblies.push_back(std::make_unique<R_Inst>(Op::div, Reg::none, reg1, reg2));
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::mfhi, regRes, Reg::none, Reg::none));
    checkTempReg(res, regRes);
}

void MIPS::And(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::and_, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Or(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::or_, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Neg(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Reg reg1 = getReg(arg1);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, regRes, Reg::zero, reg1));
    checkTempReg(res, regRes);
}

void MIPS::Not(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Reg reg1 = getReg(arg1);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::seq, regRes, Reg::zero, reg1));
    checkTempReg(res, regRes);
}

void MIPS::LoadImd(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto imm = dynamic_cast<IR::ConstVal *>(inst.arg1.get());

    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, regRes, Reg::none, imm->value));
    checkTempReg(res, regRes);
}

void MIPS::GetInt(const IR::Inst &inst) {
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Reg::v0, Reg::none, 5));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Reg::none, Reg::none, Reg::none));
}

void MIPS::PrintInt(const IR::Inst &inst) {
    auto t = dynamic_cast<IR::Temp *>(inst.arg1.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::move, Reg::a0, getReg(t), Reg::none));
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Reg::v0, Reg::none, 1));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Reg::none, Reg::none, Reg::none));
}

void MIPS::PrintStr(const IR::Inst &inst) {
    auto str = dynamic_cast<IR::Str *>(inst.arg1.get());

    assemblies.push_back(std::make_unique<I_label_Inst>(Op::la, Reg::a0, Reg::none, Label(str->toString())));
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Reg::v0, Reg::none, 4));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Reg::none, Reg::none, Reg::none));
}

void MIPS::Alloca(const IR::Inst &inst) {
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());
    auto size = dynamic_cast<IR::ConstVal *>(inst.arg2.get());

    int byte = sizeOfType(var->type) * size->value;

    StackMemory::curOffset += byte;
    StackMemory::varToOffset[*var] = StackMemory::curOffset;
}

void MIPS::Load(const IR::Inst &inst) {
    auto temp = dynamic_cast<IR::Temp *>(inst.res.get());
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());

    // const index of array
    int arrayOffset = 0;
    if (inst.arg2) {
        auto constOffset = dynamic_cast<IR::ConstVal *>(inst.arg2.get());
        arrayOffset = sizeOfType(var->type) * constOffset->value;
    }

    Reg regRes = newReg(temp);
    if (var->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::lw, regRes, Reg::none, Label(var->name), arrayOffset));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, regRes, Reg::sp, -getStackOffset(var) + arrayOffset));
    }
    checkTempReg(temp, regRes);
}

void MIPS::LoadPtr(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto addr = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto offset = dynamic_cast<IR::ConstVal *>(inst.arg2.get());

    Reg regAddr = getReg(addr);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, regRes, regAddr, offset ? offset->value * sizeOfType(res->type) : 0));
    checkTempReg(res, regRes);
}

void MIPS::LoadDynamic(const IR::Inst &inst) {
    auto value = dynamic_cast<IR::Temp *>(inst.res.get());
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());
    auto offset = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg regOffset = getReg(offset);
    Reg regValue = newReg(value);
    if (var->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::lw, regValue, regOffset, Label(var->name)));
    } else {
        assemblies.push_back(std::make_unique<R_Inst>(Op::add, Reg::t8, Reg::sp, regOffset));
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, regValue, Reg::t8, -getStackOffset(var)));
    }
    checkTempReg(value, regValue);
}

void MIPS::Br(const IR::Inst &inst) {
    auto label = Label(dynamic_cast<IR::Label *>(inst.arg1.get()));
    assemblies.push_back(std::make_unique<J_Inst>(Op::j, label));
}

void MIPS::Bif0(const IR::Inst &inst) {
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto label = Label(dynamic_cast<IR::Label *>(inst.arg2.get()));

    assemblies.push_back(std::make_unique<I_label_Inst>(Op::beqz, getReg(arg1), Reg::none, label));
}

void MIPS::Call(const IR::Inst &inst) {
    auto func = Label(dynamic_cast<IR::Label *>(inst.arg1.get()));

    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Reg::sp, Reg::sp, -StackMemory::curOffset));
    StackMemory::offsetStack.push(StackMemory::curOffset);
    StackMemory::curOffset = 0;

    // save $sp $ra
    StackMemory::curOffset += wordSize;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Reg::sp, Reg::sp, -StackMemory::curOffset));
    StackMemory::curOffset += wordSize;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Reg::ra, Reg::sp, -StackMemory::curOffset));

    // save tempRegs
    // Only save used tempRegs, but we still allocate MAX_TEMP_REGS for stack of called function.
    // In this way, we don't need to consider tempRegs after jal.
    // I choose to use more stack memory, but fewer instructions.
    // Another way is, before jal , save the number of used tempRegs to a realReg $?,
    // use the realReg $? to locate parameters instead of $sp. (slower but less memory use of stack)
    for (auto &[tempId, reg]: tempToRegs) {
        StackMemory::curOffset += wordSize;
        assemblies.push_back(std::make_unique<I_imm_Inst>(
            Op::sw, reg, Reg::sp, -StackMemory::curOffset));
    }

    // jal to function body
    assemblies.push_back(std::make_unique<J_Inst>(Op::jal, func));

    // restore tempRegs
    for (auto i = tempToRegs.rbegin(); i != tempToRegs.rend(); ++i) {
        assemblies.push_back(std::make_unique<I_imm_Inst>(
            Op::lw, i->second, Reg::sp, -StackMemory::curOffset));
        StackMemory::curOffset -= wordSize;
    }

    // restore $ra $sp
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::ra, Reg::sp, -StackMemory::curOffset));
    StackMemory::curOffset -= wordSize;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::sp, Reg::sp, -StackMemory::curOffset));
    StackMemory::curOffset -= wordSize;

    // StackMemory::curOffset -= SymTab::find(func.nameAndId)->params.size() * wordSize;
    // deAllocate stack for call
    StackMemory::curOffset = StackMemory::offsetStack.top();
    StackMemory::offsetStack.pop();
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Reg::sp, Reg::sp, StackMemory::curOffset));
}

void MIPS::PushParam(const IR::Inst &inst) {
    int paramIndex = dynamic_cast<IR::ConstVal *>(inst.res.get())->value;
    auto param = dynamic_cast<IR::Temp *>(inst.arg1.get());

    if (paramIndex < 3) {
        auto reg = static_cast<Reg>(paramIndex + static_cast<int>(Reg::a1));
        assemblies.push_back(std::make_unique<R_Inst>(Op::move, reg, getReg(param), Reg::none));
    } else {
        // set function's parameters to varToOffset is done in MIPS.cpp
        StackMemory::curOffset += wordSize;
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(param), Reg::sp, -StackMemory::curOffset));
    }
}

void MIPS::PushAddressParam(const IR::Inst &inst) {
    int paramIndex = dynamic_cast<IR::ConstVal *>(inst.res.get())->value;
    auto varAddr = dynamic_cast<IR::Var *>(inst.arg1.get());

    // set function's parameters to varToOffset is done in MIPS.cpp
    if (varAddr->depth == 0) {
        if (auto constOffset = dynamic_cast<IR::ConstVal *>(inst.arg2.get())) {
            int offset = constOffset->value * wordSize;
            assemblies.push_back(std::make_unique<I_label_Inst>(Op::la, Reg::t8, Reg::none, Label(varAddr->name), offset));
        } else {
            auto dynamicOffset = dynamic_cast<IR::Temp *>(inst.arg2.get());
            assemblies.push_back(std::make_unique<I_label_Inst>(Op::la, Reg::t8, getReg(dynamicOffset), Label(varAddr->name)));
        }
    } else {
        if (auto constOffset = dynamic_cast<IR::ConstVal *>(inst.arg2.get())) {
            // int offset = constOffset->value * wordSize - getStackOffset(varAddr);
            if (varAddr->symType == SymType::Param && !varAddr->dims.empty()) {
                assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::t8, Reg::sp, -getStackOffset(varAddr)));
                assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Reg::t8, Reg::t8, constOffset->value * wordSize));
            } else {
                assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Reg::t8, Reg::sp,
                                                                  constOffset->value * wordSize - getStackOffset(varAddr)));
            }
        } else {
            if (varAddr->symType == SymType::Param && !varAddr->dims.empty()) {
                auto dynamicOffset = dynamic_cast<IR::Temp *>(inst.arg2.get());
                assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Reg::t8, Reg::sp, -getStackOffset(varAddr)));
                assemblies.push_back(std::make_unique<R_Inst>(Op::add, Reg::t8, Reg::t8, getReg(dynamicOffset)));
            } else {
                auto dynamicOffset = dynamic_cast<IR::Temp *>(inst.arg2.get());
                assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Reg::t8, Reg::sp, -getStackOffset(varAddr)));
                assemblies.push_back(std::make_unique<R_Inst>(Op::add, Reg::t8, Reg::t8, getReg(dynamicOffset)));
            }
        }
    }

    if (paramIndex < 3) {
        auto reg = static_cast<Reg>(paramIndex + static_cast<int>(Reg::a1));
        assemblies.push_back(std::make_unique<R_Inst>(Op::move, reg, Reg::t8, Reg::none));
    } else {
        // set function's parameters to varToOffset is done in MIPS.cpp
        StackMemory::curOffset += wordSize;
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Reg::t8, Reg::sp, -StackMemory::curOffset));
    }
}

void MIPS::Ret(const IR::Inst &inst) {
    auto ret = dynamic_cast<IR::Temp *>(inst.arg1.get());
    if (ret) {
        assemblies.push_back(std::make_unique<R_Inst>(Op::move, Reg::v0, getReg(ret), Reg::none));
    }
    assemblies.push_back(std::make_unique<R_Inst>(Op::jr, Reg::none, Reg::ra, Reg::none));
}

void MIPS::RetMain(const IR::Inst &inst) {
    auto ret = dynamic_cast<IR::Temp *>(inst.arg1.get());
    if (ret) {
        assemblies.push_back(std::make_unique<R_Inst>(Op::move, Reg::a0, getReg(ret), Reg::none));
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Reg::v0, Reg::none, 17));
        assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Reg::none, Reg::none, Reg::none));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Reg::v0, Reg::none, 10));
        assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Reg::none, Reg::none, Reg::none));
    }
}

void MIPS::TempMove(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Reg regRes = newReg(res);
    Reg reg1 = getReg(arg1);
    assemblies.push_back(std::make_unique<R_Inst>(Op::move, regRes, reg1, Reg::none));
    checkTempReg(res, regRes);
}

void MIPS::Leq(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::sle, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Lss(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::slt, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Geq(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::sge, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Gre(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::sgt, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Eql(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::seq, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Neq(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Reg reg1 = getReg(arg1);
    Reg reg2 = getReg(arg2);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::sne, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Bif1(const IR::Inst &inst) {
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto label = Label(dynamic_cast<IR::Label *>(inst.arg2.get()));

    assemblies.push_back(std::make_unique<I_label_Inst>(Op::bne, getReg(arg1), Reg::zero, label));
}

void MIPS::MulImd(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto imm = dynamic_cast<IR::ConstVal *>(inst.arg2.get());

    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::mul, regRes, getReg(arg1), imm->value));
    checkTempReg(res, regRes);
}

void MIPS::Mult4(const IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Reg reg1 = getReg(arg1);
    Reg regRes = newReg(res);
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sll, regRes, reg1, 2));
    checkTempReg(res, regRes);
}
