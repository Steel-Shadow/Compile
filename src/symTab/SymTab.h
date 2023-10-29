//
// Created by Steel_Shadow on 2023/10/15.
//

#ifndef COMPILER_SYMTAB_H
#define COMPILER_SYMTAB_H

#include <unordered_map>
#include <string>
#include <memory>
#include "Symbol.h"

// Stack
// global -next-> SymTab... -next-> *cur
class SymTab {
    SymTab *prev; // prev SymTable

    std::vector<std::unique_ptr<SymTab>> next; // next SymTable

    // Ident to Symbol
    std::unordered_map<std::string, Symbol> symbols;

public:
    static SymTab *cur;
    static SymTab global;

    explicit SymTab(SymTab *prev);

    static bool reDefine(const std::string &ident);

    static Symbol *find(const std::string &ident);

    // const var
    // no effect if reDefine(ident)
    static void add(const std::string &ident,
                    bool cons,
                    const std::vector<int> &dims);

    // param
    // no effect if reDefine(ident)
    static void add(const std::string &ident,
                    const std::vector<int> &dims);

    // func
    // no effect if reDefine(ident)
    static void add(const std::string &ident,
                    NodeType reType,
                    const std::vector<Dimensions> &params,
                    SymTab *where = cur);

    static void deepIn();

    static void deepOut();

    SymTab *getPrev() const;
};


#endif //COMPILER_SYMTAB_H
