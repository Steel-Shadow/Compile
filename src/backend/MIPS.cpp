//
// Created by Steel_Shadow on 2023/10/31.
//
#include "MIPS.h"

#include <iostream>

#include "config.h"
#include "Instruction.h"
#include "StackMemory.h"
#include "frontend/error/Error.h"

using namespace MIPS;

std::ofstream MIPS::mipsFileStream;
std::vector<std::unique_ptr<Assembly>> MIPS::assemblies;

void MIPS::output(const std::string& str, bool newLine) {
#if defined(FILEOUT_MIPS)
    mipsFileStream << str;
    if (newLine) {
        mipsFileStream << '\n';
    }
#endif
#if defined(STDOUT_MIPS)
    std::cout << str;
    if (newLine) {
        std::cout << '\n';
    }
#endif
}

Label::Label(const std::string& name_and_id): nameAndId(name_and_id) {
}

Label::Label(const IR::Label* label): nameAndId(label->nameAndId) {
}

std::string Label::toString() {
    return nameAndId + ":";
}

void MIPS::outputAll(IR::Module& module) {
    /*---- .data output ----------------------*/
    output("#### MIPS ####");
    output(".data");
    for (auto [name,globVar] : module.getGlobVars()) {
        output(name + ": .word ", false);
        for (auto i = globVar.initVal.rbegin(); i != globVar.initVal.rend(); ++i) {
            output(std::to_string(*i) + ",", false);
        }
        output("");
    }
    int i = 0;
    for (auto str : IR::Str::MIPS_strings) {
        output("str_" + std::to_string(i) + ": .asciiz " + str);
        i++;
    }

    /*----- .text generate ---------------------*/
    output("");
    output(".text");
    // main
    for (auto& basicBlock : module.getFunctions().back()->getBasicBlocks()) {
        assemblies.push_back(std::make_unique<Label>(basicBlock->label.nameAndId));
        for (auto& inst : basicBlock->instructions) {
            irToMips(inst);
        }
    }

    // other function (except main)
    for (auto& func : module.getFunctions()) {
        for (auto basicBlock = func->getBasicBlocks().begin();
             basicBlock != std::prev(func->getBasicBlocks().end());
             ++basicBlock) {
            assemblies.push_back(std::make_unique<Label>((*basicBlock)->label.nameAndId));
            for (auto& inst : (*basicBlock)->instructions) {
                irToMips(inst);
            }
        }
    }

    /*----- .text optimize ---------------------*/
    optimize();

    /*----- .text output  ---------------------*/
    for (auto& assem : assemblies) {
        output(assem->toString());
    }
}

int MIPS::getGlobOffset(IR::Var* var) {
    int i = 0;
    for (auto [name,globVar] : IR::Module::getGlobVars()) {
        if (name == var->name) {
            return i * 4;
        }
        ++i;
    }
    return -1;
}

int MIPS::getStackOffset(IR::Var* var) {
    return StackMemory::varToOffset[var];
}

void MIPS::optimize() {
}

void MIPS::irToMips(IR::Inst& inst) {
    switch (inst.op) {
    case IR::Op::Empty: ;
        break;
    case IR::Op::InStack:
        InStack(inst);
        break;
    case IR::Op::OutStack:
        OutStack(inst);
        break;
    case IR::Op::Assign:
        Assign(inst);
        break;
    case IR::Op::Add:
        Add(inst);
        break;
    case IR::Op::Sub:
        Sub(inst);
        break;
    case IR::Op::Mul:
        Mul(inst);
        break;
    case IR::Op::Div:
        Div(inst);
        break;
    case IR::Op::Mod:
        Mod(inst);
        break;
    case IR::Op::And:
        And(inst);
        break;
    case IR::Op::Or:
        Or(inst);
        break;
    case IR::Op::Neg:
        Neg(inst);
        break;
    case IR::Op::LoadImd:
        LoadImd(inst);
        break;
    case IR::Op::GetInt:
        GetInt(inst);
        break;
    case IR::Op::PrintInt:
        PrintInt(inst);
        break;
    case IR::Op::PrintStr:
        PrintStr(inst);
        break;
    case IR::Op::Cmp:
        Cmp(inst);
        break;
    case IR::Op::Alloca:
        Alloca(inst);
        break;
    case IR::Op::Load:
        Load(inst);
        break;
    case IR::Op::Store:
        Store(inst);
        break;
    case IR::Op::Br:
        Br(inst);
        break;
    case IR::Op::Bif0:
        Bif0(inst);
        break;
    case IR::Op::Call:
        Call(inst);
        break;
    case IR::Op::PushParam:
        PushParam(inst);
        break;
    case IR::Op::Ret:
        Ret(inst);
        break;
    default: Error::raise("Bad IR Op in MIPS gen");
    }
}
