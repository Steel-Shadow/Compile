#include "lexer/Lexer.h"
#include <iostream>

int main() {
    // 打开文件以进行读写
    Lexer lexer;
    lexer.compile("testfile.txt", "output.txt");

    while (lexer.next());
    return 0;
}



