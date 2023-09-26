//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"

Lexer *Lexer::instance = nullptr;

Lexer::Lexer(std::ifstream *i, std::ofstream *o) :
        inFileStream(i), outFileStream(o) {
    reserveWords = buildReserveWords();
}

Lexer *Lexer::getInstance(std::ifstream *i, std::ofstream *o) {
    if (instance == nullptr) {
        instance = new Lexer(i, o);
    }

    return instance;
}

// also return EOF
char Lexer::nextChar() {
    if (column == line.length()) {
        row++;
        column = 0;

        if (!std::getline(*inFileStream, line)) {
            c = EOF;
            return c;
        }
        line += '\n'; // getline has no '\n'
    }
    c = line[column++];
    return c;
}

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
        // error: bad number
        lexValue.INTCON = std::stoi(token);
        lexType = LexType::INTCON;
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
            lexType = LexType::COMMENT;
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
            lexType = LexType::COMMENT;
        } else {
            token = "/"; //q4
            lexType = LexType::DIV;
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
            } else; // error: bad char in format string
        }

        lexType = LexType::STRCON;
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

std::pair<LexType, LexValue> Lexer::preRead(int n) {
// todo:
// return nullptr;
}

void Lexer::retract() { column--; }

void Lexer::reserve() {
    if (reserveWords.containsKey(token)) {
        lexType = reserveWords.get(token);
    } else {
        lexType = LexType::IDENFR;
    }
}

LinkedHashMap<std::string, LexType> Lexer::buildReserveWords() {
    auto map = new LinkedHashMap<std::string, LexType>;
    // IDENFR
    // INTCON
    // STRCON
    map->put("main", LexType::MAINTK);
    map->put("const", LexType::CONSTTK);
    map->put("int", LexType::INTTK);
    map->put("break", LexType::BREAKTK);
    map->put("continue", LexType::CONTINUETK);
    map->put("if", LexType::IFTK);
    map->put("else", LexType::ELSETK);
    map->put("&&", LexType::AND);
    map->put("||", LexType::OR);
    map->put("for", LexType::FORTK);
    map->put("getint", LexType::GETINTTK);
    map->put("printf", LexType::PRINTFTK);
    map->put("return", LexType::RETURNTK);
    map->put("+", LexType::PLUS);
    map->put("-", LexType::MINU);
    map->put("void", LexType::VOIDTK);
    map->put("*", LexType::MULT);
    map->put("/", LexType::DIV);
    map->put("%", LexType::MOD);
    map->put("<=", LexType::LEQ);
    map->put("<", LexType::LSS);
    map->put(">=", LexType::GEQ);
    map->put(">", LexType::GRE);
    map->put("==", LexType::EQL);
    map->put("!=", LexType::NEQ);
    map->put("!", LexType::NOT);
    map->put("=", LexType::ASSIGN);
    map->put(";", LexType::SEMICN);
    map->put(",", LexType::COMMA);
    map->put("(", LexType::LPARENT);
    map->put(")", LexType::RPARENT);
    map->put("[", LexType::LBRACK);
    map->put("]", LexType::RBRACK);
    map->put("{", LexType::LBRACE);
    map->put("}", LexType::RBRACE);

    return *map;
}

std::string Lexer::lexTypeToStr(LexType lexType) {
    switch (lexType) {
        case LexType::COMMENT:
            return "COMMENT";
        case LexType::IDENFR:
            return "IDENFR";
        case LexType::INTCON:
            return "INTCON";
        case LexType::STRCON:
            return "STRCON";
        case LexType::MAINTK:
            return "MAINTK";
        case LexType::CONSTTK:
            return "CONSTTK";
        case LexType::INTTK:
            return "INTTK";
        case LexType::BREAKTK:
            return "BREAKTK";
        case LexType::CONTINUETK:
            return "CONTINUETK";
        case LexType::IFTK:
            return "IFTK";
        case LexType::ELSETK:
            return "ELSETK";
        case LexType::AND:
            return "AND";
        case LexType::OR:
            return "OR";
        case LexType::FORTK:
            return "FORTK";
        case LexType::GETINTTK:
            return "GETINTTK";
        case LexType::PRINTFTK:
            return "PRINTFTK";
        case LexType::RETURNTK:
            return "RETURNTK";
        case LexType::PLUS:
            return "PLUS";
        case LexType::MINU:
            return "MINU";
        case LexType::VOIDTK:
            return "VOIDTK";
        case LexType::MULT:
            return "MULT";
        case LexType::DIV:
            return "DIV";
        case LexType::MOD:
            return "MOD";
        case LexType::LEQ:
            return "LEQ";
        case LexType::LSS:
            return "LSS";
        case LexType::GEQ:
            return "GEQ";
        case LexType::GRE:
            return "GRE";
        case LexType::EQL:
            return "EQL";
        case LexType::NEQ:
            return "NEQ";
        case LexType::NOT:
            return "NOT";
        case LexType::ASSIGN:
            return "ASSIGN";
        case LexType::SEMICN:
            return "SEMICN";
        case LexType::COMMA:
            return "COMMA";
        case LexType::LPARENT:
            return "LPARENT";
        case LexType::RPARENT:
            return "RPARENT";
        case LexType::LBRACK:
            return "LBRACK";
        case LexType::RBRACK:
            return "RBRACK";
        case LexType::LBRACE:
            return "LBRACE";
        case LexType::RBRACE:
            return "RBRACE";
    }
}

void Lexer::output() {
    if (lexType == LexType::COMMENT) { return; }
#ifdef DEBUG
    std::cout << Lexer::lexTypeToStr(lexType) << " " << token << std::endl;
#endif
    *outFileStream << Lexer::lexTypeToStr(lexType) << " " << token << std::endl;
}
