//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"

#include <sstream>
#include <iostream>

#include "error/Error.h"
#include "config.h"

std::ofstream Lexer::outFileStream{};
std::string Lexer::fileContents{};


char Lexer::c{}; // c = fileContents[pos[deep-1] - 1]
int Lexer::pos[deep]{}; // count from 1
int Lexer::column[deep]{}; // count from 1
int Lexer::row[deep]{}; // count from 0

bool Lexer::first = true;
NodeType Lexer::lastLexType{};
Token Lexer::lastToken{};

Word Lexer::words[deep];
NodeType &Lexer::curLexType = words[0].first;
Token &Lexer::curToken = words[0].second;

LinkedHashMap<std::string, NodeType>Lexer::reserveWords;

void Lexer::init(const std::string &inFile, const std::string &outFile) {
    reserveWords = buildReserveWords();

    auto inFileStream = std::ifstream(inFile);
    outFileStream = std::ofstream(outFile);

    if (!inFileStream) {
        throw std::runtime_error("Reading " + inFile + " fails!");
    }
    if (!outFileStream && !outFile.empty()) {
        throw std::runtime_error("Writing " + outFile + " fails!");
    }

    std::stringstream buffer;
    buffer << inFileStream.rdbuf();
    fileContents = buffer.str();

    for (int i = 0; i < deep; ++i) {
        next();
    }
}

// also return EOF
char Lexer::nextChar() {
    c = fileContents[pos[deep - 1]];

    pos[deep - 1]++;

    if (c == '\n') {
        row[deep - 1]++;
        column[deep - 1] = 0;
    } else { column[deep - 1]++; }

    return c;
}

// stop if you read NodeType::Lex_END
// or the file will be read in loop
Word Lexer::next() {
    // Ident
    // IntConst
    // FormatString
    while (isspace(nextChar()));

    if (pos[deep - 1] > fileContents.length()) {
        updateWords(NodeType::LEX_END, "");
        output();
        return words[0];
    }

    NodeType l;
    Token t = std::string(1, c);

    if (c >= '0' && c <= '9') {
        while (isdigit(nextChar())) {
            t += c;
        }
        // error: bad number
        l = NodeType::INTCON;
        retract();
    } else if (c == '_' || isalpha(c)) {
        while (nextChar(), c == '_' || isalpha(c) || isdigit(c)) {
            t += c;
        }
        reserve(t, l);
        retract();
    } else if (c == '/') { //q1
        nextChar();
        if (c == '/') { //q2
            while (nextChar() != '\n');
            // line comment //
            t = "";
            l = NodeType::COMMENT;
        } else if (c == '*') { //q5
            q5:
            while (nextChar() != '*');
            do {
                if (c == '*');
                else if (c == '/') {
                    break; //q7
                } else {
                    goto q5;
                }
            } while (nextChar());
            // block comment /**/
            t = "";
            l = NodeType::COMMENT;
        } else {
            t = "/"; //q4
            l = NodeType::DIV;
            retract();
        }
    } else if (c == '\"') {
        // STRCON
        while (nextChar()) {
            t += c;
            if (c == '\"') {
                break;
            } else if (c == '\\') {
                if (nextChar() == 'n') {
                    t += 'n';
                } else {
                    Error::raise_error();
                }
            } else; // error: bad char in format string
        }

        l = NodeType::STRCON;
    } else {
        // special operator +-*/ && &
        for (const auto &i: reserveWords) {
            std::string str = i.first;
            NodeType type = i.second;

            if (fileContents.substr(pos[deep - 1] - 1, str.length()) == str) {
                pos[deep - 1] += static_cast<int>(str.length()) - 1;

                t = str;
                l = type;
                break;
            }
        }
    }

    if (l != NodeType::COMMENT) {
        updateWords(l, t);
        output();
    } else {
        next();
    }

    return words[0];
}

Word Lexer::peek(int n) {
    return words[n];
}

void Lexer::retract() { pos[deep - 1]--; }

void Lexer::reserve(const Token &t, NodeType &l) {
    if (reserveWords.containsKey(t)) {
        l = reserveWords.get(t);
    } else {
        l = NodeType::IDENFR;
    }
}

LinkedHashMap<std::string, NodeType> Lexer::buildReserveWords() {
    auto map = new LinkedHashMap<std::string, NodeType>;
    // IDENFR
    // INTCON
    // STRCON
    map->put("main", NodeType::MAINTK);
    map->put("const", NodeType::CONSTTK);
    map->put("int", NodeType::INTTK);
    map->put("break", NodeType::BREAKTK);
    map->put("continue", NodeType::CONTINUETK);
    map->put("if", NodeType::IFTK);
    map->put("else", NodeType::ELSETK);
    map->put("&&", NodeType::AND);
    map->put("||", NodeType::OR);
    map->put("for", NodeType::FORTK);
    map->put("getint", NodeType::GETINTTK);
    map->put("printf", NodeType::PRINTFTK);
    map->put("return", NodeType::RETURNTK);
    map->put("+", NodeType::PLUS);
    map->put("-", NodeType::MINU);
    map->put("void", NodeType::VOIDTK);
    map->put("*", NodeType::MULT);
    map->put("/", NodeType::DIV);
    map->put("%", NodeType::MOD);
    map->put("<=", NodeType::LEQ);
    map->put("<", NodeType::LSS);
    map->put(">=", NodeType::GEQ);
    map->put(">", NodeType::GRE);
    map->put("==", NodeType::EQL);
    map->put("!=", NodeType::NEQ);
    map->put("!", NodeType::NOT);
    map->put("=", NodeType::ASSIGN);
    map->put(";", NodeType::SEMICN);
    map->put(",", NodeType::COMMA);
    map->put("(", NodeType::LPARENT);
    map->put(")", NodeType::RPARENT);
    map->put("[", NodeType::LBRACK);
    map->put("]", NodeType::RBRACK);
    map->put("{", NodeType::LBRACE);
    map->put("}", NodeType::RBRACE);

    return *map;
}

void Lexer::output() {
    if (first) {
        first = false;
        lastLexType = Lexer::curLexType;
        lastToken = curToken;
    } else {
        if (!(lastLexType == NodeType::LEX_EMPTY || lastLexType == NodeType::LEX_END)) {

#ifdef STDOUT_PRINT_LEXER
            std::cout << typeToStr(lastLexType) << " " << lastToken << std::endl;
#endif
#ifdef FILE_PRINT_LEXER
            outFileStream << typeToStr(lastLexType) << " " << lastToken << std::endl;
#endif
        }

        lastLexType = Lexer::curLexType;
        lastToken = curToken;
    }
}

void Lexer::updateWords(NodeType l, Token t) {
    for (int i = 0; i < deep - 1; ++i) {
        words[i] = words[i + 1];
        pos[i] = pos[i + 1];
        column[i] = column[i + 1];
        row[i] = row[i + 1];
    }
    words[deep - 1].first = l;
    words[deep - 1].second = std::move(t);
}

// distinguish between Exp and LVal in Stmt
// It's wrong if Cond is a kind of Exp, but our work doesn't require it.
bool Lexer::findAssignBeforeSemicolon() {
    for (int t = pos[0] - 1;
         t < fileContents.length() && fileContents[t] != ';';
         ++t) {
        if (fileContents[t] == '=') {
            return true;
        }
    }
    return false;
}
