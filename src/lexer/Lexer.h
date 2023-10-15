//
// Created by Steel_Shadow on 2023/9/21.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <fstream>

#include "NodeType.h"
#include "LinkedHashMap.h"

typedef std::string Token;
typedef std::pair<NodeType, Token> Word;

namespace Lexer {
    void init(const std::string &inFile, const std::string &outFile);

    extern std::ofstream outFileStream;
    extern std::string fileContents;

    // pre-reading deep
    static constexpr size_t deep = 3;

    extern char c; // c = fileContents[pos[deep-1] - 1]
    extern int pos[deep]; // count from 1
    extern int column[deep]; // count from 1
    extern int row[deep]; // count from 0

    // synchronize output of parser and lexer
    extern bool first;
    extern NodeType lastLexType;
    extern Token lastToken;

    extern Word words[deep];
    extern NodeType &curLexType;
    extern Token &curToken;

    LinkedHashMap<std::string, NodeType> buildReserveWords();

    extern LinkedHashMap<std::string, NodeType> reserveWords;

    char nextChar();

    void reserve(const Token &t, NodeType &l);

    void retract();

    void output();

    void updateWords(NodeType l, Token t);

    Word next();

    // n is the depth of pre-reading
    // 0 for now
    Word peek(int n = 0);

    bool findAssignBeforeSemicolon();
}

#endif //COMPILER_LEXER_H
