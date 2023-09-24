#include "lexer/Lexer.h"

int main() {
    // 打开文件以进行读写
    Lexer::getInstance("testfile.txt", "output.txt");

    while (Lexer::getInstance()->next());
    return 0;
}
