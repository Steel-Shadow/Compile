//
// Created by Steel_Shadow on 2023/10/26.
//
#include "IR.h"
#include <iostream>
#include <utility>

#include "errorHandler/Error.h"
#include "config.h"

using namespace IR;

std::ofstream IR::IRFileStream;
std::vector<std::string> Str::MIPS_strings;

Module::Module(std::string name) :
    name(std::move(name)) {}

const std::vector<std::unique_ptr<Function>> &Module::getFunctions() const {
    return functions;
}

Inst::Inst(Op op,
           std::unique_ptr<Element> res,
           std::unique_ptr<Element> arg1,
           std::unique_ptr<Element> arg2) :
    op(op), res(std::move(res)),
    arg1(std::move(arg1)),
    arg2(std::move(arg2)) {}

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
        case Op::Store:
            return "Store";
        case Op::StoreDynamic:
            return "StoreDynamic";
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
        case Op::Alloca:
            return "Alloca";
        case Op::Load:
            return "Load";
        case Op::LoadPtr:
            return "LoadPtr";
        case Op::LoadDynamic:
            return "LoadDynamic";
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
        case Op::RetMain:
            return "RetMain";
        case Op::InStack:
            return "InStack";
        case Op::OutStack:
            return "OutStack";
        case Op::NewMove:
            return "NewMove";
        case Op::Leq:
            return "Leq";
        case Op::Lss:
            return "Lss";
        case Op::Geq:
            return "Geq";
        case Op::Gre:
            return "Gre";
        case Op::Eql:
            return "Eql";
        case Op::Neq:
            return "Neq";
        case Op::Bif1:
            return "Bif1";
        case Op::MulImd:
            return "MulImd";
        case Op::Mult4:
            return "Mult4";
        case Op::PushAddressParam:
            return "PushAddressParam";
        case Op::Not:
            return "Not";
        default:
            Error::raise("Bad IR op");
            return "Bad IR op";
    }
}


Label::Label(std::string name, bool isFunc) {
    static int idAllocator = 0;

    if (isFunc) {
        this->nameAndId = std::move(name);
    } else {
        this->nameAndId = std::move(name) + "_" +
                          std::to_string(idAllocator++);
    }
}

std::string Label::toString() const {
    return nameAndId;
}

GlobVar::GlobVar(
    bool cons,
    std::vector<int> dims,
    std::vector<int> initVal) :
    cons(cons),
    dims(std::move(dims)),
    initVal(std::move(initVal)) {}

GlobVar::GlobVar(bool cons, const std::vector<int> &dims) :
    cons(cons),
    dims(dims) {
    int size = 1;
    for (const int i: dims) {
        size *= i;
    }
    initVal = std::vector(size, 0);
}


Function::Function(std::string name, Type reType,
                   const std::vector<Param> &params) :
    name(std::move(name)),
    reType(reType),
    params(params) {
    idAllocator = 0;
}

void Function::moveBasicBlocks(BasicBlocks &&bBlocks) {
    basicBlocks = std::move(bBlocks);
}

void Module::outputIR() const {
    for (const auto &[ident, globVar]: globVars) {
#if defined(STDOUT_IR)
        std::cout << ident << '\n';
#endif

#if defined(FILEOUT_IR)
        IRFileStream << ident << '\n';
#endif
    }
    for (auto &i: mainFunction->getBasicBlocks()) {
        i->outputIR();
    }

    for (auto &i: functions) {
        for (auto &j: i->getBasicBlocks()) {
            j->outputIR();
        }
    }
}

const std::vector<std::pair<std::string, GlobVar>> &Module::getGlobVars() const {
    return globVars;
}

const Function &Module::getMainFunction() const {
    return *mainFunction;
}

void Module::setMainFunction(std::unique_ptr<Function> main_function) {
    mainFunction = std::move(main_function);
}

void Module::addFunction(std::unique_ptr<Function> function) {
    functions.emplace_back(std::move(function));
}

void Module::addGlobVar(std::string name, GlobVar globVar) {
    globVars.emplace_back(std::move(name), std::move(globVar));
}

int Function::idAllocator = 0;

const BasicBlocks &Function::getBasicBlocks() const {
    return basicBlocks;
}

std::vector<Param> Function::getParams() const {
    return params;
}

void BasicBlock::addInst(Inst inst) {
    instructions.push_back(std::move(inst));
}

BasicBlock::BasicBlock(std::string labelName, bool isFunc) :
    label(Label(std::move(labelName), isFunc)) {}

void BasicBlock::outputIR() const {
    using namespace std;
#if defined(STDOUT_IR)
    cout << label.nameAndId << ":" << '\n';
#endif
#if defined(FILEOUT_IR)
    IRFileStream << label.nameAndId << ":" << '\n';
#endif
    for (auto &i: instructions) {
        i.outputIR();
    }
}

Var::Var(std::string name,
         int depth,
         bool cons,
         const std::vector<int> &dims,
         Type type,
         SymType symType) :
    name(std::move(name)),
    depth(depth),
    cons(cons),
    dims(dims),
    type(type), symType(symType) {}

Var::Var(std::string name, int depth) :
    name(std::move(name)),
    depth(depth),
    cons(false), type(Type::Int) {}

std::string Var::toString() const {
    std::string s = name + "(" + std::to_string(depth) + ")";
    for (auto &i: dims) {
        s += "[" + std::to_string(i) + "]";
    }
    return s;
}

Temp::Temp(Type type) :
    type(type) {
    id = Function::idAllocator++;
}

Temp::Temp(int id, Type type) :
    id(id),
    type(type) {}

std::string Temp::toString() const {
    return "%" + std::to_string(id);
}

Temp::Temp(const Temp &other) {
    id = other.id;
    type = other.type;
}

ConstVal::ConstVal(int value, Type type) :
    value(value),
    type(type) {}

std::string ConstVal::toString() const {
    return std::to_string(value);
}

Op IR::NodeTypeToIROp(LexType n) {
    switch (n) {
        case LexType::PLUS:
            return Op::Add;
        case LexType::MINU:
            return Op::Sub;
        case LexType::MULT:
            return Op::Mul;
        case LexType::DIV:
            return Op::Div;
        case LexType::MOD:
            return Op::Mod;
        case LexType::AND:
            return Op::And;
        case LexType::OR:
            return Op::Or;
        case LexType::LEQ:
            return Op::Leq;
        case LexType::LSS:
            return Op::Lss;
        case LexType::GEQ:
            return Op::Geq;
        case LexType::GRE:
            return Op::Gre;
        case LexType::EQL:
            return Op::Eql;
        case LexType::NEQ:
            return Op::Neq;
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

bool IR::operator==(const Var &lhs, const Var &rhs) {
    return lhs.name == rhs.name
           && lhs.depth == rhs.depth;
}

std::size_t IR::hash_value(const Var &obj) {
    std::size_t seed = 0x2100EB1C;
    seed ^= (seed << 6) + (seed >> 2) + 0x0CCF5A17 + std::hash<std::string>()(obj.name);
    seed ^= (seed << 6) + (seed >> 2) + 0x6CBE80FE + static_cast<std::size_t>(obj.depth);
    return seed;
}
