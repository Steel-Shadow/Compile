//
// Created by Steel_Shadow on 2023/10/26.
//
#include "IR.h"
#include <iostream>
#include "frontend/error/Error.h"
#include "config.h"

using namespace IR;

std::ofstream IR::IRFileStream;
std::vector<std::pair<std::string, GlobVar>> Module::globVars;
std::vector<std::string> Str::MIPS_strings;

Module::Module(std::string name)
    : name(std::move(name)) {
}

std::vector<std::unique_ptr<Function>>& Module::getFunctions() {
    return functions;
}

std::vector<std::pair<std::string, GlobVar>>& Module::getGlobVars() {
    return globVars;
}

Inst::Inst(
    Op op,
    std::unique_ptr<Element> res,
    std::unique_ptr<Element> arg1,
    std::unique_ptr<Element> arg2)
    : op(op), res(std::move(res)),
      arg1(std::move(arg1)),
      arg2(std::move(arg2)) {
}

Inst::Inst():
    op(Op::Empty) {
}

void Inst::outputIR() const {
    using namespace std;
    if (op == Op::Empty) {
        return;
    }

#if defined(STDOUT_IR)
    cout << opToStr(op) << '\t'
    << (res ? res->toString() : "_") << '\t'
    << (arg1 ? arg1->toString() : "_") << '\t'
    << (arg2 ? arg2->toString() : "_") << '\t'
    << '\n';
#endif

#if defined(FILEOUT_IR)
    IRFileStream << opToStr(op) << '\t'
        << (res ? res->toString() : "_") << '\t'
        << (arg1 ? arg1->toString() : "_") << '\t'
        << (arg2 ? arg2->toString() : "_") << '\t'
        << '\n';
#endif
}

std::string Inst::opToStr(Op anOperator) {
    switch (anOperator) {
    case Op::Empty:
        return "Empty";
    case Op::Assign:
        return "Assign";
    case Op::Add:
        return "Add";
    case Op::Sub:
        return "Sub";
    case Op::Mul:
        return "Mul";
    case Op::Div:
        return "Div";
    case Op::Mod:
        return "Mod";
    case Op::And:
        return "And";
    case Op::Or:
        return "Or";
    case Op::Neg:
        return "Neg";
    case Op::LoadImd:
        return "LoadImd";
    case Op::GetInt:
        return "GetInt";
    case Op::PrintInt:
        return "PrintInt";
    case Op::PrintStr:
        return "PrintStr";
    case Op::Cmp:
        return "Cmp";
    case Op::Alloca:
        return "Alloca";
    case Op::Load:
        return "Load";
    case Op::Store:
        return "Store";
    case Op::Br:
        return "Br";
    case Op::Bif0:
        return "Bif0";
    case Op::Call:
        return "Call";
    case Op::PushParam:
        return "PushParam";
    case Op::Ret:
        return "Ret";
    case Op::InStack: return "InStack";
    case Op::OutStack: return "OutStack";
    default:
        Error::raise("Bad IR op");
        return "Bad IR op";
    }
}

Label::Label(std::string name, bool isFunc) {
    if (isFunc) {
        this->nameAndId = std::move(name);
    } else {
        this->nameAndId = std::move(name) + "_" +
            std::to_string(Function::idAllocator++);
    }
}

std::string Label::toString() const {
    return nameAndId + ":";
}

GlobVar::GlobVar(
    bool cons,
    const std::vector<int>& dims,
    std::vector<int>& initVal)
    : cons(cons),
      dims(dims),
      initVal(initVal) {
}

GlobVar::GlobVar(bool cons, const std::vector<int>& dims)
    : cons(cons),
      dims(dims) {
    int size = 1;
    for (const int i : dims) {
        size *= i;
    }
    initVal = std::vector<int>(size, 0);
}


Function::Function(std::string name, Type reType,
                   const std::vector<Dimensions>& params):
    name(std::move(name)),
    reType(reType),
    params(params) {
    idAllocator = 0;
}

void Function::setBasicBlocks(BasicBlocks& bBlocks) {
    basicBlocks = std::move(bBlocks);
}

void Module::outputIR() const {
    for (const auto& [fst, snd] : globVars) {
        std::cout << fst << '\n';
        IRFileStream << fst << '\n';
    }
    for (auto& i : functions) {
        for (auto& j : i->getBasicBlocks()) {
            j->outputIR();
        }
    }
}

int Function::idAllocator = 0;

const std::string& Function::getName() const {
    return name;
}

const BasicBlocks& Function::getBasicBlocks() const {
    return basicBlocks;
}

Type Function::convertType(NodeType type) {
    switch (type) {
    case NodeType::INTTK:
        return Type::Int;
    case NodeType::VOIDTK:
        return Type::Void;
    default:
        Error::raise("Bad Function reType");
        return Type::Void;
    }
}

void BasicBlock::addInst(Inst inst) {
    instructions.push_back(std::move(inst));
}

BasicBlock::BasicBlock(std::string labelName, bool isFunc)
    : label(Label(std::move(labelName), isFunc)) {
}

void BasicBlock::outputIR() const {
    using namespace std;
    cout << label.nameAndId << ":" << '\n';
    IRFileStream << label.nameAndId << ":" << '\n';

    for (auto& i : instructions) {
        i.outputIR();
    }
    jump.outputIR();
}

int IR::sizeOfType(Type type) {
    switch (type) {
    case Type::Void:
        return 0;
    case Type::Int:
        return 4;
    default:
        return 0;
    }
}

Var::Var(std::string name,
         int depth,
         bool cons,
         const std::vector<int>& dims,
         Type type) :
    name(std::move(name)),
    depth(depth),
    cons(cons),
    dims(dims),
    type(type) {
}

std::string Var::toString() const {
    std::string s = name + "(" + std::to_string(depth) + ")";
    for (auto& i : dims) {
        s += "[" + std::to_string(i) + "]";
    }
    return s;
}

bool Var::operator==(const Var& other) {
    return name == other.name && depth == other.depth;
}

Temp::Temp(Type type)
    : type(type) {
    id = Function::idAllocator++;
}

Temp::Temp(int id, Type type)
    : id(id),
      type(type) {
}

std::string Temp::toString() const {
    return "%" + std::to_string(id);
}

Temp::Temp(const Temp& other) {
    id = other.id;
    type = other.type;
}

ConstVal::ConstVal(int value, Type type):
    value(value),
    type(type) {
}

std::string ConstVal::toString() const {
    return std::to_string(value);
}

Op IR::NodeTypeToIROp(NodeType n) {
    switch (n) {
    case NodeType::PLUS:
        return Op::Add;
    case NodeType::MINU:
        return Op::Sub;
    case NodeType::MULT:
        return Op::Mul;
    case NodeType::DIV:
        return Op::Div;
    case NodeType::MOD:
        return Op::Mod;
    case NodeType::AND:
        return Op::And;
    case NodeType::OR:
        return Op::Or;
    default:
        Error::raise("Bad IR Operator");
        return Op::Empty;
    }
}

Str::Str() {
    static int idAllocator = 0;
    id = idAllocator++;
}

std::string Str::toString() const {
    return "str_" + std::to_string(id);
}
