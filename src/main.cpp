#include "lexer/Lexer.h"
#include "parser/CompUnit.h"


void compile(const std::string &inFile, const std::string &outFile) {
    Lexer::getInstance(inFile, outFile);
    auto compUnit = CompUnit::parse();
}

int main(int argc, char *argv[]) {
    if (argc == 3) {
        compile(argv[1], argv[2]);
    } else {
        compile("testfile.txt", "output.txt");
    }
    return 0;
}
