//
// Created by Steel_Shadow on 2023/9/29.
//

#include "Node.h"

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

    if (type == NodeType::INTCON) {
        values.erase(this);
    } else if (type == NodeType::STRCON || type == NodeType::IDENFR) {
        delete values[this].str;
        values.erase(this);
    }
}

NodeType Node::getType() {
    return type;
}

void Node::setValue(std::string &&str) {
    Value v;
    v.str = &str;
    values[this] = v;
}

void Node::setValue(int i) {
    Value v;
    v.i = i;
    values[this] = v;
}

Value Node::getValue() {
    return values[this];
}

void Node::singleLex(NodeType t) {
    if (Lexer::getInstance()->peek().first == t) {
        addChild(new Node(t));
        Lexer::getInstance()->next();
    } else { Error::raise_error(); }
}
