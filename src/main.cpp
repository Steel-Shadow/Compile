#include "lexer/Lexer.h"
#include "parser/Parser.h"

int main() {
    // 打开文件以进行读写
    Lexer::getInstance("testfile.txt", "output.txt");
    Parser::getInstance();
    while (Lexer::getInstance()->next());
    return 0;
}
