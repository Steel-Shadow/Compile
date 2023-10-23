//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_DECL_H
#define COMPILER_DECL_H

#include <memory>
#include <vector>

#include "Def.h"
#include "parser/stmt/Stmt.h"

class Btype {
    NodeType type;
public:
    static std::unique_ptr<Btype> parse();
};

// Decl → ConstDecl | VarDecl
// ConstDecl → 'const' BType ConstDef {','ConstDef}';'
// VarDecl → BType VarDef { ',' VarDef } ';'
class Decl : public BlockItem {
    bool cons;

    std::unique_ptr<Btype> btype;
    std::vector<std::unique_ptr<Def>> defs;

public:
    static std::unique_ptr<Decl> parse();
};

// wrap string
class Ident {
public :
    static std::string parse();
};

#endif //COMPILER_DECL_H
