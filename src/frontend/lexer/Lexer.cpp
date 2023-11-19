//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"

#include <sstream>
#include <utility>

#include "config.h"
#include "frontend/error/Error.h"
#include "tools/LinkedHashMap.h"

std::ofstream Lexer::outFileStream;
std::string Lexer::fileContents;

Word words[Lexer::deep];
NodeType& Lexer::curLexType = words[0].first;
Token& Lexer::curToken = words[0].second;

int Lexer::pos[deep]; // count from 1
int Lexer::column[deep]; // count from 1
int Lexer::row[deep]; // count from 1.
int& Lexer::curRow = row[0];

char c; // c = fileContents[posTemp - 1]
int posTemp;
int columnTemp;
int rowTemp{1};

// synchronize output of parser and lexer
bool firstOutput = true;
NodeType lastLexType;
Token lastToken;

LinkedHashMap<std::string, NodeType> reserveWords;

Word Lexer::peek(int n) {
    return words[n];
}

// also return EOF
char nextChar() {
    if (posTemp >= Lexer::fileContents.size()) {
        c = EOF;
        return EOF;
    }

    if (c == '\n') {
        rowTemp++;
        columnTemp = 1;
    } else {
        columnTemp++;
    }

    c = Lexer::fileContents[posTemp++];

    return c;
}

void reserve(const Token& t, NodeType& l) {
    if (reserveWords.containsKey(t)) {
        l = reserveWords.get(t);
    } else {
        l = NodeType::IDENFR;
    }
}

void output();

void updateWords(NodeType l, Token t);

// stop if you read NodeType::Lex_END
// or the file will be read in loop
Word Lexer::next() {
    // Ident
    // IntConst
    // FormatString

    if (posTemp > fileContents.length()) {
        updateWords(NodeType::LEX_END, "");
        output();
        return words[0];
    }

    NodeType lexType;
    Token token = std::string(1, c);

    if (c >= '0' && c <= '9') {
        while (isdigit(nextChar())) {
            token += c;
        }
        // error: bad number
        lexType = NodeType::INTCON;
        //        retract();
    } else if (c == '_' || isalpha(c)) {
        while (nextChar(), c == '_' || isalpha(c) || isdigit(c)) {
            token += c;
        }
        reserve(token, lexType);
        //        retract();
    } else if (c == '/') {
        // q1
        nextChar();
        if (c == '/') {
            // q2
            while (nextChar() != '\n') {
            }
            // line comment //
            token = "";
            lexType = NodeType::COMMENT;
        } else if (c == '*') {
            // q5
        q5:
            while (nextChar() != '*') {
            }
            do {
                if (c == '*') {
                } else if (c == '/') {
                    break; // q7
                } else {
                    goto q5;
                }
            } while (nextChar());
            // block comment /**/
            token = "";
            lexType = NodeType::COMMENT;
            nextChar();
        } else {
            token = "/"; // q4
            lexType = NodeType::DIV;
            //            retract();
        }
    } else if (c == '\"') {
        // STRCON
        while (nextChar() != EOF) {
            token += c;
            if (c == '\"') {
                break;
            } // error: bad char in format string
        }

        lexType = NodeType::STRCON;
        nextChar();
    } else {
        // special operator +-*/ && &
        for (const auto& [str, type] : reserveWords) {
            if (fileContents.substr(posTemp - 1, str.length()) == str) {
                posTemp += static_cast<int>(str.length());
                columnTemp += static_cast<int>(str.length());
                c = fileContents[posTemp - 1];

                token = str;
                lexType = type;
                break;
            }
        }
    }

    if (lexType == NodeType::COMMENT) {
        while (isspace(c)) {
            nextChar();
        }
        next();
    } else {
        updateWords(lexType, token);
        output();
        while (isspace(c)) {
            nextChar();
        }
    }

    return words[0];
}

LinkedHashMap<std::string, NodeType> buildReserveWords() {
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

void output() {
    if (firstOutput) {
        firstOutput = false;
        lastLexType = Lexer::curLexType;
        lastToken = Lexer::curToken;
    } else {
        if (!(lastLexType == NodeType::LEX_EMPTY ||
            lastLexType == NodeType::LEX_END)) {
#ifdef STDOUT_LEXER
            std::cout << typeToStr(lastLexType) << " " << lastToken
                      << '\n';
#endif
#ifdef FILEOUT_LEXER
            Lexer::outFileStream << typeToStr(lastLexType) << " " << lastToken
                                 << '\n';
#endif
        }

        lastLexType = Lexer::curLexType;
        lastToken = Lexer::curToken;
    }
}

void updateWords(NodeType l, Token t) {
    using namespace Lexer;
    for (int i = 0; i < deep - 1; ++i) {
        words[i] = words[i + 1];
        pos[i] = pos[i + 1];
        column[i] = column[i + 1];
        row[i] = row[i + 1];
    }

    pos[deep - 1] = posTemp;
    column[deep - 1] = columnTemp;
    row[deep - 1] = rowTemp;

    words[deep - 1].first = l;
    words[deep - 1].second = std::move(t);
}

// distinguish between Exp and LVal in Stmt
// It's wrong if Cond is a kind of Exp, but our work doesn't require it.
bool Lexer::findAssignBeforeSemicolon() {
    for (int t = pos[0] - 1;
         t < fileContents.length() && fileContents[t] != ';'; ++t) {
        if (fileContents[t] == '=') {
            return true;
        }
    }
    return false;
}

void Lexer::init(const std::string& inFile, const std::string& outFile) {
    reserveWords = buildReserveWords();

    auto inFileStream = std::ifstream(inFile);
    if (!inFileStream) {
        throw std::runtime_error("Reading " + inFile + " fails!");
    }

#if defined(FILEOUT_LEXER) || defined(FILEOUT_PARSER)
    outFileStream = std::ofstream(outFile);
    if (!outFileStream && !outFile.empty()) {
        throw std::runtime_error("Writing " + outFile + " fails!");
    }
#endif


    std::stringstream buffer;
    buffer << inFileStream.rdbuf();
    fileContents = buffer.str();

    while (isspace(nextChar())) {
    }
    for (int i = 0; i < deep; ++i) {
        next();
    }
}
