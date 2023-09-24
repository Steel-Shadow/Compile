#include "lexer/Lexer.h"
#include <iostream>

int main() {
    // 打开文件以进行读写
    Lexer::compile("testfile.txt", "output.txt");

    while (Lexer::getInstance().next());
    return 0;
}



