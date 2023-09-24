//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"

Lexer *Lexer::instance = nullptr;

Lexer *Lexer::getInstance(const std::string &i, const std::string &o) {
    if (instance == nullptr) {
        instance = new Lexer(i, o);
        if (!instance->inFileStream) {
            std::cerr << "Read testfile.txt fail!" << std::endl;
            throw std::runtime_error("Read testfile.txt fail!");
        }
    }

    return instance;
}

Lexer::Lexer(const std::string &i, const std::string &o) {
    inFileStream = std::ifstream(i);
    outFileStream = std::ofstream(o);
    reserveWords = buildReserveWords();
}

// also return EOF
char Lexer::nextChar() {
    if (column == line.length()) {
        row++;
        column = 0;

        if (!std::getline(inFileStream, line)) {
            c = EOF;
            return c;
        }
        line += '\n'; // getline has no '\n'
    }
    c = line[column++];
    return c;
}

// no need to retract
bool Lexer::next() {
    // Ident
    // IntConst
    // FormatString
    // 单词类别码 单词的字符/字符串形式(中间仅用一个空格间隔)
    while (isspace(nextChar()));

    if (c == EOF) {
        return false;
    }

    token = std::string(1, c);

    if (c >= '0' && c <= '9') {
        while (isdigit(nextChar())) {
            token += c;
        }
        num = std::stoi(token);
        lexType = "INTCON";
        retract();
    } else if (c == '_' || isalpha(c)) {
        while (nextChar(), c == '_' || isalpha(c) || isdigit(c)) {
            token += c;
        }
        reserve();
        retract();
    } else if (c == '/') { //q1
        nextChar();
        if (c == '/') { //q2
            while (nextChar() != '\n');
            // line comment //
            token = "";
            lexType = "";
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
            token = "";
            lexType = "";
        } else {
            token = "/"; //q4
            lexType = "DIV";
            retract();
        }
    } else if (c == '\"') {
        // STRCON
        while (nextChar()) {
            token += c;
            if (c == '\"') {
                break;
            } else if (c == '\\') {
                if (nextChar() == 'n') {
                    token += 'n';
                } else {
                    Error::raise_error();
                }
            } else;
        }

        lexType = "STRCON";
    } else {
        // special operator +-*/ && &
        for (const auto &i: reserveWords) {
            std::string str = i.first;
            LexType type = i.second;

            if (line.substr(column - 1, str.length()) == str) {
                column += static_cast<int>(str.length()) - 1;

                token = str;
                lexType = type;
                break;
            }
        }
    }

    output();
    return true;
}

void Lexer::retract() { column--; }

void Lexer::reserve() {
    if (reserveWords.containsKey(token)) {
        lexType = reserveWords.get(token);
    } else {
        lexType = "IDENFR";
    }
}

LinkedHashMap<std::string, LexType> Lexer::buildReserveWords() {
    auto map = new LinkedHashMap<std::string, LexType>;
    // IDENFR
    // INTCON
    // STRCON
    map->put("main", "MAINTK");
    map->put("const", "CONSTTK");
    map->put("int", "INTTK");
    map->put("break", "BREAKTK");
    map->put("continue", "CONTINUETK");
    map->put("if", "IFTK");
    map->put("else", "ELSETK");
    map->put("&&", "AND");
    map->put("||", "OR");
    map->put("for", "FORTK");
    map->put("getint", "GETINTTK");
    map->put("printf", "PRINTFTK");
    map->put("return", "RETURNTK");
    map->put("+", "PLUS");
    map->put("-", "MINU");
    map->put("void", "VOIDTK");
    map->put("*", "MULT");
    map->put("/", "DIV");
    map->put("%", "MOD");
    map->put("<=", "LEQ");
    map->put("<", "LSS");
    map->put(">=", "GEQ");
    map->put(">", "GRE");
    map->put("==", "EQL");
    map->put("!=", "NEQ");
    map->put("!", "NOT");
    map->put("=", "ASSIGN");
    map->put(";", "SEMICN");
    map->put(",", "COMMA");
    map->put("(", "LPARENT");
    map->put(")", "RPARENT");
    map->put("[", "LBRACK");
    map->put("]", "RBRACK");
    map->put("{", "LBRACE");
    map->put("}", "RBRACE");

    return *map;
}

void Lexer::output() {
    if (lexType.empty() && token.empty()) { return; }
#ifdef DEBUG
    std::cout << lexType << " " << token << std::endl;
#endif
    outFileStream << lexType << " " << token << std::endl;
}
