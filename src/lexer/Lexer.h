//
// Created by Steel_Shadow on 2023/9/21.
//
// C++单例模式真复杂，分离头文件和源代码！
// 2023年9月24日20:05:42
// 初入C++就敢上手设计模式，算了。我是**

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "config.h"
#include "Error.h"
#include "LinkedHashMap.h"

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <sstream>

typedef std::string LexType;

class Lexer {
private:
    std::ifstream *inFilePtr{};
    std::ofstream *outFilePtr{};

    char c{};
    int position{};
    int column{}; //count from 0
    int row{}; // count from 1
    std::string line{};

    std::string token{};
    LexType lexType{};
    int num{};

    static LinkedHashMap<std::string, LexType> &buildReserveWords();

    inline static auto &reserveWords = buildReserveWords();

public:
    void compile(const char *i, const char *o);

    bool next();

    char nextChar();

    void reserve();

    void retract();
};


#endif //COMPILER_LEXER_H
