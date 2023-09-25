#include "lexer/Lexer.h"

int main() {
    // 打开文件以进行读写
    Lexer::compile("testfile.txt", "output.txt");

    while (Lexer::getInstance()->next()) {
        Lexer::getInstance()->output();
    }
    return 0;
}
