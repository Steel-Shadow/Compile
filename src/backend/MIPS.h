//
// Created by Steel_Shadow on 2023/10/31.
//

#ifndef COMPILER_MIPS_H
#define COMPILER_MIPS_H

#include <fstream>

#include "middle/IR.h"
#include "Register.h"

// init register at beginning
namespace MIPS {
constexpr int wordSize = 4;
extern std::ofstream mipsFileStream;

struct Assembly {
    virtual ~Assembly() = default;

    virtual std::string toString() = 0;
};

extern std::vector<std::unique_ptr<Assembly>> assemblies;

// Label for MIPS instruction
struct Label : public Assembly {
    std::string nameAndId;

    explicit Label(const std::string &name_and_id);

    explicit Label(const IR::Label *label);

    std::string toString() override;
};

void irToMips(IR::Inst &inst);

void optimize();

void outputAll(IR::Module &module);

void output(const std::string &str, bool newLine = true);

}
#endif //COMPILER_MIPS_H
