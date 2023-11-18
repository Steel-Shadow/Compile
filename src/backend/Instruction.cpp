//
// Created by Steel_Shadow on 2023/11/15.
//

#include "Instruction.h"

#include <string>

#include "Register.h"
#include "StackMemory.h"
#include "frontend/symTab/SymTab.h"

using namespace MIPS;

std::string MIPS::opToString(Op e) {
    switch (e) {
    case Op::none: return "none";
    case Op::addu: return "addu";
    case Op::subu: return "subu";
    case Op::mul: return "mul";
    case Op::div: return "div";
    case Op::mfhi: return "mfhi";
    case Op::and_: return "and";
    case Op::or_: return "or";
    case Op::sw: return "sw";
    case Op::lw: return "lw";
    case Op::li: return "li";
    case Op::syscall: return "syscall";
    case Op::move: return "move";
    case Op::la: return "la";
    case Op::j: return "j";
    case Op::jal: return "jal";
    case Op::addi: return "addi";
    case Op::jr: return "jr";
    default: return "unknown";
    }
}

Instruction::Instruction(Op op):
    op(op) {
}

R_Inst::R_Inst(Op op, Register rd, Register rs, Register rt):
    Instruction(op),
    rd(rd),
    rs(rs),
    rt(rt) {
}

std::string R_Inst::toString() {
    return opToString(op) + '\t'
        + regToString(rd) + '\t'
        + regToString(rs) + '\t'
        + regToString(rt);
}

I_imm_Inst::I_imm_Inst(Op op, Register rt, Register rs, int immediate):
    Instruction(op),
    rt(rt),
    rs(rs),
    immediate(immediate) {
}

std::string I_imm_Inst::toString() {
    if (op == Op::lw || op == Op::sw) {
        return opToString(op) + '\t'
            + regToString(rt) + '\t'
            + std::to_string(immediate)
            + "(" + regToString(rs) + ")" + '\t';
    } else {
        return opToString(op) + '\t'
            + regToString(rt) + '\t'
            + regToString(rs) + '\t'
            + std::to_string(immediate);
    }
}

I_label_Inst::I_label_Inst(Op op, Register rs, Register rt, const Label& label):
    Instruction(op),
    rs(rs),
    rt(rt),
    label(label) {
}

std::string I_label_Inst::toString() {
    return opToString(op) + '\t'
        + regToString(rs) + '\t'
        + regToString(rt) + '\t'
        + label.nameAndId;
}

J_Inst::J_Inst(Op op, const Label& label):
    Instruction(op),
    label(label) {
}

std::string J_Inst::toString() {
    return opToString(op) + '\t'
        + label.toString();
}

void MIPS::InStack(IR::Inst&) {
    StackMemory::Allocate();
}


void MIPS::OutStack(IR::Inst&) {
    StackMemory::Deallocate();
}

void MIPS::Assign(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Var*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());

    if (res->depth == 0) {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(arg1), Register::gp, getGlobOffset(res)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(arg1), Register::sp, getStackOffset(res)));
    }
}

void MIPS::Add(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::addu, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Sub(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Mul(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::mul, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Div(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::div, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Mod(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::div, Register::none, getReg(arg1), getReg(arg2)));

    assemblies.push_back(std::make_unique<R_Inst>(Op::mfhi, newReg(res), Register::none, Register::none));
}

void MIPS::And(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::and_, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Or(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());
    auto arg2 = dynamic_cast<IR::Temp*>(inst.arg2.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::or_, newReg(res), getReg(arg1), getReg(arg2)));
}

void MIPS::Neg(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Temp*>(inst.arg1.get());

    assemblies.push_back(std::make_unique<R_Inst>(Op::subu, newReg(res), Register::zero, getReg(arg1)));
}

void MIPS::LoadImd(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto imm = dynamic_cast<IR::ConstVal*>(inst.arg1.get());

    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, newReg(res), Register::none, imm->value));
}

void MIPS::GetInt(IR::Inst& inst) {
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 5));

    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));

    auto res = dynamic_cast<IR::Var*>(inst.res.get());
    if (res->depth == 0) {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::v0, Register::gp, getGlobOffset(res)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::v0, Register::sp, getStackOffset(res)));
    }
}

void MIPS::PrintInt(IR::Inst& inst) {
    auto t = dynamic_cast<IR::Temp*>(inst.arg1.get());

    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 1));
    assemblies.push_back(std::make_unique<R_Inst>(Op::move, Register::a0, getReg(t), Register::none));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));
}

void MIPS::PrintStr(IR::Inst& inst) {
    auto str = dynamic_cast<IR::Str*>(inst.arg1.get());

    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::li, Register::v0, Register::none, 4));
    assemblies.push_back(std::make_unique<I_label_Inst>(Op::la, Register::a0, Register::none, Label(str->toString())));
    assemblies.push_back(std::make_unique<R_Inst>(Op::syscall, Register::none, Register::none, Register::none));
}

void MIPS::Cmp(IR::Inst& inst) {
    // todo: 继续写 MIPS 代码生成
}

void MIPS::Alloca(IR::Inst& inst) {
    auto var = dynamic_cast<IR::Var*>(inst.arg1.get());
    auto size = dynamic_cast<IR::ConstVal*>(inst.arg2.get());

    int byte = sizeOfType(size->type) * size->value;

    StackMemory::curOffset += byte;
    StackMemory::varToOffset[var] = StackMemory::curOffset;
}

void MIPS::Load(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Var*>(inst.arg1.get());

    if (arg1->depth == 0) {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, newReg(res), Register::gp, getGlobOffset(arg1)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, newReg(res), Register::sp, getStackOffset(arg1)));
    }
}

void MIPS::Store(IR::Inst& inst) {
    auto res = dynamic_cast<IR::Temp*>(inst.res.get());
    auto arg1 = dynamic_cast<IR::Var*>(inst.arg1.get());

    if (arg1->depth == 0) {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(res), Register::gp, getGlobOffset(arg1)));
    } else {
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(res), Register::sp, getStackOffset(arg1)));
    }
}

void MIPS::Br(IR::Inst& inst) {
    auto label = Label(dynamic_cast<IR::Label*>(inst.arg1.get()));
    assemblies.push_back(std::make_unique<J_Inst>(Op::j, label));
}

void MIPS::Bif0(IR::Inst& inst) {
    // todo: 代码生成二
}

void MIPS::Call(IR::Inst& inst) {
    auto func = Label(dynamic_cast<IR::Label*>(inst.arg1.get()));

    // save %n(IR::Temp) -> $t0-$t3
    for (int i = static_cast<int>(Register::t3); i >= static_cast<int>(Register::t0); --i) {
        StackMemory::curOffset += 4;
        assemblies.push_back(
            std::make_unique<I_imm_Inst>(Op::sw, static_cast<Register>(i), Register::sp, -StackMemory::curOffset));
    }

    // save $sp $ra
    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::sp, Register::sp, -StackMemory::curOffset));
    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, Register::ra, Register::sp, -StackMemory::curOffset));

    // move $sp for called function
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::addi, Register::ra, Register::sp, -StackMemory::curOffset));

    // jal to function body
    assemblies.push_back(std::make_unique<J_Inst>(Op::jal, func));
}

void MIPS::PushParam(IR::Inst& inst) {
    // todo: 代码生成二 数组传参
    // 优化 使用 a0-a3 传参
    auto param = dynamic_cast<IR::Temp*>(inst.res.get());

    StackMemory::curOffset += 4;
    assemblies.push_back(std::make_unique<I_imm_Inst>(Op::sw, getReg(param), Register::sp, StackMemory::curOffset));
}

void MIPS::Ret(IR::Inst& inst) {
    if (auto ret = dynamic_cast<IR::Temp*>(inst.arg1.get())) {
        assemblies.push_back(std::make_unique<R_Inst>(Op::move, Register::v0, getReg(ret), Register::none));
    }

    // main don't need restore the environment
    // In MIPS gen, main is the first function
    static bool firstIsMainRet = true;
    if (!firstIsMainRet) {
        // restore $ra $sp
        int offset = 0;
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Register::ra, Register::sp, offset));
        offset += 4;
        assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, Register::sp, Register::sp, offset));
        offset += 4;

        // resotre %n(IR::Temp) -> $t0-$t3
        for (int i = static_cast<int>(Register::t0); i <= static_cast<int>(Register::t3); ++i) {
            assemblies.push_back(std::make_unique<I_imm_Inst>(Op::lw, static_cast<Register>(i), Register::sp, offset));
            offset += 4;
        }
        assemblies.push_back(std::make_unique<R_Inst>(Op::jr, Register::none, Register::ra, Register::none));
    } else {
        firstIsMainRet = true;
    }
}
