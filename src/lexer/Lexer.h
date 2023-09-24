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
    static Lexer *instance;

    Lexer(const std::string &inFile, const std::string &outFile);

    std::ifstream inFileStream;
    std::ofstream outFileStream;

    char c{};
    int column{}; //count from 0
    int row{}; // count from 1
    std::string line{};

    std::string token{};
    LexType lexType{};
    int num{};

    static LinkedHashMap<std::string, LexType> buildReserveWords();

    LinkedHashMap<std::string, LexType> reserveWords;

    char nextChar();

    void reserve();

    void retract();

    void output();

public:
    // Singleton begin
    static Lexer *getInstance(const std::string &inFile = "", const std::string &outFile = "");

    Lexer(Lexer const &) = delete;

    void operator=(Lexer const &) = delete;
    // Singleton end

    bool next();
};


#endif //COMPILER_LEXER_H
