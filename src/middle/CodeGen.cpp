//
// Created by Steel_Shadow on 2023/10/26.
//
#include "CodeGen.h"

using namespace CodeGen;

std::ofstream CodeGen::mipsFileStream;

Module::Module(std::string name) : name(std::move(name)) {}

std::vector<Function> &Module::getFunctions() {
    return functions;
}

std::vector<std::pair<std::string, std::vector<int>>> &Module::getGlobals() {
    return globals;
}

Instruction::Instruction(
        Op op,
        const Operand &res,
        const Operand &arg1,
        const Operand &arg2) :
        res(res), op(op),
        arg1(arg1),
        arg2(arg2) {}

Operand::Operand(std::string name) : name(std::move(name)) {}

Operand::Operand(int i) : i(i) {}
