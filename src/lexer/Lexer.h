//
// Created by Steel_Shadow on 2023/9/21.
//
// C++单例模式真复杂，分离头文件和源代码！

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "config.h"
#include "LexType.h"
#include "LinkedHashMap.h"

#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <list>

class Lexer {
private:
    static Lexer lexer;

    std::ifstream *infile{};
    std::ofstream *outfile{};

    char c{};
    int position{};
    int column{};
    int row{};

    std::string token{};
    LexType lexType{};
    int num{};

    static LinkedHashMap<std::string, LexType> &buildReserveWords();

    inline static const auto &reserveWords = buildReserveWords();

    // Singleton 构造函数和析构函数私有，以防止外部直接创建或删除实例
    Lexer() {}


public:
    static Lexer &getInstance();

    static void start(const char *i, const char *o);

    bool next();

    const std::string &getToken();

    LexType getLexType();

    const LinkedHashMap<std::string, LexType> &getReserveWords();

};


#endif //COMPILER_LEXER_H
