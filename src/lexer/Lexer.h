//
// Created by Steel_Shadow on 2023/9/21.
//
// C++单例模式真复杂，分离头文件和源代码！
// 2023年9月24日20:05:42
// 初入C++就敢上手设计模式，算了。我是**
// 2023年9月25日13:12:51
// 不过如此！

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "LexType.h"
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

union LexValue {
    constexpr LexValue() {}

    ~LexValue() {}

    std::string IDENFR;
    int INTCON{};
    std::string STRCON;
};

class Lexer {
private:
    static Lexer *instance;

    Lexer(std::ifstream *i, std::ofstream *o);

    std::ifstream *inFileStream;
    std::ofstream *outFileStream;

    char c{};
    int column{}; //count from 0
    int row{}; // count from 1
    std::string line{};

    std::string token{};
    LexType lexType{};

    // union is quite special!
    union LexValue {
        constexpr LexValue() {}

        ~LexValue() {}

        std::string IDENFR;
        int INTCON{};
        std::string STRCON;
    } lexValue{};

    static LinkedHashMap<std::string, LexType> buildReserveWords();

    LinkedHashMap<std::string, LexType> reserveWords;

    char nextChar();

    void reserve();

    void retract();

    static inline std::string lexTypeToStr(LexType lexType);

    void output();

public:
    // Singleton
    // parameter is only needed on the first call
    static Lexer *getInstance(std::ifstream *i = nullptr, std::ofstream *o = nullptr);

    Lexer(Lexer const &) = delete;

    void operator=(Lexer const &) = delete;
    // Singleton

    bool next();

    // n is the depth of pre-reading
    // 0 for now
    std::pair<LexType, LexValue> preRead(int n);

};

#endif //COMPILER_LEXER_H
