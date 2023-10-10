//
// Created by Steel_Shadow on 2023/9/21.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include "NodeType.h"
#include "LinkedHashMap.h"

#include <string>
#include <fstream>

typedef std::string Token;
typedef std::pair<NodeType, Token> Word;

class Lexer {
private:
    Lexer(const std::string &inFile, const std::string &outFile);

    std::ofstream outFileStream;
    std::string fileContents;

    // pre-reading deep
    static constexpr size_t deep = 3;

    char c{}; // c = fileContents[pos[deep-1] - 1]
    int pos[deep]{}; // count from 1
    int column[deep]{}; // count from 1
    int row[deep]{}; // count from 0

    // synchronize output of parser and lexer
    bool first = true;
    NodeType lastLexType{};
    Token lastToken{};

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
    static Lexer &getInstance(const std::string &inFile = "", const std::string &outFile = "");

    Lexer(Lexer const &) = delete;

    void operator=(Lexer const &) = delete;
    // Singleton

    Word next();

    // n is the depth of pre-reading
    // 0 for now
    Word peek(int n = 0);

    bool findAssignBeforeSemicolon();

    NodeType &getLexType() const;

    std::ofstream &getOutFileStream();

    const int *getColumn() const;

    const int *getRow() const;
};

#endif //COMPILER_LEXER_H
