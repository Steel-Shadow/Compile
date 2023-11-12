//
// Created by Steel_Shadow on 2023/10/26.
//
#include "IR.h"
#include <memory>
#include <utility>
#include <iostream>
#include "frontend/error/Error.h"

using namespace IR;

std::ofstream IR::IRFileStream;

Module::Module(std::string name) : name(std::move(name)) {}

std::vector<std::unique_ptr<Function>> &Module::getFunctions() {
    return functions;
}

std::vector<std::pair<std::string, GlobVar>> &Module::getGlobVars() {
    return globVars;
}

Instruction::Instruction(
        Operator op,
        std::unique_ptr<Element> res,
        std::unique_ptr<Element> arg1,
        std::unique_ptr<Element> arg2) :
        op(op), res(std::move(res)),
        arg1(std::move(arg1)),
        arg2(std::move(arg2)) {}

Instruction::Instruction() : op(Operator::Empty) {}

void Instruction::outputIR() const {
    using namespace std;
    if (op == Operator::Empty) {
        return;
    }
    cout << opToStr(op) << "\t"
         << (res ? res->toString() : "_") << "\t"
         << (arg1 ? arg1->toString() : "_") << "\t"
         << (arg2 ? arg2->toString() : "_") << "\t"
         << endl;
}

std::string Instruction::opToStr(Operator anOperator) {
    switch (anOperator) {
        case Operator::Empty:
            return "Empty";
        case Operator::Assign:
            return "Assign";
        case Operator::Add:
            return "Add";
        case Operator::Sub:
            return "Sub";
        case Operator::Mul:
            return "Mul";
        case Operator::Div:
            return "Div";
        case Operator::Mod:
            return "Mod";
        case Operator::And:
            return "And";
        case Operator::Or:
            return "Or";
        case Operator::Neg:
            return "Neg";
        case Operator::LoadImd:
            return "LoadImd";
        case Operator::GetInt:
            return "GetInt";
        case Operator::PrintInt:
            return "PrintInt";
        case Operator::PrintStr:
            return "PrintStr";
        case Operator::Cmp:
            return "Cmp";
        case Operator::Alloca:
            return "Alloca";
        case Operator::Load:
            return "Load";
        case Operator::Store:
            return "Store";
        case Operator::Br:
            return "Br";
        case Operator::Bif0:
            return "Bif0";
        case Operator::Call:
            return "Call";
        case Operator::PushParam:
            return "PushParam";
        case Operator::PopParam:
            return "PopParam";
        case Operator::Ret:
            return "Ret";
    }
}

GlobVar::GlobVar(
        bool cons,
        const std::vector<int> &dims,
        std::vector<int> &initVal) :
        cons(cons),
        dims(dims),
        initVal(initVal) {}

GlobVar::GlobVar(bool cons, const std::vector<int> &dims) :
        cons(cons),
        dims(dims) {
    int size = 1;
    for (int i: dims) {
        size *= i;
    }
    initVal = std::vector<int>(size, 0);
}


Function::Function(std::string name, NodeType reType, const std::vector<Dimensions> &params) :
        name(std::move(name)),
        reType(reType),
        params(params) {
    idAllocator = 0;
}

void Function::setBasicBlocks(BasicBlocks &bBlocks) {
    Function::basicBlocks = std::move(bBlocks);
}

void Module::outputIR() {
    using namespace std;
    for (const auto &i: globVars) {
        cout << i.first << endl;
    }
    for (auto &i: functions) {
        for (auto &j: i->getBasicBlocks()) {
            j->outputIR();
        }
    }
}

int Function::idAllocator = 0;

const std::string &Function::getName() const {
    return name;
}

const BasicBlocks &Function::getBasicBlocks() const {
    return basicBlocks;
}

void BasicBlock::addInst(Instruction inst) {
    instructions.push_back(std::move(inst));
}

BasicBlock::BasicBlock(std::string labelName, bool isFunc) :
        label(Label(std::move(labelName), isFunc)) {
}

void BasicBlock::outputIR() {
    using namespace std;
    cout << label.nameAndId << ":" << endl;

    for (auto &i: instructions) {
        i.outputIR();
    }
    jump.outputIR();
}

Var::Var(std::string name,
         int depth,
         bool cons,
         const std::vector<int> &dims) :
        name(std::move(name)),
        depth(depth),
        cons(cons),
        dims(dims) {}

std::string Var::toString() {
    std::string s;
    s = name + "(" + std::to_string(depth) + ")";
    for (auto &i: dims) {
        s += "[" + std::to_string(i) + "]";
    }
    return s;
}

Label::Label(std::string name, bool isFunc) {
    if (isFunc) {
        this->nameAndId = std::move(name);
    } else {
        this->nameAndId = std::move(name) + "_" + std::to_string(Function::idAllocator++);
    }
}

std::string Label::toString() {
    return nameAndId;
}

Temp::Temp() {
    id = Function::idAllocator++;
}

Temp::Temp(int id) : id(id) {}

std::string Temp::toString() {
    return "%" + std::to_string(id);
}

Temp::Temp(const Temp &other) {
    id = other.id;
}

ConstVal::ConstVal(int value) : value(value) {}

std::string ConstVal::toString() {
    return std::to_string(value);
}

Operator IR::NodeTypeToIROp(NodeType n) {
    switch (n) {
        case NodeType::PLUS:
            return Operator::Add;
        case NodeType::MINU:
            return Operator::Sub;
        case NodeType::MULT:
            return Operator::Mul;
        case NodeType::DIV:
            return Operator::Div;
        case NodeType::MOD:
            return Operator::Mod;
        case NodeType::AND:
            return Operator::And;
        case NodeType::OR:
            return Operator::Or;
        default:
            Error::raise("Bad IR Operator");
            return Operator::Empty;
    }
}

std::vector<std::string>Str::MIPS_strings;

Str::Str() {
    static int idAllocator = 0;
    id = idAllocator++;
}

std::string Str::toString() {
    return "str_" + std::to_string(id);
}
