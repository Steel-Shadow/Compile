//
// Created by Steel_Shadow on 2023/9/29.
//

#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <variant>

#include "lexer/NodeType.h"

typedef std::variant<std::string, int> Value;

class Node {
private:
    NodeType type;
    std::vector<Node *> children;

    static std::unordered_map<Node *, Value> values;

public:
    explicit Node(NodeType type);

    NodeType getType();

    Value getValue();

    void setValue(const Value &v);

    void singleLex(NodeType t);

    void addChild(Node *node);

    ~Node();

    //todo: rule of 3/5
};

#endif //COMPILER_NODE_H
