//
// Created by Steel_Shadow on 2023/9/25.
//

#include "Parser.h"

Parser *Parser::instance = nullptr;

Parser *Parser::getInstance() {
    if (instance == nullptr) {
        instance = new Parser();
    }

    return instance;
}
