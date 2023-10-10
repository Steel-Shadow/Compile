#include "lexer/Lexer.h"
#include "parser/Parser.h" // conflict with mingw Parser.h

using namespace std;

void compile(const string &inFile, const string &outFile) {
    auto lexer = Lexer::getInstance(inFile, outFile);
    auto parser = Parser::getInstance(*lexer);

    parser->CompUnit();
}

int main(int argc, char *argv[]) {
    if (argc == 3) {
        compile(argv[1], argv[2]);
    } else {
        compile("testfile.txt", "output.txt");
    }
    return 0;
}
