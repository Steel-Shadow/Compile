//
// Created by Steel_Shadow on 2023/11/15.
//

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "MIPS.h"
#include "Register.h"

namespace MIPS {
enum class Op {
    // IR::Instruction.op == Empty
    // it's not nop
    none,
    addu, subu, mul, div, mfhi, and_, or_,
    addi,
    move,
    sw, lw,
    li, la,
    syscall,
    j,
    jal,
    jr,
};

std::string opToString(Op e);

struct Instruction : public Assembly {
    Op op;

    explicit Instruction(Op op = Op::none);
};

struct R_Inst : public Instruction {
    Register rd;
    Register rs;
    Register rt;

    R_Inst(Op op, Register rd, Register rs, Register rt);

    std::string toString() override;
};

struct I_imm_Inst : public Instruction {
    Register rs;
    Register rt;
    int immediate;

    I_imm_Inst(Op op, Register rt, Register rs, int immediate);

    std::string toString() override;
};

struct I_label_Inst : public Instruction {
    Register rs;
    Register rt;
    Label label;

    I_label_Inst(Op op, Register rs, Register rt, const Label &label);

    std::string toString() override;
};

struct J_Inst : public Instruction {
    Label label;

    explicit J_Inst(Op op, const Label &label);

    std::string toString() override;
};

void InStack(IR::Inst &);

void OutStack(IR::Inst &);

void Assign(IR::Inst &);

void Add(IR::Inst &);

void Sub(IR::Inst &);

void Mul(IR::Inst &);

void Div(IR::Inst &);

void Mod(IR::Inst &);

void And(IR::Inst &);

void Or(IR::Inst &);

void Neg(IR::Inst &);

void LoadImd(IR::Inst &);

void GetInt(IR::Inst &);

void PrintInt(IR::Inst &);

void PrintStr(IR::Inst &);

void Cmp(IR::Inst &);

void Alloca(IR::Inst &);

void Load(IR::Inst &);

void Store(IR::Inst &);

void Br(IR::Inst &);

void Bif0(IR::Inst &);

void Call(IR::Inst &);

void PushParam(IR::Inst &);

void Ret(IR::Inst &);

void NewMove(IR::Inst &);

#endif //INSTRUCTION_H
}