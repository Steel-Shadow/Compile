//
// Created by Steel_Shadow on 2023/9/21.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#define LEXER_OUTPUT //TODO:宏开关，Lexer输出

class Lexer {
public:
    explicit Lexer(int position = 0, bool t = false);

    void f();


private:
    int position;
    bool t;

};


#endif //COMPILER_LEXER_H
