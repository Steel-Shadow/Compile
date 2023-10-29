//
// Created by Steel_Shadow on 2023/10/26.
//

#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include <fstream>
#include <vector>

namespace CodeGen {
    extern std::ofstream mipsFileStream;

    enum class Op {
        Assign,
        Add,
        Sub,
        Mul,
        Div,
        Cmp, // 1 0 -1
        And,
        Or,
        Alloca, // allocates memory on the stack frame of the currently executing function
        Load,
        Store,
        At, // array []
        Br,
        Call,
        Ret,
    };

    class Operand {
        int i;
        std::string name;
    public:
        explicit Operand(int i);

        explicit Operand(std::string name);
    };

    class Instruction {
        Op op;
        Operand res;
        Operand arg1;
        Operand arg2;
    public:
        Instruction(Op op, const Operand &res, const Operand &arg1, const Operand &arg2);
    };

    class BasicBlock {
        std::string id;
        std::vector<Instruction> ins;
        Instruction jump; // Br Call
    };

    // main is a Function, whose name is "main"
    class Function {
        std::string name;
        std::vector<BasicBlock> basicBlocks;
    };

    // only one module in our work
    class Module {
        std::string name;
        std::vector<std::pair<std::string, std::vector<int>>> globals;
        std::vector<Function> functions;
    public:
        explicit Module(std::string name);

        std::vector<std::pair<std::string, std::vector<int>>> &getGlobals();

        std::vector<Function> &getFunctions();
    };
} // namespace CodeGen
#endif //COMPILER_CODEGEN_H
