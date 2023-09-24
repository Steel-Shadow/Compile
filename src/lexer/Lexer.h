//
// Created by Steel_Shadow on 2023/9/21.
//
// C++单例模式真复杂，分离头文件和源代码！

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

typedef std::string LexType;

class Lexer {
private:
    //Singleton: init in compile(...)
    static Lexer lexer;

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

    // Singleton 构造函数和析构函数私有，以防止外部直接创建或删除实例
    Lexer() {}


public:
    static Lexer getInstance();

    static void compile(const char *i, const char *o);

    bool next();

    char nextChar();

    const std::string &getToken();

    std::string getLexType();

    const LinkedHashMap<std::string, LexType> &getReserveWords();

    void reserve();
};


#endif //COMPILER_LEXER_H
