//
// Created by Steel_Shadow on 2023/10/15.
//

#include "SymTab.h"
#include "errorHandler/Error.h"

SymTab SymTab::global{nullptr};

SymTab *SymTab::cur = &global;

bool SymTab::reDefine(const std::string &ident) {
    if (cur->symbols.find(ident) != cur->symbols.end()) {
        return true;
    }
    return false;
}

Symbol *SymTab::find(const std::string &ident) {
    for (auto p = cur; p != nullptr; p = p->prev) {
        auto it = p->symbols.find(ident);
        if (it != p->symbols.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

int SymTab::findDepth(const std::string &ident) {
    for (auto symTab = cur; symTab != nullptr; symTab = symTab->prev) {
        auto it = symTab->symbols.find(ident);
        if (it != symTab->symbols.end()) {
            return symTab->depth;
        }
    }
    return -1;
}

void SymTab::add(const std::string &ident, Symbol &&symbol, SymTab *where) {
    where->symbols.emplace(ident, std::move(symbol));
}

void SymTab::deepIn() {
    auto &newSymTab = cur->next.emplace_back(std::make_unique<SymTab>(cur));
    cur = newSymTab.get();
}

void SymTab::deepOut() {
    cur = cur->prev;
}

SymTab *SymTab::getPrev() const {
    return prev;
}

SymTab::SymTab(SymTab *prev) :
    prev(prev) {
    depth = prev == nullptr ? 0 : prev->depth + 1;
}

void SymTab::iterIn() {
    static int iterIndex = 0;

    if (iterIndex >= cur->next.size()) {
        iterIndex = 0;
        cur = cur->next[0].get();
    } else {
        cur = cur->next[iterIndex++].get();
    }
}

void SymTab::iterOut() {
    cur = cur->prev;
}

int SymTab::getDepth() const {
    return depth;
}
