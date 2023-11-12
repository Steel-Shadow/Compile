#include "frontend/lexer/Lexer.h"
#include "frontend/error/Error.h"
#include "backend/CodeGen.h"
#include "middle/IR.h"
#include "AST/CompUnit.h"

void compile(const std::string &inFile,
             const std::string &outFile,
             const std::string &errorFile,
             const std::string &IRFile,
             const std::string &mipsFile) {
    Lexer::init(inFile, outFile);

    Error::errorFileStream = std::ofstream(errorFile);
    CodeGen::mipsFileStream = std::ofstream(mipsFile);
    IR::IRFileStream = std::ofstream(IRFile);

    auto compUnit = CompUnit::parse();
    auto module = compUnit->genIR();
    module->outputIR();
}

int main(int argc, char *argv[]) {
    if (argc == 5) {
        compile(argv[1], argv[2], argv[3], argv[4], argv[5]);
    } else {
        compile("testfile.txt", "", "", "", "mips.txt");
    }
    return 0;
}
