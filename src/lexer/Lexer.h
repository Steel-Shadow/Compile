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

#include "NodeType.h"
#include "Error.h"
#include "LinkedHashMap.h"

#include <sstream>
#include <fstream>
#include <string>
#include <utility>

typedef std::string Token;
typedef std::pair<NodeType, Token> Word;

class Lexer {
private:
    static Lexer *instance;

    Lexer(const std::string &inFile, const std::string &outFile);

    std::ofstream outFileStream;
    std::string fileContents;

    // pre-reading deep
    static constexpr size_t deep = 3;

    char c{}; // c = fileContents[pos[deep-1] - 1]
    int pos[deep]{}; // count from 1

public:
    int column[deep]{}; // count from 1
    int row[deep]{}; // count from 0
private:

    // synchronize output of parser and lexer
    bool first = true;
    NodeType lastLexType{};
    Token lastToken{};

private:
    Word words[deep];
    NodeType &lexType = words[0].first;
    Token &token = words[0].second;

    inline static LinkedHashMap<std::string, NodeType> buildReserveWords();

    LinkedHashMap<std::string, NodeType> reserveWords;

    char nextChar();

    void reserve(const Token &t, NodeType &l);

    void retract();

    void output();

    void updateWords(NodeType l, Token t);

public:
    // Singleton
    // parameter is only needed on the first call
    static Lexer *getInstance(const std::string &inFile = "", const std::string &outFile = "");

    Lexer(Lexer const &) = delete;

    void operator=(Lexer const &) = delete;
    // Singleton

    Word next();

    // n is the depth of pre-reading
    // 0 for now
    Word peek(int n = 0);

    bool findAssignBeforeSemicolon();

    NodeType *getLexTypePtr() const;

    std::ofstream &getOutFileStream();
};

#endif //COMPILER_LEXER_H
