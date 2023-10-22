#include "lexer/Lexer.h"
#include "parser/CompUnit.h"
#include "error/Error.h"

void compile(const std::string &inFile = "testfile.txt",
             const std::string &outFile = "output.txt",
             const std::string &errorFile = "error.txt") {
    Lexer::init(inFile, outFile);
    Error::errorFileStream = std::ofstream(errorFile);
    auto compUnit = CompUnit::parse();
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        compile(argv[1], "", argv[2]);
    } else {
        compile("testfile.txt", "", "error.txt");
    }
    return 0;
}

//#include <iostream>
//
//int main() {
//#ifdef __GNUC__
//    printf("Compiled with GCC or a GCC-compatible compiler\n");
//#endif
//
//#ifdef _MSC_VER
//    std::cout << "Compiled with Microsoft Visual C++" << std::endl;
//#endif
//
//#ifdef __clang__
//    std::cout << "Compiled with Clang" << std::endl;
//    std::cout << "Compiled with Clang version " << __clang_major__ << "."
//              << __clang_minor__ << "." << __clang_patchlevel__ << std::endl;
//#endif
//
//    return 0;
//}
