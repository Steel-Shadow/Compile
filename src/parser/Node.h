//
// Created by Steel_Shadow on 2023/9/29.
//

#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include "NodeType.h"
#include "Lexer.h"
#include <vector>
#include <unordered_map>
#include <string>

union Value {
    std::string *str{};
    int i;
};

class Node {
private:
    NodeType type;
    std::vector<Node *> children;

    static std::unordered_map<Node *, Value> values;

public:
    explicit Node(NodeType type);


    NodeType getType();

    Value getValue();

    void setValue(std::string &&str);

    void setValue(int i);

    void singleLex(NodeType t);

    void addChild(Node *node);

    ~Node();

    //todo: rule of 3/5
};

#endif //COMPILER_NODE_H
