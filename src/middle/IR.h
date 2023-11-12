//
// Created by Steel_Shadow on 2023/10/26.
//

#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <fstream>
#include <vector>
#include <memory>
#include "frontend/lexer/NodeType.h"

// Intermediate Representation
// like the Parser, specific genIR method is distributed in respective AST node struct.
// IR generation is the 2nd pass (1st pass builds the AST).
namespace IR {
    extern std::ofstream IRFileStream;

    using Dimensions = std::vector<int>;

    enum class Operator {
        Empty, // not a valid Op, only used for init

        Assign, // res(Val) = arg1(Temp)

        // res(Temp) = arg1(Temp) op arg2(Temp)
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        And,
        Or,

        Neg, // arg1(Temp) = -arg1

        LoadImd,

        GetInt, // res(Var) = getint()
        PrintInt, // arg1(Temp)
        PrintStr, // arg1(Label)

        Cmp, // 1 0 -1

        // allocates memory on the stack frame.
        // arg1: Var
        // arg2: size (number of element, times sizeof(type) in backend)
        Alloca,
        Load, // index doesn't consider sizeof(type)
        Store, // index doesn't consider sizeof(type)

        // no condition jump to arg1(Label)
        Br,
        // if arg1(Temp)==0, jump to arg2(label)
        Bif0,

        Call,

        // arg1: value   | base address of array Var
        // arg2: nullptr | offset of array Var
        PushParam,
        PopParam,
        Ret,
    };

    // Var Temp ConstVal Str
    struct Element {
        virtual ~Element() = default;

        virtual std::string toString() = 0;
    };

    // like llvm, Label and Temp share id allocator
    // nameAndId: ForCond_1
    struct Label : public Element {
        std::string nameAndId; // nameAndId of BasicBlock

        explicit Label(std::string name, bool isFunc = false);

        std::string toString() override;
    };

    struct Var : public Element {
        std::string name;
        int depth;

        bool cons; // const | var
        std::vector<int> dims; // At most 2 dimensions in our work.

        Var(std::string name, int depth, bool cons, const std::vector<int> &dims);

        std::string toString() override;
    };

    // temp register
    // ret id = -1
    struct Temp : public Element {
        int id;

        Temp();

        // function return Temp
        // id must be -1
        explicit Temp(int id);

        Temp(Temp const &other);

        std::string toString() override;
    };

    struct ConstVal : public Element {
        int value;

        explicit ConstVal(int value);

        std::string toString() override;
    };

    // No need to save the address of the string,
    // it is done by the assembler.
    // str_{id}
    struct Str : public Element {
        static std::vector<std::string> MIPS_strings;
        int id;

        Str();

        std::string toString() override;
    };

    struct Instruction {
        Operator op;
        std::unique_ptr<Element> res;
        std::unique_ptr<Element> arg1;
        std::unique_ptr<Element> arg2;

        Instruction();

        Instruction(Operator op,
                    std::unique_ptr<Element> res,
                    std::unique_ptr<Element> arg1,
                    std::unique_ptr<Element> arg2);

        void outputIR() const;

    private:
        static std::string opToStr(Operator anOperator);
    };

    // if (jump.op == Empty)
    // go to next BasicBlock
    struct BasicBlock {
        Label label; // string is good for debugging
        std::vector<Instruction> instructions;
        Instruction jump; // Br Call

        explicit BasicBlock(std::string labelName, bool isFunc = false);

        void addInst(Instruction inst);

        void outputIR();
    };

    using BasicBlocks = std::vector<std::unique_ptr<BasicBlock>>;

    // main is a Function, whose name is "main"
    // Function init with a basicBlocks has an empty BasicBlock (can be optimized)
    class Function {
        std::string name;
        NodeType reType; // void int
        std::vector<Dimensions> params; // Param -> p | p[] | p[][...]
        BasicBlocks basicBlocks;

    public:
        // id for BasicBlock & Temp
        // reset to 0 at start of Function
        static int idAllocator;

        Function(std::string name, NodeType reType, const std::vector<Dimensions> &params);

        void setBasicBlocks(BasicBlocks &bBlocks);

        const std::string &getName() const;

        const BasicBlocks &getBasicBlocks() const;
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
        GlobVar(bool cons, const std::vector<int> &dims, std::vector<int> &initVal);

        GlobVar(bool cons, const std::vector<int> &dims);
    };

    // only one module in our work
    // address of globVars start at $gp
    class Module {
        std::string name;
        std::vector<std::pair<std::string, GlobVar>> globVars;
        std::vector<std::unique_ptr<Function>> functions;

    public:
        explicit Module(std::string name);

        std::vector<std::pair<std::string, GlobVar>> &getGlobVars();

        std::vector<std::unique_ptr<Function>> &getFunctions();

        void outputIR();
    };

    Operator NodeTypeToIROp(NodeType n);
} // namespace IR
#endif //COMPILER_IR_H
