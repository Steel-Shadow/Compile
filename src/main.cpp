#include "lexer/Lexer.h"
#include "parser/Parser.h" // conflict with mingw Parser.h

void compile(const std::string &inFile, const std::string &outFile) {
    auto *i = new std::ifstream(inFile);
    auto *o = new std::ofstream(outFile);

    if (!*i) {
        std::cerr << "Reading testfile.txt fails!" << std::endl;
        throw std::runtime_error("Reading testfile.txt fails!");
    }
    if (!*o) {
        std::cerr << "Writing testfile.txt fails!" << std::endl;
        throw std::runtime_error("Writing testfile.txt fails!");
    }

    auto lexer = Lexer::getInstance(i, o);
    auto parser = Parser::getInstance(*lexer, o);

    lexer->next();
    parser->CompUnit();
}

int main() {
    compile("testfile.txt", "output.txt");
    return 0;
}
