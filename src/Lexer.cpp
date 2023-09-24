//
// Created by Steel_Shadow on 2023/9/21.
//

#include "Lexer.h"

// outfile can be none
void Lexer::start(const char *inFile, const char *outFile) {
    auto i = std::ifstream(inFile);
    auto o = std::ofstream(outFile);

    if (!i) {
        std::cerr << "Read testfile.txt fail!" << std::endl;
        throw std::runtime_error("Read testfile.txt fail!");
    }

    getInstance().infile = new std::ifstream("testfile.txt");
    getInstance().outfile = new std::ofstream("output.txt");
}

// return: x
bool Lexer::next() {
    //todo:
    return true;
}

const std::string &Lexer::getToken() {
    return token;
}

LexType Lexer::getLexType() {
    return lexType;
}

LinkedHashMap<std::string, LexType> &Lexer::buildReserveWords() {
    auto map = new LinkedHashMap<std::string, LexType>;

    map->put("main", MAINTK);
    map->put("const", CONSTTK);
    map->put("int", INTTK);
    map->put("break", BREAKTK);
    map->put("continue", CONTINUETK);
    map->put("if", IFTK);
    map->put("else", ELSETK);
    map->put("!", NOT);
    map->put("&&", AND);
    map->put("||", OR);
    map->put("for", FORTK);
    map->put("getint", GETINTTK);
    map->put("printf", PRINTFTK);
    map->put("return", RETURNTK);
    map->put("+", PLUS);
    map->put("-", MINU);
    map->put("void", VOIDTK);
    map->put("*", MULT);
    map->put("/", DIV);
    map->put("%", MOD);
    map->put("<=", LEQ);
    map->put("<", LSS);
    map->put(">=", GEQ);
    map->put(">", GRE);
    map->put("==", EQL);
    map->put("!=", NEQ);
    map->put("=", ASSIGN);
    map->put(";", SEMICN);
    map->put(",", COMMA);
    map->put("(", LPARENT);
    map->put(")", RPARENT);
    map->put("[", LBRACK);
    map->put("]", RBRACK);
    map->put("{", LBRACE);
    map->put("}", RBRACE);

    return *map;
}

const LinkedHashMap<std::string, LexType> &Lexer::getReserveWords() {
    return reserveWords;
}

Lexer &Lexer::getInstance() {
    static Lexer instance;
    return instance;
}
