#include "lexer/Lexer.h"
#include "parser/CompUnit.h"
#include "error/Error.h"
#include "middle/CodeGen.h"

void compile(const std::string &inFile,
             const std::string &outFile,
             const std::string &errorFile,
             const std::string &mipsFile) {
    Lexer::init(inFile, outFile);
    Error::errorFileStream = std::ofstream(errorFile);
    CodeGen::mipsFileStream = std::ofstream(mipsFile);

    auto compUnit = CompUnit::parse();

    auto module = compUnit->codeGen();
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        compile(argv[1], "", argv[2], "");
    } else {
        compile("testfile.txt", "", "", "mips.txt");
    }
    return 0;
}
