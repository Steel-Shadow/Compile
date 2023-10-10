//
// Created by Steel_Shadow on 2023/9/29.
//

#include "Node.h"

#include "lexer/Lexer.h"

std::unordered_map<Node *, Value> Node::values;

void Node::addChild(Node *node) {
    children.push_back(node);
}

Node::Node(NodeType type) : type(type) {}

Node::~Node() {
    for (Node *child: children) {
        delete child;
    }
    children.clear();

    if (type == NodeType::INTCON || type == NodeType::STRCON ||
        type == NodeType::IDENFR) {
        values.erase(this);
    }
}

NodeType Node::getType() {
    return type;
}

Value Node::getValue() {
    return values[this];
}

void Node::setValue(const Value &v) {
    values[this] = v;
}

// todo: 不要添加多余符号，此时我思考到需要重构
Node *Node::singleLex(NodeType t) {
    if (Lexer::getInstance().peek().first == t) {
        auto x = new Node(t);
        addChild(x);
        Lexer::getInstance().next();
        return x;
    } else {
        Error::raise_error();
        return nullptr;
    }
}
