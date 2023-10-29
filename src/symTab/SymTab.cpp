//
// Created by Steel_Shadow on 2023/10/15.
//

#include "SymTab.h"
#include "error/Error.h"

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

void SymTab::add(const std::string &ident,
                 bool cons,
                 const std::vector<int> &dims) {
    cur->symbols.emplace(ident, Symbol(cons, dims));
}

void SymTab::add(const std::string &ident,
                 const std::vector<int> &dims) {
    cur->symbols.emplace(ident, Symbol(dims));
}

void SymTab::add(const std::string &ident,
                 NodeType reType,
                 const std::vector<Dimensions> &params,
                 SymTab *where) {
    where->symbols.emplace(ident, Symbol(reType, params));
}

void SymTab::deepIn() {
    cur->next.push_back(std::make_unique<SymTab>(cur));
    cur = cur->next.back().get();
}

void SymTab::deepOut() {
    cur = cur->prev;
}

SymTab *SymTab::getPrev() const {
    return prev;
}

SymTab::SymTab(SymTab *prev) : prev(prev) {}
