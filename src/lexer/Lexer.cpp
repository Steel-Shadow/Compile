//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"
#include <iostream>
LEXER_OUTPUT
void Lexer::f() {
    std::cout << "postion++" << position++ << std::endl;
}

Lexer::Lexer(int position, bool t) : position(position), t(t) {

}
