//
// Created by Steel_Shadow on 2023/10/26.
//

#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <fstream>
#include <memory>
#include <vector>

#include "frontend/lexer/NodeType.h"

// Intermediate Representation
// like the Parser, specific genIR method is distributed in respective AST node struct.
// IR generation is the 2nd pass (1st pass builds the AST).
namespace IR {
extern std::ofstream IRFileStream;

using Dimensions = std::vector<int>;

enum class Type {
    Void,
    Int,
};

int sizeOfType(Type type);

enum class Op {
    // not a valid Op, only for init
    Empty,

    // only for showing step into a new contiguous stack memory
    // IfStmt BigForStmt BlockStmt
    InStack,
    OutStack,

    // res(Var) = arg1(Temp)
    Assign,

    // res(Temp) = arg1(Temp) op arg2(Temp)
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    And,
    Or,

    // arg1(Temp) = -arg1
    Neg,

    LoadImd,

    GetInt,
    // res(Var) = getint()
    PrintInt,
    // arg1(Temp)
    PrintStr,
    // arg1(Label)

    // 1 0 -1
    Cmp,

    // allocates memory on the stack frame.
    // arg1: Var
    // arg2: size (number of element, times sizeof(type) in backend)
    Alloca,

    // index doesn't consider sizeof(type)
    Load,
    Store,

    // no condition jump to arg1(Label)
    Br,
    // if arg1(Temp)==0, jump to arg2(label)
    Bif0,

    Call,

    // arg1: value   | base address of array Var
    // arg2: nullptr | offset of array Var
    PushParam,
    Ret,
};

// Var Temp ConstVal Str
struct Element {
    virtual ~Element() = default;

    virtual std::string toString() const = 0;
};

struct Var : public Element {
    std::string name;
    int depth;

    bool cons; // const | var
    std::vector<int> dims; // At most 2 dimensions in our work.
    Type type;

    Var(std::string name, int depth, bool cons, const std::vector<int>& dims, Type type = Type::Int);

    std::string toString() const override;

    bool operator==(const Var& other);
};

// temp register
// ret id = -1
struct Temp : public Element {
    int id;
    Type type;

    explicit Temp(Type type = Type::Int);

    // function return Temp
    // id must be -1
    explicit Temp(int id, Type type);

    Temp(Temp const& other);

    std::string toString() const override;
};

struct ConstVal : public Element {
    int value;
    Type type;

    explicit ConstVal(int value, Type type = Type::Int);

    std::string toString() const override;
};

// No need to save the address of the string,
// it is done by the assembler.
// str_{id}
struct Str : public Element {
    static std::vector<std::string> MIPS_strings;
    int id;

    Str();

    std::string toString() const override;
};

struct Inst {
    Op op;
    std::unique_ptr<Element> res;
    std::unique_ptr<Element> arg1;
    std::unique_ptr<Element> arg2;

    Inst();

    Inst(Op op,
         std::unique_ptr<Element> res,
         std::unique_ptr<Element> arg1,
         std::unique_ptr<Element> arg2);

    void outputIR() const;

private:
    static std::string opToStr(Op anOperator);
};

// like llvm, Label and Temp share id allocator
// nameAndId: Function has no id
struct Label : public Element {
    std::string nameAndId; // nameAndId of BasicBlock

    explicit Label(std::string name, bool isFunc = false);

    std::string toString() const override;
};

// if (jump.op == Empty)
struct BasicBlock {
    // go to next BasicBlock
    Label label; // string is good for debugging
    std::vector<Inst> instructions;
    Inst jump; // Br Call

    // if isFunc, no suffix number
    explicit BasicBlock(std::string labelName, bool isFunc = false);

    void addInst(Inst inst);

    void outputIR() const;
};

using BasicBlocks = std::vector<std::unique_ptr<BasicBlock>>;

// main is a Function, whose name is "main"
// Function init with a basicBlocks has an empty BasicBlock (can be optimized)
class Function {
    std::string name;
    Type reType; // void int
    std::vector<Dimensions> params; // Param -> p | p[] | p[][...]
    BasicBlocks basicBlocks;

public:
    // id for BasicBlock & Temp
    // reset to 0 at start of Function
    static int idAllocator;

    Function(std::string name, Type reType,
             const std::vector<Dimensions>& params);

    void setBasicBlocks(BasicBlocks& bBlocks);

    const std::string& getName() const;

    const BasicBlocks& getBasicBlocks() const;
    static Type convertType(NodeType type);
};

// backend CodeGen should not rely on SymTab
// we should store information in IR
// opt: const GlobVar can be simplified to ConstVal
struct GlobVar {
    bool cons; // const | var
    std::vector<int> dims; // At most 2 dimensions in our work.

    // filled with 0 if not initialized
    std::vector<int> initVal;

    // the teaching team guarantees that
    // "whenever an array initialization exists, a value must be assigned to each array member."
    GlobVar(bool cons, const std::vector<int>& dims, std::vector<int>& initVal);

    GlobVar(bool cons, const std::vector<int>& dims);
};

// only one module in our work
// address of globVars start at $gp
class Module {
    std::string name;
    static std::vector<std::pair<std::string, GlobVar>> globVars;
    std::vector<std::unique_ptr<Function>> functions;

public:
    explicit Module(std::string name);

    static std::vector<std::pair<std::string, GlobVar>>& getGlobVars();

    std::vector<std::unique_ptr<Function>>& getFunctions();

    void outputIR() const;
};

Op NodeTypeToIROp(NodeType n);
} // namespace IR

template <>
struct std::hash<IR::Var*> {
    std::size_t operator()(const IR::Var* k) const noexcept {
        using std::size_t;
        using std::hash;
        using std::string;

        std::size_t seed = 0x771DE635;
        seed ^= (seed << 6) + (seed >> 2) + 0x68DBA875 + std::hash<std::string>()(k->name);
        seed ^= (seed << 6) + (seed >> 2) + 0x48E349E1 + static_cast<std::size_t>(k->depth);
        return seed;
    }
};

#endif //COMPILER_IR_H
