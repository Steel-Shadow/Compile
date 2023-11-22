//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Instruction.h"

#include <string>

#include "Register.h"
#include "Memory.h"
#include "frontend/symTab/SymTab.h"
#include "frontend/error/Error.h"

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
        default:
            Error::raise("unknown Op");
            return "unknown";
    }
}

Instruction::Instruction(Op op) :
        op(op) {
}

R_Inst::R_Inst(Op op, Register rd, Register rs, Register rt) :
        Instruction(op),
        rd(rd),
        rs(rs),
        rt(rt) {
}

std::string R_Inst::toString() {
    return opToString(op) + ' '
           + regToString(rd) + ' '
           + regToString(rs) + ' '
           + regToString(rt);
}

I_imm_Inst::I_imm_Inst(Op op, Register rt, Register rs, int immediate) :
        Instruction(op),
        rt(rt),
        rs(rs),
        immediate(immediate) {
}

std::string I_imm_Inst::toString() {
    if (op == Op::lw || op == Op::sw) {
        return opToString(op) + ' '
               + regToString(rt) + ' '
               + std::to_string(immediate)
               + "(" + regToString(rs) + ")";
    } else {
        return opToString(op) + ' '
               + regToString(rt) + ' '
               + regToString(rs) + ' '
               + std::to_string(immediate);
    }
}

I_label_Inst::I_label_Inst(Op op, Register rs, Register rt, const Label &label) :
        Instruction(op),
        rs(rs),
        rt(rt),
        label(label) {
}

std::string I_label_Inst::toString() {
    return opToString(op) + ' '
           + regToString(rs) + ' '
           + regToString(rt) + ' '
           + label.nameAndId;
}

J_Inst::J_Inst(Op op, const Label &label) :
        Instruction(op),
        label(label) {
}

std::string J_Inst::toString() {
    return opToString(op) + ' '
           + label.nameAndId;
}

void MIPS::InStack(IR::Inst &) {
    StackMemory::Allocate();
}


void MIPS::OutStack(IR::Inst &) {
    StackMemory::Deallocate();
}

void MIPS::Assign(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Var *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    if (res->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::sw, getReg(arg1), Register::none, Label(res->name)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(arg1), Register::sp, -getStackOffset(*res)));
    }
}

void MIPS::Add(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::addu, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Sub(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Mul(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::mul, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Div(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::div, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Mod(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    assemblies.push_back(std::make_unique<R_Inst>(Op::div, Register::none, reg1, reg2));
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::mfhi, regRes, Register::none, Register::none));
    checkTempReg(res, regRes);
}

void MIPS::And(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::and_, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Or(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp *>(inst.arg2.get());

    Register reg1 = getReg(arg1);
    Register reg2 = getReg(arg2);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::or_, regRes, reg1, reg2));
    checkTempReg(res, regRes);
}

void MIPS::Neg(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Register reg1 = getReg(arg1);
    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, regRes, Register::zero, reg1));
    checkTempReg(res, regRes);
}

void MIPS::LoadImd(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto imm = dynamic_cast<IR::ConstVal *>(inst.arg1.get());

    Register regRes = newReg(res);
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, regRes, Register::none, imm->value));
    checkTempReg(res, regRes);
}

void MIPS::GetInt(IR::Inst &inst) {
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 5));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));

    auto res = dynamic_cast<IR::Var *>(inst.res.get());
    if (res->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::sw, Register::v0, Register::none, Label(res->name)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::v0, Register::sp, -getStackOffset(*res)));
    }
}

void MIPS::PrintInt(IR::Inst &inst) {
    auto t = dynamic_cast<IR::Temp *>(inst.arg1.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::move, Register::a0, getReg(t), Register::none));
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 1));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));
}

void MIPS::PrintStr(IR::Inst &inst) {
    auto str = dynamic_cast<IR::Str *>(inst.arg1.get());

    assemblies.push_back(std::make_unique<I_label_Inst>(Op::la, Register::a0, Register::none, Label(str->toString())));
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 4));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));
}

void MIPS::Cmp(IR::Inst &inst) {
    // todo: 继续写 MIPS 代码生成
}

void MIPS::Alloca(IR::Inst &inst) {
    auto var = dynamic_cast<IR::Var *>(inst.arg1.get());
    auto size = dynamic_cast<IR::ConstVal *>(inst.arg2.get());

    int byte = sizeOfType(size->type) * size->value;

    StackMemory::curOffset += byte;
    StackMemory::varToOffset[*var] = StackMemory::curOffset;
}

void MIPS::Load(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Var *>(inst.arg1.get());

    Register regRes = newReg(res);
    if (arg1->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::lw, regRes, Register::none, Label(arg1->name)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, regRes, Register::sp, -getStackOffset(*arg1)));
    }
    checkTempReg(res, regRes);
}

void MIPS::Store(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Var *>(inst.arg1.get());

    if (arg1->depth == 0) {
        assemblies.push_back(std::make_unique<I_label_Inst>(Op::sw, getReg(res), Register::none, Label(arg1->name)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(res), Register::sp, -getStackOffset(*arg1)));
    }
}

void MIPS::Br(IR::Inst &inst) {
    auto label = Label(dynamic_cast<IR::Label *>(inst.arg1.get()));
    assemblies.push_back(std::make_unique<J_Inst>(Op::j, label));
}

void MIPS::Bif0(IR::Inst &inst) {
    // todo: 代码生成二
}

void MIPS::Call(IR::Inst &inst) {
    auto func = Label(dynamic_cast<IR::Label *>(inst.arg1.get()));

    // save tempRegs
    for (int i = static_cast<int>(Register::t0) + MAX_TEMP_REGS - 1; i >= static_cast<int>(Register::t0); --i) {
        StackMemory::curOffset += 4;
        assemblies.push_back(std::make_unique<I_imm_Inst>(
                Op::sw, static_cast<Register>(i), Register::sp, -StackMemory::curOffset));
    }

    // save $sp $ra
    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::sp, Register::sp, -StackMemory::curOffset));
    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::ra, Register::sp, -StackMemory::curOffset));

    // allocate stack for call
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Register::sp, Register::sp, -StackMemory::curOffset));
    // jal to function body
    assemblies.push_back(std::make_unique<J_Inst>(Op::jal, func));
    // deAllocate stack for call
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Register::sp, Register::sp, StackMemory::curOffset));

    // restore $ra $sp
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Register::ra, Register::sp, -StackMemory::curOffset));
    StackMemory::curOffset -= 4;
    int spShouldBeTheLastToRestore = StackMemory::curOffset;
    StackMemory::curOffset -= 4;

    // restore tempRegs
    for (int i = static_cast<int>(Register::t0); i < static_cast<int>(Register::t0) + MAX_TEMP_REGS; ++i) {
        assemblies.push_back(
                std::make_unique<I_imm_Inst>(Op::lw, static_cast<Register>(i), Register::sp, -StackMemory::curOffset));
        StackMemory::curOffset -= 4;
    }

    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Register::sp, Register::sp, -spShouldBeTheLastToRestore));
}

void MIPS::PushParam(IR::Inst &inst) {
    // todo: 代码生成二 数组传参
    auto param = dynamic_cast<IR::Temp *>(inst.arg1.get());

    // set function's parameters to varToOffset is done in MIPS.cpp
    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(param), Register::sp, -StackMemory::curOffset));
}

void MIPS::Ret(IR::Inst &inst) {
    static bool firstIsMainRet = true;

    auto ret = dynamic_cast<IR::Temp *>(inst.arg1.get());
    if (firstIsMainRet) {
        firstIsMainRet = false;
        if (ret) {
            assemblies.push_back(std::make_unique<R_Inst>(Op::move, Register::a0, getReg(ret), Register::none));
        }
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 17));
        assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));
    } else {
        if (ret) {
            assemblies.push_back(std::make_unique<R_Inst>(Op::move, Register::v0, getReg(ret), Register::none));
        }
        assemblies.push_back(std::make_unique<R_Inst>(Op::jr, Register::none, Register::ra, Register::none));
    }
}

void MIPS::NewMove(IR::Inst &inst) {
    auto res = dynamic_cast<IR::Temp *>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp *>(inst.arg1.get());

    Register regRes = newReg(res);
    Register reg1 = getReg(arg1);
    assemblies.push_back(std::make_unique<R_Inst>(Op::move, regRes, reg1, Register::none));
    checkTempReg(res, regRes);
}
